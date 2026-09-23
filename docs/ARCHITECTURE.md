# tars-ml — Arquitetura da Biblioteca TinyML & Co-Design

`tars-ml` é uma **biblioteca TinyML de Co-Design Hardware/Software** construída do zero.
Ela foi desenhada para resolver um problema real: treinar e executar modelos de aprendizado
de máquina em dispositivos com recursos severamente restritos (microcontroladores, FPGAs,
ASICs dedicados), onde frameworks como PyTorch/TensorFlow e GPUs não entram.

---

## 🏛️ Os 3 Pilares Integrados do `tars-ml`

```text
 ┌──────────────────────────────────────────────────────────────────────────┐
 │                         tars-ml (TinyML Platform)                        │
 ├────────────────────────────┬────────────────────────────┬────────────────┤
 │ PILAR 1: Engine Rust (2024)│ PILAR 2: IP Core NPU       │ PILAR 3:       │
 │ (Treino, QAT & Runtime)    │ (SystemVerilog)            │ Co-Design      │
 │                            │                            │ Benchmarking   │
 │ • Zero dependências ML     │ • Sintetizável (FPGA/ASIC) │ • Via tars-viz │
 │ • Treino QAT direto:       │ • Módulo Parametrizado     │   (ferramenta  │
 │   - Q8.24 (Ponto Fixo 32b) │   (`parameter MODE`)       │   externa)      │
 │   - INT8 QAT (8 bits)      │ • Multiplicador Q8/INT8    │ • Acurácia (%) │
 │   - Ternário (-1, 0, +1)   │ • Mux 3:1 p/ Ternário      │ • Memória (B)  │
 │ • Runtime Rust de inferên- │ • CFE p/ CfC (v6-v8)       │ • Ciclos/Clock │
 │   cia portátil em pt fixo  │ • Interface .mem padrão    │ • Paridade 0   │
 └────────────────────────────┴────────────────────────────┴────────────────┘
```

### 1. Engine de Treino & Runtime de Inferência em Rust (Edition 2024) (Software)
- **Zero Dependências de ML**: Rust puro (Edition 2024), sem Python, sem CUDA, sem frameworks pesados (apenas `rand` para inicialização estocástica).
- **Treino Consciente da Precisão (QAT)**: o modelo aprende já na representação que o hardware vai executar:
  - **Q8.24**: Ponto fixo de 32 bits (1 sinal, 7 inteiros, 24 fração) — referência de alta precisão.
  - **INT8 QAT**: Quantização simétrica de 8 bits com escala aprendida.
  - **Ternário QAT**: Pesos restritos a {-1, 0, +1} com limiar adaptativo e Straight-Through Estimator (STE).
- **Runtime Portátil de Inferência**: além de exportar para a NPU em SystemVerilog, o código Rust de inferência é compilável diretamente para qualquer microcontrolador (ARM Cortex-M, RISC-V, ESP32) usando ponto fixo e compatibilidade com `no_std` — isso garante que o `tars-ml` funcione como biblioteca de software mesmo antes de a NPU física ser fabricada!

### 2. IP Core NPU em SystemVerilog (Hardware Parametrizado)
- **Módulo Parametrizado Universal**: um único módulo que adapta a lógica aritmética em tempo de compilação/síntese:
  ```systemverilog
  module npu_core #(
      parameter string MODE = "Q8_24",  // "Q8_24", "INT8", "TERNARY"
      parameter int IN_FEATURES  = 4,
      parameter int OUT_FEATURES = 1
  ) (
      input  logic clk,
      input  logic rst,
      input  logic start,
      output logic done,
      // barramento de interface padrão (ver MEM_FORMAT.md)
  );
  ```
- **Lógica Adaptativa por Modo**:
  - `Q8_24`: multiplicador 32x32 com acumulador estendido de 48 bits e ajuste de escala por deslocamento `[55:24]`.
  - `INT8`: multiplicador compacto 8x8 com acumulador de 32 bits.
  - `TERNARY`: substituição dos multiplicadores por **Multiplexadores 3:1** (se peso=+1 passa entrada; se peso=-1 inverte entrada; se peso=0 zera).
  - `v6-v8`: incorpora a **Unidade de Forma Fechada (CFE)** para redes CfC.

### 3. Framework de Co-Design Benchmarking & Observabilidade (`tars-viz`)
> 💡 **Arquitetura Formalizada**: a observabilidade e benchmarking são tratados como uma **ferramenta externa**
> ao motor do Tars (ver especificação completa em [VISUALIZATION.md](VISUALIZATION.md) e ADR-008 em [DECISIONS.md](DECISIONS.md)).
> A regra inegociável é: $\boxed{\text{visualizer depende do core; core nunca depende do visualizer}}$.
> O `tars-core` não possui dependências web nem de serialização, preservando a compatibilidade com `no_std`.

As métricas-alvo e capacidades do ecossistema visual incluem:
- **Acurácia (%) por Modo**: Q8.24 vs INT8 QAT vs Ternário QAT.
- **Pegada de Memória (Bytes)**: tamanho dos pesos exportados em cada formato.
- **Métricas de Síntese e Simulação**: contagem de LUTs/DSPs e ciclos de clock por inferência no testbench.
- **Paridade Software ↔ Hardware**: validação com zero erros de divergência entre a execução em Rust e a simulação em SystemVerilog.
- **Inspeção de Co-Design**: mapeamento interativo camada a camada entre o modelo em Rust e os blocos ativos da NPU.
- **Inspetor de Memória `.mem`**: alternância dinâmica entre Float, Q8.24, INT8, Hexadecimal e Binário.
- **Níveis de Detalhe (LOD 1 a 5)**: exploração escalável que vai da arquitetura global da rede até o bit individual do peso.
 
 ---
 
 ## 🎯 Aplicações Práticas Alvo (Casos de Uso TinyML)
 
 O `tars-ml` é projetado para operar onde computadores convencionais não conseguem:
 
 1. **Dispositivos Médicos Embarcados (Edge Healthcare)**:
    - Processamento contínuo em próteses, marcapassos e monitores de ECG/EEG sem aquecimento e com consumo na escala de miliwatts.
 2. **Robótica de Alta Velocidade e Drones Autônomos**:
    - Controle em tempo real lendo sensores inerciais (IMU) e atuando nos motores em ciclos sub-milissegundos com latência determinística.
 3. **Indústria 4.0 e Sensoriamento Inteligente**:
    - Análise de vibração e telemetria acústica direto na cabeça do sensor para manutenção preditiva, 100% offline.
 4. **Sistemas Aeroespaciais e Cubesats**:
    - Processamento de atitude e navegação em nanossatélites onde peso e energia solar disponível são críticos.
 
 ---
 
 ## 🚫 O que o `tars-ml` NÃO é
 
 - **Não é um concorrente do PyTorch/TensorFlow** para treinar modelos com bilhões de parâmetros em data centers.
 - **Não é um software "apenas de CPU"** nem um **hardware "sem software"** — seu valor está na união indivisível das duas partes.
 - **Não é uma caixa preta** — toda a matemática do Rust e a arquitetura do SystemVerilog são abertas e compreensíveis do nível de bit ao de algoritmo.
