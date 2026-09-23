# tars-ml — Especificação do Formato de Memória (`.mem`)

Este documento estabelece o **contrato de interface entre o Rust (Exporter)** e o
**SystemVerilog (NPU / Testbench)**. Ele especifica como pesos, biases e vetores de teste
são salvos em disco para serem lidos pelo comando `$readmemh` do SystemVerilog.

---

## 📄 Visão Geral dos Arquivos

Para cada modelo treinado, o Rust gera 3 arquivos de texto hexadecimal na pasta de build:

| Arquivo | Conteúdo | Usado por |
| :--- | :--- | :--- |
| `model.mem` | Cabeçalho de topologia + todos os pesos e biases ordenados por camada | NPU (`npu_core.sv`) |
| `input.mem` | Vetores de entrada de teste | Testbench (`tb.sv`) |
| `expected.mem` | Saídas esperadas calculadas pelo Rust para validação de paridade | Testbench (`tb.sv`) |

---

## 🧱 Layout do Arquivo `model.mem`

O arquivo utiliza palavras de **32 bits em hexadecimal** (uma por linha).
Linhas iniciadas por `//` são comentários legíveis por humanos e são **ignoradas
pelo `$readmemh` nativo do Verilog**.

```text
// tars-ml model export v1
// MODE: Q8_24
// TOPOLOGY: 2 -> 2 -> 1
00000002    // Palavra 0: N = número de camadas com pesos (2)
00000002    // Palavra 1: entradas da camada 0 (in_features = 2)
00000002    // Palavra 2: saídas da camada 0 (out_features = 2)
00000001    // Palavra 3: saídas da camada 1 (out_features = 1)
// --- CAMADA 0: PESOS (2x2 = 4 palavras, ordem row-major: neurônio 0, neurônio 1) ---
00800000    // W[0][0] = +0.5 em Q8.24
FF800000    // W[0][1] = -0.5 em Q8.24
00C00000    // W[1][0] = +0.75 em Q8.24
FF400000    // W[1][1] = -0.75 em Q8.24
// --- CAMADA 0: BIASES (2 palavras) ---
00100000    // B[0] = +0.0625
00000000    // B[1] = 0.0
// --- CAMADA 1: PESOS (1x2 = 2 palavras) ---
01000000    // W[0][0] = +1.0
FF000000    // W[0][1] = -1.0
// --- CAMADA 1: BIASES (1 palavra) ---
00050000    // B[0] = +0.0195
```

---

## 🔢 Regras de Codificação Hexadecimal por Modo Numérico

Na versão **v0 a v6**, cada parâmetro ocupa **uma palavra inteira de 32 bits** no arquivo `.mem`
para manter o carregamento simples. (O empacotamento denso de 16 pesos ternários por palavra
é introduzido na **v7**).

### 1. Modo `Q8_24` (Ponto Fixo 32 bits)
- **Representação**: inteiro em complemento de dois de 32 bits.
- **Formatação Hex**: 8 dígitos hexadecimais sem sufixo.
- **Exemplos**:
  - `+1.0`  → `1 * 16777216` = `16777216` → `01000000`
  - `-1.0`  → `-16777216` em 32-bit comp-2 → `FF000000`
  - `+0.5`  → `8388608` → `00800000`
  - `-0.5`  → `-8388608` → `FF800000`

### 2. Modo `INT8` (Quantização 8 bits)
- **Representação**: o byte quantizado é armazenado nos **8 bits inferiores** da palavra de 32 bits.
  Os 24 bits superiores são estendidos com zero.
- **Exemplos**:
  - `+127` → `0000007F`
  - `-128` → `00000080` (em complemento de dois de 8 bits)
  - `0`    → `00000000`

### 3. Modo `TERNARY` (Pesos -1, 0, +1)
- **Representação**: o código de 2 bits é armazenado nos **2 bits inferiores** da palavra de 32 bits.
- **Tabela de Códigos**:
  - `2'b01` (`00000001` em hex) = **+1**
  - `2'b11` (`00000003` em hex) = **-1**
  - `2'b00` (`00000000` em hex) = **0**
  - `2'b10` = reservado (interpretado como 0)

---

## 🧪 Arquivos de Validação de Paridade (`input.mem` e `expected.mem`)

Para validar a NPU no testbench:

### `input.mem` (Exemplo para XOR com entrada [1.0, 0.0] em Q8.24)
```text
// Entrada de teste XOR [1.0, 0.0]
01000000    // In[0] = +1.0
00000000    // In[1] = 0.0
```

### `expected.mem` (Saída esperada do Rust para a mesma entrada)
```text
// Saida esperada do Rust para XOR [1.0, 0.0]
00FC0000    // Out[0] ≈ +0.984 em Q8.24
```

---

## 🛠️ Exemplo de Carregamento em SystemVerilog (`tb.sv`)

```systemverilog
module tb;
    logic signed [31:0] model_ram [0:1023];
    logic signed [31:0] test_input [0:15];
    logic signed [31:0] expected_out [0:15];

    initial begin
        // Carrega os arquivos gerados pelo Exporter Rust
        $readmemh("build/model.mem", model_ram);
        $readmemh("build/input.mem", test_input);
        $readmemh("build/expected.mem", expected_out);

        $display("Modelo carregado. Camadas: %0d", model_ram[0]);
    end
endmodule
```
