# tars-ml — Roteiro de Modelos & Arquiteturas de NPU

Este documento detalha o plano de evolução incremental do `tars-ml`:
desde as bases matemáticas de matrizes em C++ até a emulação/síntese de NPUs
dedicadas para redes neurais de tempo contínuo (*Closed-form Continuous-time Neural Networks — CfC*).

---

## Índice

1. [Diretrizes de Implementação](#diretrizes-de-implementação)
2. [Tabela Resumo das Versões](#tabela-resumo-das-versões)
3. [Detalhamento Versão por Versão](#detalhamento-versão-por-versão)
   - [v0 — Neural Core](#v0--neural-core)
   - [v1 — MNIST MLP](#v1--mnist-mlp)
   - [v2 — MNIST CNN](#v2--mnist-cnn)
   - [v3 — CIFAR-10 CNN](#v3--cifar-10-cnn)
   - [v4 — Neural ODE](#v4--neural-ode)
   - [v5 — LTC (Liquid Time-Constant)](#v5--ltc-liquid-time-constant)
   - [v6 — CfC (Closed-Form Continuous-Time)](#v6--cfc-closed-form-continuous-time)
   - [v7 — Quantized CfC](#v7--quantized-cfc)
   - [v8 — Streaming CfC](#v8--streaming-cfc)
4. [Métricas de Validação de NPU](#métricas-de-validação-de-npu)

---

## Diretrizes de Implementação

1. **Zero bibliotecas externas de ML:** Toda álgebra, otimizadores e funções de ativação
   devem ser codificados a mão em C++17 puro.
2. **Separação de Software e NPU:**
   - **Software:** Código C++ rodando na CPU (suporta treino com autodiff/backprop ou
     gradientes analíticos).
   - **NPU:** Arquitetura de hardware emulada/simulada (ciclo-exata ou functional simulator),
     focada em **inferência de baixa latência** e/ou aceleração de laços críticos.
3. **Casamento de Precisão:** O modelo treinado em software expõe exportação de pesos
   e biases para o formato binário/layout de memória consumido pela NPU correspondente.

---

## Tabela Resumo das Versões

| Versão | Modelo / Arquitetura | Dataset / Alvo | Ativações / Módulo | Otimizador | NPU Dedicada |
| :---: | :--- | :--- | :--- | :--- | :--- |
| **v0** | Neural Core | Sintético / Validação | Identity, ReLU, Sigmoid | SGD Simples | **Matrix/Tensor Engine** (MAC Array + Accumulator) |
| **v1** | MNIST MLP | MNIST (28x28) | ReLU, Softmax, Cross-Entropy | SGD com Momentum | **Dense NPU** (Sistolic Array 1D/2D) |
| **v2** | MNIST CNN | MNIST (28x28) | Conv2D, Max/Avg Pooling, ReLU | Adam | **CNN NPU** (Line-buffer / Sliding Window Engine) |
| **v3** | CIFAR-10 CNN | CIFAR-10 (32x32x3) | Multi-channel Conv, BatchNorm, Spatial Dropout | AdamW + Cosine Decay | **Multi-channel NPU** (Dataflow Interconnect + Double Buffering) |
| **v4** | Neural ODE | Séries Temporais / Regressão | Solvers ODE (Euler, Runge-Kutta RK4) | Adam / RMSprop | **ODE NPU** (Integrator Pipeline + Adaptive Stepper) |
| **v5** | LTC | Séries Irregulares | Recorrência contínua $dx/dt = -\frac{x}{\tau} + f(x,I)$ | AdamW | **Liquid NPU** (Non-linear Exponential Solver Core) |
| **v6** | CfC | Controle / Séries Temporais | Solução em forma fechada do LTC ($h(t)$ analítico) | AdamW / Lion | **CfC NPU** (Fast Interpolation Engine + Closed-Form Core) |
| **v7** | Quantized CfC | Ultra-low Power | Quantização INT8, Ternária (-1,0,1), Shift-Arithmetic | Quantization-Aware Training (QAT) | **INT8/Ternary CfC NPU** (Bitwise/Popcount Arithmetic Core) |
| **v8** | Streaming CfC | Sensores / Edge | Entrada assíncrona, estado oculto persistente | QAT + Fine-Tuning | **Streaming Edge NPU** (Zero-latency Sensor DMA + Ring Buffer) |

---

## Detalhamento Versão por Versão

---

### v0 — Neural Core

> **Foco:** Fundamentos matemáticos de álgebra linear e primitives de computação vetorial.

#### 1. Módulo de Software (C++)
- **Estruturas de Dados:** `Tensor<T>`, `Matrix<T>`, `Vector<T>` com alocação contígua em memória.
- **Operações Fundamentais:**
  - Multiplicação de Matriz ($\mathbf{C} = \mathbf{A} \cdot \mathbf{B}$);
  - Soma vetorial e produto Hadamard (elemento a elemento);
  - Transposição de matrizes.
- **Funções de Ativação:**
  - **Sigmoid:** $\sigma(x) = \frac{1}{1 + e^{-x}}$ e derivada $\sigma'(x) = \sigma(x)(1 - \sigma(x))$;
  - **ReLU:** $f(x) = \max(0, x)$ e derivada $f'(x) = \mathbb{I}(x > 0)$.
- **Função de Perda:** Erro Quadrático Médio (MSE): $\mathcal{L} = \frac{1}{N} \sum (y - \hat{y})^2$.
- **Otimizador:** Stochastic Gradient Descent (SGD) vanilla ($\theta \leftarrow \theta - \eta \cdot \nabla_\theta \mathcal{L}$).

#### 2. Requisitos de Aprendizado / Validação
- Regressão linear sintética ($y = wx + b$);
- Problema do XOR lógico ($2 \to 2 \to 1$).

#### 3. NPU Respectiva: **Matrix/Tensor Core**
- **Unidade de Processamento:** Elemento MAC (*Multiply-Accumulate*): $\text{Acc} \leftarrow \text{Acc} + (A \times B)$.
- **Arquitetura da NPU:**
  - Registradores de pesos, entrada e acumulador de 32-bit float (ou ponto fixo `Q16.16`);
  - Unidade de função especial (SFU) para lookup table (LUT) de Sigmoid/ReLU;
  - Barramento de memória simples para leitura sequencial de matrizes.

---

### v1 — MNIST MLP

> **Foco:** Redes totalmente conectadas (*Dense/Perceptron Multicamadas*) aplicadas à classificação de imagens simples.

#### 1. Módulo de Software (C++)
- **Camadas:** `DenseLayer(in_features, out_features)`.
- **Ativação Final:** **Softmax**: $S_i = \frac{e^{z_i}}{\sum e^{z_j}}$ com estabilização numérica ($\max(z)$ subtraído).
- **Função de Perda:** **Categorical Cross-Entropy (CCE)**: $\mathcal{L} = -\sum y_i \log(\hat{y}_i)$.
- **Inicialização de Pesos:** Xavier/Glorot (`Uniform` ou `Normal`).
- **Otimizador:** **SGD com Momentum**:
  $$v_t = \beta v_{t-1} + \eta \nabla_\theta \mathcal{L}, \quad \theta \leftarrow \theta - v_t$$
- **Dataset Pipeline:** Loader para o dataset **MNIST** (28x28 imagens em escala de cinza, 10 classes). Normalização $[0, 1]$ e mini-batching.

#### 2. Requisitos para Execução
- Acurácia alvo: $> 95\%$ no conjunto de teste MNIST.

#### 3. NPU Respectiva: **Dense NPU**
- **Arquitetura:**
  - **Array Sistólico 1D ou 2D** (ex.: $8 \times 8$ MACs);
  - Memória On-Chip (SRAM) dedicada para pesos da camada densa;
  - Unidade de Pipelining: produto escalar paralelo $y = \mathbf{W}\mathbf{x} + \mathbf{b}$;
  - Vetorizador de ativação em hardware (ReLU e LUT Softmax).

---

### v2 — MNIST CNN

> **Foco:** Extração de recursos espaciais via convolução 2D e amostragem.

#### 1. Módulo de Software (C++)
- **Novas Camadas:**
  - `Conv2D(in_channels, out_channels, kernel_size, stride, padding)`;
  - `MaxPool2D(kernel_size, stride)` e `AvgPool2D(kernel_size, stride)`;
  - `Flatten` (conversão de tensor 3D para vetor 1D).
- **Algoritmo de Convolução:**
  - Abordagem inicial: laços aninhados diretos;
  - Abordagem otimizada: `im2col` + GEMM (General Matrix Multiplication).
- **Otimizador:** **Adam (Adaptive Moment Estimation)**:
  $$m_t = \beta_1 m_{t-1} + (1-\beta_1)g_t, \quad v_t = \beta_2 v_{t-2} + (1-\beta_2)g_t^2$$
  $$\hat{m}_t = \frac{m_t}{1-\beta_1^t}, \quad \hat{v}_t = \frac{v_t}{1-\beta_2^t}, \quad \theta \leftarrow \theta - \frac{\eta}{\sqrt{\hat{v}_t} + \epsilon}\hat{m}_t$$

#### 2. Requisitos para Execução
- Acurácia alvo: $> 98\%$ no MNIST.

#### 3. NPU Respectiva: **CNN NPU**
- **Arquitetura:**
  - **Line-Buffer Engine:** Mantém $K$ linhas da imagem em registradores para deslizar a janela do kernel sem releitura de memória externa;
  - **Parallel Conv Block:** $K \times K$ multiplicadores operando em um único ciclo de clock;
  - Hardware de **Max-Pooling**: comparadores em cascata diretamente no fluxo de saída da convolução.

---

### v3 — CIFAR-10 CNN

> **Foco:** Processamento multicanal RGB, regularização e gerenciamento avançado de dados.

#### 1. Módulo de Software (C++)
- **Novas Camadas e Recursos:**
  - Convolução Multicanal com suporte a $C_{\text{in}} \to C_{\text{out}}$;
  - **Batch Normalization (BatchNorm2D)**: normalização por batch no treino, estatísticas móveis ($\mu, \sigma^2$) na inferência;
  - **Spatial Dropout** / **Dropout** regularizador;
  - Aumento de Dados simples (Random Crop, Horizontal Flip) em C++.
- **Otimizador:** **AdamW** (Adam com Weight Decay desacoplado) + **Cosine Annealing Learning Rate Scheduler**.
- **Dataset Pipeline:** Loader para **CIFAR-10** (Imagens $32 \times 32 \times 3$, 10 classes).

#### 2. Requisitos para Execução
- Acurácia alvo: $> 75\%$ no CIFAR-10.

#### 3. NPU Respectiva: **Multi-channel NPU**
- **Arquitetura:**
  - **Double Buffering Ping-Pong SRAM:** Enquanto o bloco $N$ calcula a convolução do canal $C$, o bloco $N+1$ carrega os pesos do canal $C+1$;
  - **Dataflow Interconnect:** Roteamento interno entre camadas sem gravação em DRAM;
  - Módulo BatchNorm integrado em hardware (escala e bias fundidos com o kernel na inferência: $w_{\text{fused}} = \frac{\gamma w}{\sigma}$, $b_{\text{fused}} = \frac{\gamma (b - \mu)}{\sigma} + \beta$).

---

### v4 — Neural ODE

> **Foco:** Modelagem de sistemas dinâmicos contínuos e equações diferenciais ordinárias parametrizadas por redes neurais.

#### 1. Módulo de Software (C++)
- **Conceito Matemático:**
  $$\frac{dh(t)}{dt} = f_{\theta}(h(t), t)$$
  O estado oculto $h(t)$ evolui continuamente no tempo através da função neural $f_\theta$.
- **Solvers de ODE em C++:**
  - **Euler Method** (1ª ordem): $h(t + \Delta t) = h(t) + \Delta t \cdot f_\theta(h(t), t)$;
  - **Runge-Kutta 4ª Ordem (RK4)**:
    $$k_1 = f_\theta(h, t)$$
    $$k_2 = f_\theta\left(h + \frac{\Delta t}{2}k_1, t + \frac{\Delta t}{2}\right)$$
    $$k_3 = f_\theta\left(h + \frac{\Delta t}{2}k_2, t + \frac{\Delta t}{2}\right)$$
    $$k_4 = f_\theta(h + \Delta t k_3, t + \Delta t)$$
    $$h(t + \Delta t) = h(t) + \frac{\Delta t}{6}(k_1 + 2k_2 + 2k_3 + k_4)$$
- **Backpropagation:** Adjoint State Method ou Backpropagation através dos passos do solver.
- **Aplicações:** Regressão em trajetórias dinâmicas, séries temporais espursas ou irregulares.

#### 2. Requisitos para Execução
- Reconstrução de trajetórias físicas (ex.: pêndulo simples, espiral 2D ou sistema de Lotka-Volterra).

#### 3. NPU Respectiva: **ODE NPU**
- **Arquitetura:**
  - **Integrator Pipeline Engine:** Módulo que executa os 4 estágios do RK4 reaproveitando o mesmo núcleo Denso/MLP;
  - **Adaptive Step Controller:** Ajustador de passo em hardware que reduz $\Delta t$ se a derivada crescer abruptamente.

---

### v5 — LTC (Liquid Time-Constant)

> **Foco:** Redes neurais inspiradas na biologia (como o sistema nervoso do *C. elegans*), com dinâmicas temporais não-lineares e constantes de tempo adaptativas.

#### 1. Módulo de Software (C++)
- **Formulação Matemática da Célula LTC:**
  $$\frac{dx_i(t)}{dt} = -\left[\frac{1}{\tau_i} + \sum_j f_j(x_j(t))\right] x_i(t) + \sum_j f_j(x_j(t)) E_{ij}$$
  onde $f_j(x_j) = \nu_{ij} \cdot \sigma(\gamma_{ij}(x_j + \mu_{ij}))$.
- **Características de Software:**
  - Constante de tempo não-linear dependente da entrada e do estado atual ("tempo líquido");
  - Integração via solver numérico híbrido (Euler/RK4 adaptativo especial para LTC);
  - Supre em amostragem assíncrona/irregular de dados.
- **Otimizador:** AdamW com clipping de gradiente rigoroso ($\|\nabla\| \le 1.0$).

#### 2. Requisitos para Execução
- Predição e controle de dinâmica em séries temporais com gaps de dados/ruído.

#### 3. NPU Respectiva: **Liquid NPU**
- **Arquitetura:**
  - **Non-linear Exponential Solver Core:** Módulo de hardware acelerador de exponenciais e sigmoides paralelas para calcular a condutância das sinapses solúveis;
  - **State Feedback Register Array:** Banco de registradores que atualiza o estado interno $x(t)$ recursivamente a cada sub-passo temporal.

---

### v6 — CfC (Closed-Form Continuous-Time)

> **Foco:** Substituição do solver ODE por uma solução analítica aproximada em forma fechada, multiplicando a velocidade de inferência por ordens de grandeza.

#### 1. Módulo de Software (C++)
- **Formulação Matemática da Célula CfC:**
  $$h(t) \approx \left(f(x, h_0; \theta) \odot e^{-\left[A(x, h_0; \theta) + b\right] t}\right) + g(x, h_0; \theta)$$
  onde $f, A, g$ são sub-redes neurais rasas (MLPs simples).
- **Vantagens em Software:**
  - Não requer iterações de solvers ODE (Euler/RK4) durante o tempo de execução!
  - Avaliação direta em qualquer tempo $t$ com complexidade $\mathcal{O}(1)$;
  - Estabilidade garantida por limites assintóticos da função exponencial.
- **Ativação:** SiLU/Swish ou Tanh com sigmoides nas portas de tempo.
- **Otimizador:** AdamW ou Lion.

#### 2. Requisitos para Execução
- Controle em tempo real (ex.: navegação autônoma simulada ou seguimento de trajetória) rodando com latência ultrabaixa.

#### 3. NPU Respectiva: **CfC NPU**
- **Arquitetura:**
  - **Fast Closed-Form Engine (CFE):**
    - Unidade de exponenciação rápida ($e^{-x}$) baseada em aproximação por séries de Taylor ou tabelas CORDIC/LUT;
    - Três sub-blocos de multiplicação matricial trabalhando em paralelo para calcular $f(x)$, $A(x)$ e $g(x)$;
    - Unidade MACC Element-wise para fundir os três resultados no vetor de estado final $h(t)$.

---

### v7 — Quantized CfC

> **Foco:** Compressão extrema do modelo CfC para operações com inteiros de 8 bits e representações ternárias/binárias.

#### 1. Módulo de Software (C++)
- **Esquema de Quantização:**
  - **INT8 Uniform Quantization:** $q = \text{round}\left(\frac{x}{S}\right) + Z$;
  - **Ternary Weights:** $w \in \{-1, 0, +1\}$;
  - **Quantization-Aware Training (QAT):** Straight-Through Estimator (STE) para passar gradientes pelas operações de arredondamento no treino C++.
- **Substituição de Ativações Transcendentais:**
  - Substituição de $e^{-x}$ por aproximações inteiras de ponto fixo (ex.: `Shift-Arithmetic` e aproximações racionais de Pade/LUT quantizada).

#### 2. Requisitos para Execução
- Modelo CfC mantendo $> 95\%$ do desempenho do modelo float32 original, reduzindo o tamanho dos pesos em até $4\times$ a $16\times$.

#### 3. NPU Respectiva: **Quantized CfC NPU**
- **Arquitetura:**
  - **Bitwise / Popcount Arithmetic Core:** Sub-núcleos de multiplicação substituídos por multiplexadores e unidades de `popcount` para pesos ternários;
  - **Fixed-Point Shift ALUs:** Operações de escala feitas inteiramente com *bit shifts* (sem multiplicadores de ponto flutuante);
  - **Zero-Value Skipping:** Hardware que detecta pesos $0$ e pula a operação, economizando $100\%$ do consumo energético no ciclo.

---

### v8 — Streaming CfC

> **Foco:** Sistema em tempo real ponta a ponta (Edge Processing), com processamento assíncrono de eventos e baixíssimo consumo energético.

#### 1. Módulo de Software (C++)
- **Recursos da Arquitetura:**
  - **Event-Driven Streaming:** A rede aceita vetores de entrada com marcas de tempo arbitrárias $\Delta t = t_{\text{atual}} - t_{\text{anterior}}$;
  - **State Memory Management:** O estado oculto $h$ é mantido de forma persistente e atualizado apenas quando novos eventos/leituras de sensores chegam;
  - **Pipeline C++ de Latência Mínima:** Ring buffers circularmente alocados, zero alocação dinâmica durante a inferência (`no-malloc`).

#### 2. Requisitos para Execução
- Processamento de dados de sensores (ex.: IMU, ECG, ou Event Cameras) em tempo real streaming.

#### 3. NPU Respectiva: **Streaming Edge NPU**
- **Arquitetura:**
  - **Direct Sensor DMA Interface:** A NPU lê entradas diretamente da memória de periféricos/sensores sem intervenção da CPU;
  - **Timestamp Counter & Elapsed Calculator:** Timer de hardware dedicado que injeta $\Delta t$ diretamente na entrada da unidade CfC;
  - **Persistent State SRAM Ring Buffer:** Memória ultra-rápida de baixo vazamento elétrico (*low-leakage SRAM*) para manter o estado $h(t)$ com a NPU em estado de dormência (*sleep mode*) entre eventos;
  - **Wake-on-Event Logic:** A NPU só ativa seus barramentos quando um novo dado chega ao buffer de entrada.

---

## Métricas de Validação de NPU

Para cada versão da NPU desenvolvida no projeto, os seguintes relatórios e simulações devem ser produzidos:

1. **Cycle-Count Parity:** Comparação do resultado numérico obtido pelo software C++ versus o resultado do emulador da NPU (diferença máxima permitida: $10^{-5}$ em float, $0$ erros em quantizado).
2. **Memory Bandwidth & Footprint:** Quantidade de bytes transferidos da RAM para a NPU por inferência.
3. **MAC Utilization Rate:** Porcentagem de ciclos em que os multiplicadores da NPU estão ocupados (alvo: $> 80\%$).
