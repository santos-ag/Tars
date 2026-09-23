# tars-ml — Arquitetura de Visualização & Observabilidade Desacoplada

Este documento especifica a arquitetura do ecossistema de visualização e benchmarking do `tars-ml`.
Ele define a fronteira estrita entre a biblioteca matemática de machine learning (`tars-core`),
o subsistema de hardware (`tars-hw` / `npu/`), e as ferramentas de observabilidade (`tars-viz-*`),
garantindo que o runtime principal continue **100% limpo, minimalista, portável e compatível com `no_std`**.

---

## 1. Princípio Fundamental de Isolamento

A diretriz norteadora de toda a visualização no `tars-ml` é a **regra de dependência unidirecional**:

$$\boxed{\text{visualizer depende do core; core NUNCA depende do visualizer}}$$

```text
                    tars-core
                ┌────────────────┐
                │ Linear         │
                │ Conv2D         │
                │ CfC            │
                │ forward        │
                │ backward       │
                │ optimizer      │
                │ quantização    │
                └───────┬────────┘
                        │
                        │ apenas dados puros
                        ▼
                tars-viz-protocol
                ┌────────────────┐
                │ ModelGraph     │
                │ TrainingEvent  │
                │ Benchmark      │
                │ HardwareGraph  │
                └───────┬────────┘
                        │
          ┌─────────────┴──────────────┐
          ▼                            ▼
     tars-viz-web                outros consumidores
 Cytoscape / ELK / ECharts       CLI / arquivos / CI
```

### O teste definitivo de limpeza

> **Teste da Deleção Total**: se todo o subsistema de visualização (`tars-viz-*`, servidores, frontends,
> scripts de layout) for completamente deletado do repositório, o `tars-core` continuará
> compilando, treinando e executando inferência sem que uma única linha de código precise ser alterada.

### O que o `tars-core` NUNCA deve saber

O núcleo matemático não possui e jamais possuirá referências a:

```text
JSON          WebSocket      Leptos
HTML          HTTP           Actix / Axum
JavaScript    SVG            CSS
Cytoscape     ELK            ECharts
```

Uma camada `Linear` no `tars-core` contém estritamente sua formulação algébrica:

```rust
pub struct Linear {
    pub in_sz: usize,
    pub out_sz: usize,
    pub weights: Vec<f32>,
    pub bias: Vec<f32>,
}
```

A visualização não inspeciona ponteiros nem anexa hooks dentro do `forward`. Ela consome uma representação desacoplada exportada por adaptadores:

```text
Linear real (tars-core)
        │
        ▼ (extração funcional de metadados)
ModuleInfo (tars-viz-protocol)
        │
        ▼ (serialização externa)
Frontend / Visualizador
```

---

## 2. Topologia de Crates & Distribuição

Para manter o ecossistema organizado sem poluir instalações mínimas de microcontroladores ou FPGAs, o repositório é projetado como um workspace modular:

```text
tars/
├── crates/
│   ├── tars-core/              # ML puro, tensores, camadas, forward/backward, QAT (no_std friendly)
│   ├── tars-hw/                # Exportador .mem, drivers de co-simulação, integração SystemVerilog
│   ├── tars-viz-protocol/      # Structs agnósticas de visualização (GraphNode, BenchmarkRecord, etc.)
│   ├── tars-viz-server/        # Servidor leve opcional (HTTP/WebSocket/CLI)
│   └── tars-viz-web/           # Frontend SPA (Cytoscape, ELK, Apache ECharts)
├── npu/                        # RTL em SystemVerilog (main.sv, tb.sv, Makefile)
└── docs/                       # Documentação técnica e ADRs
```

### Experiência de Consumo e Empacotamento

- **Para o usuário de biblioteca embarcada (MCU / Desktop)**:
  ```toml
  # Cargo.toml
  [dependencies]
  tars = { version = "0.1", default-features = false } # Somente matemática, zero dependências web
  ```
- **Para o desenvolvedor que quer inspecionar seu modelo via browser**:
  ```bash
  cargo install tars-viz
  tars-viz --model model.mem --npu npu/main.sv
  ```
- **No fluxo local de desenvolvimento (Monorepo)**:
  ```bash
  cargo run -p tars-viz-server -- --port 8080
  ```

---

## 3. Protocolo Universal de Observabilidade (`tars-viz-protocol`)

O protocolo define um vocabulário comum de dados que não amarra o visualizador a nenhuma camada específica. Todas as entidades (sejam camadas de software ou blocos da NPU) tornam-se nós de um grafo universal.

### O Nó Universal (`GraphNode`)

```rust
pub type NodeId = usize;

#[derive(Clone, Debug, Serialize, Deserialize)]
pub enum NodeKind {
    // Software / Algorítmico
    Input,
    Linear { in_features: usize, out_features: usize },
    Activation { func: String },
    Conv2D { in_channels: usize, out_channels: usize, kernel: (usize, usize), stride: usize },
    Pooling { kind: String, size: (usize, usize) },
    CfCCell { hidden_size: usize },
    Flatten,
    
    // Hardware / NPU
    SystolicArray { rows: usize, cols: usize, mode: String },
    ProcessingElement { row: usize, col: usize, mode: String },
    SpecialFunctionUnit { supported_activations: Vec<String> },
    LocalSRAM { depth: usize, width: usize },
    DMAController,
    ClosedFormEngine,
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct Port {
    pub name: String,
    pub shape: Vec<usize>,
    pub data_type: String, // "f32", "q8.24", "int8", "ternary"
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct GraphNode {
    pub id: NodeId,
    pub label: String,
    pub kind: NodeKind,
    pub inputs: Vec<Port>,
    pub outputs: Vec<Port>,
    pub parent_id: Option<NodeId>, // Suporte nativo a subgrafos hierárquicos
    pub metadata: HashMap<String, String>,
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct GraphEdge {
    pub from_node: NodeId,
    pub from_port: usize,
    pub to_node: NodeId,
    pub to_port: usize,
    pub tensor_shape: Vec<usize>,
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct ModelGraph {
    pub nodes: Vec<GraphNode>,
    pub edges: Vec<GraphEdge>,
}
```

Com essa estrutura, quando o roadmap avançar para `Conv2D`, `MaxPool`, `ODE Solver` ou `CfC`, o frontend não precisará de nenhuma reescrita estrutural — novos módulos apenas instanciam variantes de `NodeKind`.

---

## 4. Catálogo de Visualizações

A suíte visual (`Cytoscape` para grafos + `ELK` para layout hierárquico + `Apache ECharts` para séries temporais e heatmaps) cobre seis superfícies fundamentais de inspeção:

### 4.1 Grafos de Redes Neurais

#### Rede Simples (MLP Sequencial)
```text
Input [2]
   │
   ▼
Linear 2×8
   │
   ▼
ReLU
   │
   ▼
Linear 8×2
   │
   ▼
Sigmoid [2]
```

#### Rede Residual (Skip Connections via Layout ELK)
```text
             ┌───────────────────────────────┐
             │                               │
Input ──► Linear ──► ReLU ──► Linear ────────┼──► Add ──► ReLU
  │                                          ▲
  └──────────────────────────────────────────┘
```

#### CNN (Roadmap v2 e v3)
```text
Input [3×32×32]
      │
      ▼
Conv2D (3 → 32, K=3×3)
      │
      ▼
ReLU ──► MaxPool (2×2) ──► Conv2D (32 → 64, K=3×3) ──► Flatten ──► Linear (1024 → 10)
```

#### Célula CfC (Closed-Form Continuous-Time — Roadmap v6)
O visualizador suporta expansão hierárquica (drill-down). O nó `CfC Cell` fechado pode ser expandido pelo usuário para inspecionar os três sub-blocos neurais e a solução exponencial explícita:

```text
               ┌── f(x, h) ──────────────┐
               │                         │
[x, h] ────────┼── A(x, h) ──► exp(-Δt) ─┼──► h'(t) (Closed-form state update)
               │                         │
               └── g(x, h) ──────────────┘
```

---

### 4.2 Arquitetura Hierárquica da NPU

O hardware em SystemVerilog pode ser visualizado em múltiplos níveis concêntricos:

```text
NPU Top
│
├── Memory (SRAM de Pesos & Ativações)
├── Compute (Array de Processamento)
├── SFU (Special Function Unit — ReLU / Sigmoid)
└── Control (FSM de Sequenciamento & Handshake start/done)
```

Ao expandir `Compute`:
```text
Compute
│
└── Systolic Array 8×8
    ├── PE[0,0]  PE[0,1] ... PE[0,7]
    ├── PE[1,0]  PE[1,1] ... PE[1,7]
    └── ...      ...         PE[7,7]
```

Ao expandir um único `PE` (Processing Element), o visualizador reflete a parametrização do hardware (`parameter MODE`):

- **Modo Q8.24 / INT8**:
  ```text
            Weight (SRAM)
                 │
                 ▼
  Input ──────► Multiplicador (32×32 ou 8×8) ──► Somador ──► Acumulador Parcial
  ```
- **Modo Ternário (`{-1, 0, +1}`)**:
  ```text
            Weight {-1, 0, +1}
                 │
                 ▼
  Input ──────► Multiplexador 3:1 ─────────────► Somador ──► Acumulador Parcial
  ```

---

### 4.3 Visualização de Co-Design Lado a Lado (Software ↔ Hardware)

O grande diferencial do `tars-ml` é inspecionar o modelo e a NPU simultaneamente. Clicar em uma camada do software destaca o bloco de hardware correspondente que a executará:

```text
┌─────────────────────────────┐        ┌─────────────────────────────┐
│      MODELO EM RUST         │        │     NPU EM SYSTEMVERILOG    │
├─────────────────────────────┤        ├─────────────────────────────┤
│ Linear 8×8 [DESTACADO]     │ ─────► │ Systolic Array [ATIVO]      │
│ ReLU                        │        │ SFU (ReLU Unit)             │
│ Linear 8×2                  │        │ Systolic Array              │
│ Sigmoid                     │        │ SFU (LUT Sigmoid)           │
└─────────────────────────────┘        └─────────────────────────────┘
```

---

### 4.4 Dashboard de Benchmarks Multidimensionais

Permite a comparação quantitativa entre os três modos numéricos (conforme previsto no Pilar 3 de [ARCHITECTURE.md](ARCHITECTURE.md)):

```text
MÉTRICA                    Q8.24            INT8 QAT         TERNÁRIO QAT
──────────────────────────────────────────────────────────────────────────
Acurácia (%)               98.1%            97.3%            94.8%
Tamanho dos Pesos          64 KB            16 KB             4 KB
Ciclos de Clock / Infer.   1.240            1.010              620
Recursos FPGA (DSPs)       64               32                  0 (Multiplication-free)
Recursos FPGA (LUTs)       1.200            1.450            2.100
Paridade Software/HW       0 Divergências   0 Divergências   0 Divergências
```

O dashboard permite alternar instantaneamente entre:
- Gráficos de barras agrupadas (trade-off de memória vs acurácia)
- Scatter plot de Pareto (eficiência de ciclos vs acurácia)
- Heatmap de ocupação de silício
- Tabelas detalhadas para auditoria e CI

---

### 4.5 Inspetor de Memória (`.mem`)

Como especificado em [MEM_FORMAT.md](MEM_FORMAT.md), o arquivo `.mem` é o contrato oficial entre Rust e SystemVerilog. O visualizador renderiza o mapa de memória e permite alternar a interpretação dos dados:

```text
MODEL.MEM (Topologia: 2 -> 2 -> 1 | Modo: Q8_24)

Endereço   Palavra (Hex)   Q8.24 Decodificado   Float Equivalente   Interpretação
────────────────────────────────────────────────────────────────────────────────
0x00       00000002        2.00000000           2.0                 Num Camadas (N)
0x01       00000002        2.00000000           2.0                 Camada 0 In
0x02       00000002        2.00000000           2.0                 Camada 0 Out
0x03       00000001        1.00000000           1.0                 Camada 1 Out
0x04       00800000        0.50000000           0.5                 W[0][0]
0x05       FF800000       -0.50000000          -0.5                 W[0][1]
0x06       00C00000        0.75000000           0.75                W[1][0]
0x07       FF400000       -0.75000000          -0.75                W[1][1]
...
```

Modos de visualização comutáveis por botão: **Float | Q8.24 | INT8 | Ternário | Hexadecimal | Binário Bruto**.

---

### 4.6 Inspeção Passo a Passo de Forward e Backward

Permite selecionar um vetor de entrada (ex: `[0.70, 0.20]`) e inspecionar a propagação dos tensores:

```text
Input: [0.70, 0.20]
  │
  ▼
Linear: z = [0.38, -0.91, 1.23, 0.14]
  │
  ▼
ReLU:   a = [0.38,  0.00, 1.23, 0.14]
  │
  ▼
Linear: z = [1.18, -0.42]
  │
  ▼
Sigmoid: ŷ = [0.765, 0.397]
```

No modo de inspeção de retropropagação (Backward pass), a interface exibe em paralelo:
1. Ativação intermediária ($a$)
2. Gradiente da ativação ($\frac{\partial L}{\partial a}$)
3. Peso atual ($W$)
4. Gradiente do peso ($\frac{\partial L}{\partial W}$)
5. Novo peso atualizado ($W - \eta \nabla W$)

---

## 5. Estratégia de Escala: Níveis de Detalhe (Level of Detail - LOD)

Para redes diminutas (ex: `2 → 4 → 2`), exibir cada neurônio e conexão é viável. No entanto, para modelos com milhares ou milhões de parâmetros (ex: `784 → 512 → 256 → 10`), renderizar arestas individuais congela qualquer navegador.

O visualizador adota um sistema estrito de **5 Níveis de Detalhe (LOD 1 a 5)**:

```text
LOD 1: Modelo Global (Visão de Blocos)
  │
  ▼ (Duplo clique em uma camada)
LOD 2: Módulo / Camada (Topologia, contagem de parâmetros, tipo de quantização)
  │
  ▼ (Clique em 'Inspecionar Pesos')
LOD 3: Tensor / Matriz (Heatmap interativo de pesos e ativações)
  │
  ▼ (Seleção de região ou bloco)
LOD 4: Bloco / Sub-matriz (ex: bloco 16×16 ou PE individual da NPU)
  │
  ▼ (Clique em uma célula específica)
LOD 5: Parâmetro / Bit Individual (Float, Q8.24, Hexadecimal, Gradiente)
```

### Exemplo de Inspeção LOD 5

Ao clicar em uma célula na linha 427, coluna 81 de uma matriz `1024×1024`:
```text
┌────────────────────────────────────────────────────────┐
│ INSPETOR DE PESO: W[427][81]                           │
├────────────────────────────────────────────────────────┤
│ Float original:      -0.31722000                       │
│ Ponto Fixo Q8.24:    -0.31721997                       │
│ Hexadecimal bruto:   0xFFAEC4E2                        │
│ Binário (32 bits):   1111 1111 1010 1110 1100 0100 ... │
│ Gradiente acumulado: +0.00472011                       │
│ Modo de Quantização: Q8.24 (Signed 32-bit)             │
│ PE Alocado na NPU:   PE[3, 5] (Ciclo de Fetch #84)     │
└────────────────────────────────────────────────────────┘
```

Essa abordagem permite navegar **do grafo macroscópico até o bit físico** de forma instantânea e sem degradação de performance gráfica.

---

## 6. Anti-Patterns de Arquitetura (O Que NUNCA Fazer)

Para assegurar a integridade do código do `tars-ml`, os seguintes padrões são terminantemente proibidos:

### ❌ Anti-Pattern 1: Misturar observabilidade no loop de execução

```rust
// PROIBIDO! Jamais faça isso dentro do core
fn forward(&self, input: &[f32]) -> Vec<f32> {
    let out = self.compute(input);
    websocket.send(format!("activation: {:?}", out)); // VIOLAÇÃO GRAVE
    out
}
```
*Motivo*: destrói o determinismo temporal, impede a compilação em `no_std`, introduz overhead inaceitável e quebra a portabilidade.

### ❌ Anti-Pattern 2: Estrutura de arquivos misturada

```text
src/
├── model.rs
├── linear.rs
├── optimizer.rs
├── cytoscape.rs    <-- PROIBIDO
├── echarts.rs      <-- PROIBIDO
├── websocket.rs    <-- PROIBIDO
├── html.rs         <-- PROIBIDO
└── server.rs       <-- PROIBIDO
```
*Motivo*: acopla tecnologias web ao runtime de ML. Todos esses arquivos pertencem a `crates/tars-viz-*`.

### ❌ Anti-Pattern 3: Dependências web no `Cargo.toml` do core

```toml
# PROIBIDO no Cargo.toml do core
[dependencies]
tokio = "1.0"
axum = "0.7"
serde_json = "1.0"
```
*Motivo*: a engine matemática de treino e inferência depende exclusivamente da biblioteca padrão do Rust (e futuramente apenas de `core` para `no_std`) e da crate `rand`.

---

## 7. Resumo da Fronteira

| Característica | `tars-core` | `tars-viz-protocol` | `tars-viz-web` / `tars-viz-server` |
| :--- | :--- | :--- | :--- |
| **Linguagem** | Rust (Edition 2024) | Rust (Edition 2024) | Rust / TypeScript / HTML / CSS |
| **Alvo de Execução** | CPU / MCU / `no_std` | Qualquer | Navegador / Desktop |
| **Dependências Externas** | Apenas `rand` | `serde` | Cytoscape, ELK, ECharts, Axum/Tokio |
| **Conhece visualização?**| **Não (Zero)** | Conhece apenas tipos abstratos | Sim (Tudo) |
| **Pode ser removido?** | Não (é o coração do projeto)| Sim | Sim |
