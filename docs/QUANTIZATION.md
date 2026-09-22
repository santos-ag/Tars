# tars-ml — Precisão Numérica, Quantização (QAT) e Datapath de Hardware

Este documento detalha o funcionamento dos três modos numéricos do `tars-ml`,
a especificação do **Treino Consciente da Precisão (QAT)** e as regras do datapath
em SystemVerilog (acumuladores, saturação e Unidade de Função Especial — SFU).

---

## 💡 Por que Treino Consciente (QAT) em vez de Pós-Treino (PTQ)?

Na abordagem tradicional (Post-Training Quantization - PTQ), o modelo é treinado em
float32 e quantizado só no final. Isso causa perdas severas de acurácia, especialmente
em redes pequenas e em representações extremas como a ternária.

No `tars-ml`, adotamos o **Quantization-Aware Training (QAT)**:
- **Forward Pass no Rust**: o modelo simula exatamente a aritmética discretizada do hardware.
- **Backward Pass no Rust**: usa o **Straight-Through Estimator (STE)** para passar os gradientes
  através de operações não-deriváveis (arredondamentos e limiares):
  `d(quant(w)) / dw ≈ 1`
- **Resultado**: os pesos aprendem a compensar a baixa precisão durante o próprio treino,
  preservando a acurácia na NPU.

---

## 📊 Comparativo dos 3 Modos Numéricos

| Característica | Q8.24 (Ponto Fixo 32b) | INT8 QAT (8 bits) | Ternário QAT (-1, 0, +1) |
| :--- | :--- | :--- | :--- |
| **Papel no Projeto** | Âncora / Referência | Balanceado (Média Precisão) | Compressão Extrema (Ultra-low Power) |
| **Representação** | 1 bit sinal, 7 bits int, 24 bits frac | Inteiro com sinal de 8 bits | 2 bits codificados (01 = +1, 11 = -1, 00 = 0) |
| **Faixa de Valores** | -128.0 a +127.99999994 | -128 a +127 | {-1, 0, +1} |
| **Resolução Mínima** | 1 / 2^24 ≈ 0.0000000596 | 1 inteiro | Discreta (3 níveis) |
| **Economia Memória** | 1x (Base: 4 bytes/peso) | **4x menor** (1 byte/peso) | **16x menor** (1.58 bits/peso) |
| **Aritmética NPU** | Multiplicador 32x32 + shift | Multiplicador 8x8 | **Sem multiplicadores** (Muxes 3:1) |

---

## 🔬 Detalhamento por Modo Numérico

### 1. Ponto Fixo Q8.24 (Modo Âncora)
- **Conceito**: número inteiro de 32 bits em complemento de dois, onde os 24 bits inferiores
  representam a parte fracionária (`escala = 2^24 = 16777216`).
- **Conversão f32 ↔ Q8.24 em Rust**:
  ```rust
  pub fn f32_para_q8_24(v: f32) -> i32 {
      let clamped = v.clamp(-128.0, 127.9999);
      (clamped * 16777216.0).round() as i32
  }

  pub fn q8_24_para_f32(v: i32) -> f32 {
      (v as f32) / 16777216.0
  }
  ```
- **Treino QAT em Q8.24**: no forward pass, após cada operação em float/f32, o Rust trunca o
  resultado para a precisão equivalente de 24 bits fracionários. Isso garante que o treino
  não dependa da precisão extra de 32/64 bits da CPU.

---

### 2. INT8 QAT (Quantization-Aware Training)
- **Datapath**: `pesos int8 × ativações int8 → acumulador int32`.
- **Equação de Quantização Simétrica**:
  `q = clamp(round(w / S), -128, 127)`
  onde `S` é a escala calculada por camada (`S = max(|w|) / 127`).
- **Forward Pass no C++**:
  `w_quant = clamp(round(w / S), -128, 127) * S`
- **Requantização na NPU**: a saída do acumulador de 32 bits é multiplicada por uma escala de
  requantização de ponto fixo antes de ser enviada para a próxima camada.

---

### 3. Ternário QAT (-1, 0, +1)
- **Conceito**: restrição absoluta dos pesos ao conjunto `{-1, 0, +1}`.
- **Função de Limiarização (Thresholding)**:
  `w_ternario = +1 se w > +Delta`
  `w_ternario =  0 se |w| <= Delta`
  `w_ternario = -1 se w < -Delta`
  onde `Delta = 0.7 * média(|w|)` é atualizado a cada época.
- **Backward Pass (STE)**: o gradiente atualiza o peso contínuo latente `w` diretamente:
  `d(w_ternario) / dw ≈ 1`  para `|w| <= 1`, e `0` fora dessa faixa.
- **Vantagem Absoluta em Hardware**:
  - Elimina multiplicadores digitais na NPU.
  - O produto escalar vira um multiplexador 3:1:
    ```systemverilog
    always_comb begin
        case (w_code)
            2'b01:   mult_out =  activation; // W = +1
            2'b11:   mult_out = -activation; // W = -1
            default: mult_out =  32'sd0;      // W =  0
        endcase
    end
    ```

---

## 🛡️ Política de Acumulador e Saturação (Contrato Paridade C++ ↔ NPU)

Para garantir **zero erro de divergência de bits** entre o C++ e o SystemVerilog:

### 1. Largura do Acumulador
- **Q8.24**: um acumulador de 32 bits pode estourar (overflow) se somar mais de 128 produtos
  iguais a 1.0. Por isso, a NPU utiliza um **acumulador estendido de 48 bits** durante o
  laço de produto escalar.
- **INT8**: acumulador de **32 bits** (suporta até 16.000.000 de acúmulos int8 sem overflow).
- **Ternário**: acumulador com a mesma largura das ativações (32 bits Q8.24).

### 2. Regra de Saturação (Saturating Arithmetic)
Ao final da acumulação, a NPU reduz o resultado estendido para os 32 bits finais:
- Se o valor ultrapassar `+127.9999` (ou `0x7FFFFFFF`), ele é **saturado no máximo positivo**.
- Se for menor que `-128.0` (ou `0x80000000`), ele é **saturado no mínimo negativo**.
- O simulador C++ deve implementar a exata mesma regra de saturação.

---

## ⚡ Unidade de Função Especial (SFU — Special Function Unit)

A SFU é o bloco de hardware responsável por aplicar a função de ativação na saída do acumulador.

### 1. Ativação ReLU
- **Hardware**: comparador simples. Se a entrada for negativa, a saída é 0; caso contrário,
  passa o valor inalterado. Custo de hardware irrisório (algumas portas lógicas).

### 2. Ativação Sigmoid
- **Desafio no Q8.24**: uma tabela de consulta (LUT) ingênua exigiria `2^24` entradas (~16,7 milhões),
  o que é inviável em hardware.
- **Solução no Q8.24**: aproximação linear por partes (PWL - Piecewise Linear) com 16 segmentos
  no intervalo `[-8.0, +8.0]`. Fora dessa faixa, satura em `0.0` (para x < -8) e `1.0` (para x > +8).
- **Solução no INT8**: tabela LUT direta de **256 entradas** de 8 bits (extremamente leve,
  ocupa apenas 256 bytes de ROM na NPU).

---

## 🧩 Esboço do Módulo Parametrizado em SystemVerilog

```systemverilog
module npu_core #(
    parameter string MODE = "Q8_24",  // "Q8_24", "INT8", "TERNARY"
    parameter int IN_FEATURES  = 4,
    parameter int OUT_FEATURES = 1
) (
    input  logic clk,
    input  logic rst,
    input  logic start,
    input  logic signed [31:0] in_data,
    input  logic signed [31:0] in_weight,
    output logic signed [31:0] out_result,
    output logic done
);

    // Acumulador estendido para evitar overflow
    logic signed [47:0] acc;

    generate
        if (MODE == "TERNARY") begin : g_ternary
            // Mux 3:1 + somador no acc de 48 bits
        end else if (MODE == "INT8") begin : g_int8
            // Multiplicador 8x8 + somador no acc de 32 bits
        end else begin : g_q8_24
            // Multiplicador 32x32 -> trunca [55:24] + somador no acc de 48 bits
        end
    endgenerate

endmodule
```
