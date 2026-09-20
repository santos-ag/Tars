# tars-ml — Roteiro de Evolução & Co-Design Hardware/Software

Este documento estabelece o plano de evolução incremental do `tars-ml`:
desde os fundamentos de álgebra linear em C++17 até a síntese de **NPUs dedicadas em SystemVerilog**
para redes neurais de tempo contínuo (*Closed-form Continuous-time Neural Networks — CfC*).

---

## 📋 Documentação Relacionada

- [Índice & Glossário (`README.md`)](README.md)
- [Visão Geral da Arquitetura (`ARCHITECTURE.md`)](ARCHITECTURE.md) — Os 3 pilares e casos de uso TinyML.
- [Precisão Numérica & QAT (`QUANTIZATION.md`)](QUANTIZATION.md) — Modos Q8.24, INT8 e Ternário.
- [Formato de Memória `.mem` (`MEM_FORMAT.md`)](MEM_FORMAT.md) — Contrato de exportação C++ ↔ Verilog.
- [Onde Estamos Agora (`STATUS.md`)](STATUS.md) — Estado atual do código e próximo passo.
- [Decisões de Arquitetura (`DECISIONS.md`)](DECISIONS.md) — O registro de ADRs do projeto.

---

## 🧭 Como Ler e Executar este Roadmap

To manter a progressão fluida e evitar travamentos:

1. **Modo Âncora (`Q8_24`)**: é o caminho crítico obrigatório de **todas as versões**.
   Toda versão deve ser concluída e validada em `Q8_24` antes de avançar.
2. **Modos Estendidos (`INT8` e `TERNARY`)**: o C++ e a NPU suportam a capacidade em todas
   as versões, mas a validação formal com critérios de aceitação (DoD) é exigida nos
   **marcos de destaque** (v0, v0.5, v1, v6 e v7).
3. **Regra do "Não Empacar"**: se você travar na quantização/ternarização de uma versão complexa,
   finalize o marco na versão Âncora (`Q8_24`), registre o aprendizado no `STATUS.md` e avance.

---

## 📊 Matriz Tripla de Precisão por Versão

```text
 ┌─────────────────────────────────────────────────────────────────────────────┐
 │                         MATRIZ DE PRECISÃO POR VERSÃO                       │
 ├───────────────────┬───────────────────┬───────────────────┬─────────────────┤
 │ Versão            │ Q8.24 (Âncora)    │ INT8 QAT          │ TERNÁRIO QAT    │
 ├───────────────────┼───────────────────┼───────────────────┼─────────────────┤
 │ v0: Neural Core   │ Matrix Q8.24      │ Matrix INT8       │ Matrix Mux 3:1  │
 │ v0.5: Loop Closure│ Rede XOR na NPU   │ XOR INT8          │ XOR Ternária    │
 │ v1: Dense MLP     │ Systolic Q8.24    │ Systolic INT8     │ Systolic Ternary│
 │ v2: Spatial CNN   │ LineBuffer Q8.24  │ LineBuffer INT8   │ LineBuffer Tern │
 │ v3: Multi-Ch CNN  │ DoubleBuffer Q8   │ DoubleBuffer INT8 │ DoubleBuf Tern  │
 │ v4: Neural ODE    │ Integrator Q8.24  │ Integrator INT8   │ Integrator Tern │
 │ v5: LTC           │ Liquid Core Q8    │ Liquid Core INT8  │ Liquid Core Tern│
 │ v6: CfC           │ CFE Core Q8.24    │ CFE Core INT8     │ CFE Core Ternary│
 │ v7: Sparsidade    │ Packing & Skip Q8 │ Zero-Skip INT8    │ Zero-Skip Tern  │
 │ v8: Streaming Edge│ Event DMA Q8.24   │ Event DMA INT8    │ Event DMA Tern  │
 └───────────────────┴───────────────────┴───────────────────┴─────────────────┘
```

---

## 🔍 Detalhamento Versão por Versão

---

### v0 — Neural Core & Matrix Engine Parametrizada

> **Foco**: álgebra linear contígua, backpropagation analítico, treino QAT e produto escalar na NPU.

#### 1. Software (C++)
- Classes `Matrix<T>` e `Vector<T>` com alocação contígua unidimensional em memória (`std::vector<T> data` indexado por `row * cols + col`).
- Multiplicação matricial, soma vetorial, produto Hadamard e transposição.
- Derivadas analíticas para Sigmoid `sigma'(x) = sigma(x)(1 - sigma(x))` e ReLU `f'(x) = (x > 0) ? 1 : 0`.
- Treino QAT em Q8.24, INT8 e Ternário com STE.
- Exportador `Exporter.hpp` gerando `model.mem`, `input.mem` e `expected.mem`.

#### 2. Hardware: `npu_core #(parameter MODE)`
- Acumulador estendido de 48 bits para evitar overflow na acumulação Q8.24.
- Regra de saturação para trazer o acumulador de volta para 32 bits.
- SFU com ReLU combinacional.

#### 📋 Critérios de Conclusão (DoD - Definition of Done)
- [ ] C++: treino do XOR (2->2->1) e Regressão Linear converge com erro MSE < 0.01 em `Q8_24`.
- [ ] Exporter: gera os arquivos `.mem` válidos no formato especificado em `MEM_FORMAT.md`.
- [ ] NPU: simulação de produto escalar simples de 4 elementos no `tb.sv` bate com o C++ com **zero erros de divergência** nos 3 modos.

---

### v0.5 — Fechamento do Loop de Co-Design (Marco Intermediário)

> 💡 **Por que este marco existe?** Para evitar um salto gigante e arriscado entre o produto
> escalar simples da v0 e o Array Sistólico da v1. Aqui fechamos o ciclo completo de
> hardware e software na rede XOR inteira antes de aumentar a complexidade.

#### 1. Escopo de Co-Design
- Executar a rede XOR (2->2->1) **inteira em hardware**, camada por camada, no SystemVerilog.
- O testbench `tb.sv` lê o `model.mem` exportado pelo C++ e guia a NPU sequencialmente pelas 2 camadas.

#### 📋 Critérios de Conclusão (DoD)
- [ ] NPU: executa a inferência completa das 4 combinações do XOR em SystemVerilog.
- [ ] Paridade: resultado da NPU bate com a saída de `expected.mem` em `Q8_24` com **zero erros de bit**.
- [ ] Relatório: registrado o número de ciclos de clock por inferência no `STATUS.md`.

---

### v1 — Dense MLP & Array Sistólico

> **Foco**: redes multicamadas densas, classificação no dataset MNIST e aceleração sistólica.

#### 1. Software (C++)
- `DenseLayer(in_features, out_features)`.
- Softmax estável e Categorical Cross-Entropy (CCE) com gradiente analítico `grad = y_hat - y`.
- Otimizador SGD com Momentum (`v_t = beta * v_{t-1} + lr * grad`).
- Loader nativo para dataset MNIST (formato binário IDX).

#### 2. Hardware: `npu_systolic #(parameter MODE)`
- Array Sistólico 1D/2D (8x8 Processing Elements).
- Memória SRAM on-chip para pesos por camada.

#### 📋 Critérios de Conclusão (DoD) *(Metas Iniciais — Recalibrar Empiricamente)*
- [ ] Acurácia no MNIST (Q8.24 Âncora): > 95% no conjunto de teste.
- [ ] Acurácia estendida (INT8 QAT): > 93% (hipótese).
- [ ] Acurácia estendida (Ternário QAT): > 90% (hipótese).
- [ ] Paridade NPU: zero erros em amostragem de 100 imagens de teste do MNIST.

---

### v2 — Spatial CNN 2D & Line-Buffer Engine

> **Foco**: convoluções bidimensionais e processamento espacial com buffer de linha.

#### 1. Software (C++)
- `Conv2D`, `MaxPool2D`, `AvgPool2D`, `Flatten`.
- Algoritmo `im2col` + GEMM adaptado aos modos de precisão.
- Otimizador Adam (Adaptive Moment Estimation).

#### 2. Hardware: `npu_cnn #(parameter MODE)`
- Line-Buffer Engine com registradores de deslocamento para janelas KxK.
- Max-Pooling integrado em hardware.

#### 📋 Critérios de Conclusão (DoD)
- [ ] Acurácia no MNIST (Q8.24 Âncora): > 98%.
- [ ] NPU: convolução de imagem 28x28 executada com Line-Buffer sem acessos redundantes à memória externa.

---

### v3 — Multi-channel CNN & Double Buffering

> **Foco**: sinais multicanais (RGB, vibração multi-eixo), BatchNorm e gerenciamento de memória.

#### 1. Software (C++)
- Convolução multicanal `Cin -> Cout`, `BatchNorm2D` (com fusão na inferência) e `SpatialDropout`.
- Otimizador AdamW + Cosine Annealing Learning Rate Scheduler.
- Loader nativo para dataset CIFAR-10.

#### 2. Hardware: `npu_multichannel #(parameter MODE)`
- Double Buffering Ping-Pong SRAM (carrega canal C+1 enquanto processa C).

#### 📋 Critérios de Conclusão (DoD)
- [ ] Acurácia no CIFAR-10 (Q8.24 Âncora): > 75%.
- [ ] NPU: fusão de BatchNorm no peso verificada no testbench.

---

### v4 — Neural ODE & Integrador Temporal

> **Foco**: equações diferenciais ordinárias parametrizadas por redes neurais para trajetórias contínuas.

#### 1. Software (C++)
- Formulation: `dh(t)/dt = f_theta(h(t), t)`.
- Solvers em C++: Euler e Runge-Kutta 4ª Ordem (RK4).
- Backpropagation via Adjoint State Method com QAT.

#### 2. Hardware: `npu_ode #(parameter MODE)`
- Integrator Pipeline Engine re-alimentando os 4 estágios do RK4 no núcleo matricial.

#### 📋 Critérios de Conclusão (DoD)
- [ ] C++: reconstrução de trajetória física sintética (pêndulo/espiral) com erro MSE < 0.05.
- [ ] NPU: pipeline de RK4 executa a integração temporal em ciclos determinísticos.

---

### v5 — LTC (Liquid Time-Constant Core)

> **Foco**: redes bio-inspiradas com constantes de tempo adaptativas para séries temporais irregulares.

#### 1. Software (C++)
- Célula LTC com condutâncias sinápticas solúveis.
- Otimizador AdamW com clipping rígido de gradientes (`|grad| <= 1.0`).

#### 2. Hardware: `npu_liquid #(parameter MODE)`
- Non-linear Exponential Solver Core para avaliação de sigmoides e exponenciais.

#### 📋 Critérios de Conclusão (DoD)
- [ ] C++: predição de série temporal com amostragem irregular/gaps de dados superando baseline RNN.
- [ ] NPU: registradores de feedback de estado interno atualizados sem corrupção.

---

### v6 — CfC (Closed-Form Continuous-Time NPU)

> **Foco**: solução analítica em forma fechada para redes contínuas com complexidade O(1) na inferência.

#### 1. Software (C++)
- Célula CfC em forma fechada: `h(t) ≈ (f(x, h0) ⊙ e^(-[A(x, h0) + b] * t)) + g(x, h_0)`.
- Ativação SiLU/Swish e Tanh com treino QAT.

#### 2. Hardware: `npu_cfc #(parameter MODE)`
- Fast Closed-Form Engine (CFE) com 3 sub-blocos matriciais paralelos e cálculo de e^(-x) em ponto fixo via CORDIC ou PWL.

#### 📋 Critérios de Conclusão (DoD)
- [ ] C++: modelo CfC atinge paridade de acurácia com LTC no problema de controle, mas com inferência O(1) sem passos do solver.
- [ ] NPU: avaliação de h(t) executada em número fixo de ciclos determinísticos por amostra.

---

### v7 — Sparsidade & Eficiência Energética (CfC Otimizada)

> **Foco**: compressão e zeragem de computação para o modelo CfC em modo Ternário e INT8.

#### 1. Software (C++)
- Treino com indução de esparsidade (Sparsity-Aware QAT) forçando grande percentual de pesos nulos (`w = 0`).
- Exportador de densidade com empacotamento denso (16 pesos ternários de 2 bits por palavra de 32 bits).

#### 2. Hardware: `npu_cfc_sparse #(parameter MODE)`
- **Zero-Value Skipping**: circuito que detecta pesos `0` e pula aoperação no ciclo de clock.
- Lógica de empacotamento e desempacotamento de bits na leitura da SRAM.

#### 📋 Critérios de Conclusão (DoD)
- [ ] C++: modelo CfC ternário esparso retém >= 95% da acurácia do modelo Q8.24 em tarefa de controle.
- [ ] NPU: redução mensurável de ciclos de clock proporcional ao percentual de pesos nulos.

---

### v8 — Streaming Edge & Standby Ativo

> **Foco**: processamento assíncrono direto de sensores de borda com consumo de miliwatts.

#### 1. Software (C++)
- Pipeline orientado a eventos com zero alocação dinâmica (`no-malloc`).

#### 2. Hardware: `npu_streaming #(parameter MODE)`
- Interface Direct Sensor DMA lendo diretamente do barramento do sensor inercial/ECG.
- Counter de tempo injetando o variação temporal `dt` automaticamente no cálculo da CfC.
- **Wake-on-Event Logic**: NPU em estado de sono (*sleep mode*) mantendo o estado na SRAM, despertando apenas na chegada de novo evento.

#### 📋 Critérios de Conclusão (DoD)
- [ ] C++: código de inferência compila sem warnings de alocação de memória e executa em tempo real.
- [ ] NPU: simulação no `tb.sv` demonstra o ciclo de *sleep -> wake -> inferência -> sleep*.

---

## 📐 Framework de Co-Design Benchmarking *(Em Definição pela Equipe)*

> ⚠️ **Status**: a equipe está estudando a melhor forma gráfica/interativa
> de apresentar esses dados. As métricas mínimas obrigatórias a cada versão são:

1. **Paridade Numérica**: 0 erros de bit entre C++ e SystemVerilog.
2. **Memória**: bytes ocupados no arquivo `.mem`.
3. **Ciclos/Clock**: ciclos medidos no `tb.sv` por inferência.

---

## 📚 Referências Bibliográficas

1. **CfC (Closed-Form Continuous-time Networks)**:
   Hasani, R., Lechner, M., Amini, A., Rus, D. et al. *"Closed-form continuous-time neural networks"*. Nature Machine Intelligence, vol. 4, pp. 992–1003, 2022.
2. **LTC (Liquid Time-Constant Networks)**:
   Hasani, R., Lechner, M. et al. *"Liquid Time-constant Networks"*. AAAI Conference on Artificial Intelligence, 2021.
3. **QAT (Quantization-Aware Training)**:
   Jacob, B. et al. *"Quantization and Training of Neural Networks for Efficient Integer-Arithmetic-Only Inference"*. CVPR, 2018.
4. **Pesos Ternários**:
   Li, F., Zhang, B., Liu, B. *"Ternary Weight Networks"*. arXiv:1605.04711, 2016.
   Ma, S. et al. *"The Era of 1-bit LLMs: All Large Language Models are in 1.58 Bits"*. Microsoft Research, 2024.
