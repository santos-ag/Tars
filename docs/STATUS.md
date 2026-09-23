# tars-ml — Estado Atual do Projeto & Próximos Passos (STATUS)

Este documento funciona como a **bússola do projeto**: ele detalha exatamente o que
já está implementado no código, a análise de lacunas para fechar a versão atual (**v0**)
e o checklist do próximo passo imediato para a equipe não "perder o fio da meada".

---

## 📍 Onde Estamos Agora?

> **Fase Atual**: Transição para a conclusão formal da **v0 (Neural Core)**.
>
> A documentação e o planejamento foram completamente reestruturados no formato de Co-Design
> TinyML com Matriz Tripla de Precisão (Q8.24, INT8 e Ternário). O código-fonte existente
> contém protótipos funcionais que precisam ser adaptados para cumprir os critérios da v0.

---

## 🔎 Inventário do Código Existente no Repositório

### 1. Módulo Rust (`src/` e `Cargo.toml`)
- `Cargo.toml`: pacote `tars` v0.1.0, `edition = "2024"`, dependência `rand = "0.10.3"`.
- `src/lib.rs`: exporta os módulos (`data`, `layer`, `optimizer`, `grad`, `model`), função de ativação `sigmoidf(x: f32) -> f32`, inferência `forward<const IN: usize>(model: &Model<IN>, input: [f32; IN]) -> f32` e custo MSE `cost<const IN: usize, const OUT: usize>(model: &Model<IN>, data: &[Data<IN, OUT>]) -> f32`.
- `src/layer.rs`: struct `Layer<const IN: usize, const OUT: usize>` com const generics, pesos `weights: [[f32; IN]; OUT]` e bias `bias: [f32; OUT]`.
- `src/model.rs`: struct `Model<const IN: usize>` encapsulando `layer1: Layer<IN, 1>`.
- `src/optimizer.rs`: struct `BGD` (Batch Gradient Descent) com taxa de aprendizado `lr: f32` e método `step<const IN: usize, const OUT: usize>(&self, model: &mut Model<IN>, grad: &Grad<IN, OUT>)`.
- `src/grad.rs`: struct `Grad<const IN: usize, const OUT: usize>` e função `num_grad` calculando gradientes numericamente por **diferenças finitas centrais** `(cost(w+h) - cost(w-h)) / (2h)` com `h = 1e-3`.
- `src/data.rs`: struct `Data<const IN: usize, const OUT: usize>` armazenando pares de amostra `input: [f32; IN]` e `target: [f32; OUT]`.
- `src/bin/main.rs`: binário executável interativo treinando o modelo na tabela lógica OR (`DATA_TR`) com `EPOCHS = 100000`, `LR = 10.0`, medindo e exibindo o custo MSE e a evolução percentual.

### 2. Módulo SystemVerilog (`npu/`)
- `main.sv`: módulo `npu` de produto escalar 4D inteira de 32 bits com acumulador, registrador de bias e FSM simples (`start`/`done`).
- `main2.sv`: módulo `npu_ternary` variante com multiplexador para pesos ternários (códigos `01`=+1, `11`=-1, `00`=0).
- `tb.sv` / `tb2.sv`: testbenches de simulação com testes funcionais passando (resultados 80 e -36 no binário; 25 e -15 no ternário).
- `Makefile`: alvos para `sim`, `sim2`, `test`, `wave`, `wave2`, `lint` e `clean`.

---

## 📉 Análise de Lacunas (Gap Analysis) para Concluir a v0

Para atender integralmente à definição de pronto (DoD) da versão **v0** especificada em [ROADMAP_NPU.md](ROADMAP_NPU.md):

| Componente | Estado Atual | O que precisa ser feito |
| :--- | :--- | :--- |
| **Álgebra Linear / Tensores** | Structs com const generics `[f32; N]`, monolayer | Generalizar para suporte multi-camadas e operações matriciais/tensores contíguos em memória |
| **Backpropagation** | Diferenças finitas (`num_grad`) | Implementar derivadas analíticas de Sigmoid/ReLU e backpropagation analítico exato |
| **Treino QAT** | Float32 puro | Adicionar suporte a treino simulando Q8.24 (`i32`), INT8 QAT e Ternário QAT com STE |
| **Exportador** | Não existe | Criar módulo `src/exporter.rs` gerando os arquivos `.mem` no formato especificado em [MEM_FORMAT.md](MEM_FORMAT.md) |
| **NPU Hardware** | Módulos fixos | Unificar em `npu_core #(parameter MODE)` com acumulador estendido de 48b |
| **SFU na NPU** | Não existe | Adicionar bloco combinacional de ativação (ReLU / Sigmoid) na saída do acumulador |
| **Paridade** | Testes manuais | Testbench ler `model.mem`, `input.mem` e `expected.mem` validando 0 erros de bit em relação ao Rust |
| **Experimentos v0** | OR gate (2->1) | Adicionar validação do XOR (2->2->1) multicamada e Regressão Linear Sintética |

---

## 🎯 Checklist do Próximo Passo Imediato ("Fio da Meada")

Siga esta sequência exata para avançar no projeto sem se perder:

### Etapa 1: Documentação e Infraestrutura
- [x] Reestruturar a documentação com a matriz tripla de precisão e os 3 pilares.
- [x] Atualizar a documentação completa para Rust (Edition 2024).
- [ ] **Ação Humana**: Corrigir os arquivos de ambiente (`shell.nix` e `README.txt`) — ver Seção "Pendências de Ambiente".

### Etapa 2: Refatoração da Matemática e Backprop em Rust (Primeiro Código Humano)
- [ ] Implementar suporte a redes multicamadas (ex.: XOR 2->2->1) em `src/model.rs`.
- [ ] Adicionar derivadas analíticas de Sigmoid e ReLU em `src/lib.rs` / módulo de ativações.
- [ ] Refatorar o treino para usar Backpropagation analítico exato (substituindo `num_grad`).

### Etapa 3: Exportador e Paridade NPU (Fechamento da v0 e v0.5)
- [ ] Criar `src/exporter.rs` para exportar a rede treinada no formato `model.mem`.
- [ ] Adicionar o módulo SFU com ReLU em `npu/main.sv`.
- [ ] Carregar `model.mem` no `tb.sv` e validar paridade com 0 erros no XOR.

---

## ⚠️ Pendências de Ambiente (Para Correção Manual Humana)

Conforme a política do repositório ([AGENTS.md](../AGENTS.md)), agentes não podem alterar arquivos de build ou código. As seguintes pendências devem ser corrigidas manualmente pelos mantenedores:

1. **Toolchain Rust (`Cargo.toml`)**:
   - O projeto agora é compilado e executado nativamente via Cargo: `cargo run`, `cargo test`, `cargo build`.
   - Os arquivos legados de C++ (`CMakeLists.txt` e `run.sh`) na raiz do repositório estão obsoletos e devem ser limpos ou adaptados pelo mantenedor.
2. **`shell.nix`**:
   - Atualmente configurado para o ambiente C++ antigo (`cmake`, `gnumake`, `clang`).
   - Atualizar para incluir as ferramentas de Rust (`rustc`, `cargo`, `rust-analyzer` ou `rustPlatform.rustc`) e as ferramentas de hardware (`iverilog`, `verilator`, `gtkwave`).
3. **`npu/README.md`**:
   - Remover a instrução `cd simple` (a pasta `simple/` não existe).
   - Documentar os alvos `make sim2`, `make test` e `make wave2` (que testam a NPU ternária).
4. **`README.txt` (Raiz)**:
   - Atualizar as instruções de build para o fluxo do Cargo (`cargo run`), linkando para a pasta `docs/`.
