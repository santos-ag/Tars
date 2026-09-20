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

### 1. Módulo C++ (`src/`)
- `Types.hpp`: aliases de tipos padrão (`f32`, `i32`, `v<T>`, `m<T>`).
- `Math.hpp`/`Math.cpp`: funções de utilidade contendo `sigmoidf(x)`, `ReLU(x)` e gerador aleatório.
- `Layer.hpp`: struct de camada densa usando `std::vector<std::vector<float>>` inicializada com pesos em `[-1, 1]`.
- `NeuralNetwork.hpp`/`NeuralNetwork.cpp`: classe de rede sequencial com método `forward` usando Sigmoid rígida em todas as camadas. Topologia configurável.
- `Train_conf.hpp`: configurações globais (`epochs=10000`, `lr=10`, `h=0.0001`) e dataset estático para a paridade 3D (8 amostras).
- `Train.hpp`/`Train.cpp`: classe de treino usando **diferenças finitas centrais** `(cost(w+h) - cost(w-h)) / 2h` para estimar gradientes numericamente.
- `main.cpp`: executável interativo que exibe a evolução do Erro Quadrático Médio (MSE) e o tempo de treino em milissegundos.

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
| **Álgebra Linear** | `std::vector` aninhado | Criar classes `Matrix<T>` e `Vector<T>` com alocação contígua em memória |
| **Backpropagation** | Diferenças finitas | Implementar derivadas analíticas e backpropagation analítico exato |
| **Treino QAT** | Float32 puro | Adicionar suporte a treino simulação Q8.24, INT8 QAT e Ternário QAT |
| **Exportador** | Não existe | Criar `Exporter.hpp` gerando os arquivos `.mem` no formato especificado em [MEM_FORMAT.md](MEM_FORMAT.md) |
| **NPU Hardware** | Módulos fixos | Unificar em `npu_core #(parameter MODE)` com acumulador estendido de 48b |
| **SFU na NPU** | Não existe | Adicionar bloco combinacional de ativação (ReLU / Sigmoid) na saída do acumulador |
| **Paridade** | Testes manuais | Testbench ler `model.mem`, `input.mem` e `expected.mem` validando 0 erros de bit |
| **Experimentos v0** | Apenas Paridade 3D | Adicionar validação do XOR 2->2->1 e Regressão Linear Sintética |

---

## 🎯 Checklist do Próximo Passo Imediato ("Fio da Meada")

Siga esta sequência exata para avançar no projeto sem se perder:

### Etapa 1: Documentação e Infraestrutura
- [x] Reestruturar a documentação com a matriz tripla de precisão e os 3 pilares.
- [ ] **Ação Humana**: Fazer o merge/commit da branch `docs/roadmap-update`.
- [ ] **Ação Humana**: Corrigir os arquivos de ambiente (`npu/README.md`, `shell.nix` e `README.txt`) — ver Seção "Pendências de Ambiente".

### Etapa 2: Refatoração da Matemática C++ (Primeiro Código Humano)
- [ ] Criar `src/Matrix.hpp` com buffer contíguo unidimensional.
- [ ] Adicionar derivadas analíticas de Sigmoid e ReLU em `src/Math.cpp`.
- [ ] Refatorar `Train.cpp` para usar Backpropagation analítico exato.

### Etapa 3: Exportação e Paridade NPU (Fechamento da v0 e v0.5)
- [ ] Criar `src/Exporter.hpp` para exportar a rede treinada no formato `model.mem`.
- [ ] Adicionar o módulo SFU com ReLU em `npu/main.sv`.
- [ ] Carregar `model.mem` no `tb.sv` e validar paridade com $0$ erros no XOR.

---

## ⚠️ Pendências de Ambiente (Para Correção Manual Humana)

Conforme a política do repositório ([AGENTS.md](../AGENTS.md)), agentes não podem alterar arquivos de build ou código. As seguintes pendências devem ser corrigidas manualmente pelos mantenedores:

1. **`npu/README.md`**:
   - Remover a instrução `cd simple` (a pasta `simple/` não existe).
   - Documentar os alvos `make sim2`, `make test` e `make wave2` (que testam a NPU ternária).
2. **`shell.nix`**:
   - Atualmente possui apenas `cmake`, `gnumake` e `clang`.
   - Adicionar os pacotes de hardware: `iverilog`, `verilator` e `gtkwave`.
3. **`README.txt` (Raiz)**:
   - Adicionar o link para a pasta `docs/` e uma breve descrição do propósito do `tars-ml` como biblioteca TinyML de Co-Design.
