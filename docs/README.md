# tars-ml — Documentação (Índice)

`tars-ml` é uma **biblioteca TinyML de Co-Design Hardware/Software**: treine redes neurais
pequenas em Rust (Edition 2024, zero dependências de ML além do runtime padrão e rand), e faça o deploy delas em uma NPU
parametrizada em SystemVerilog ou em qualquer microcontrolador via runtime de
inferência portátil em ponto fixo.

> **TinyML** = Machine Learning embarcado: modelos pequenos rodando em hardware
> de baixíssimo consumo (MCUs, FPGAs, NPUs dedicadas), com latência determinística
> e processamento local — sem nuvem.

---

## 📚 Ordem de Leitura Recomendada

| # | Documento | O que contém |
| :---: | :--- | :--- |
| 1 | [STATUS.md](STATUS.md) | **Onde estamos agora** — estado atual do código, gap para a v0 e próximos passos imediatos |
| 2 | [ARCHITECTURE.md](ARCHITECTURE.md) | Visão geral, os 3 pilares, casos de uso TinyML |
| 3 | [ROADMAP_NPU.md](ROADMAP_NPU.md) | Progressão v0 → v8 com critérios de conclusão (DoD) por versão |
| 4 | [QUANTIZATION.md](QUANTIZATION.md) | Os 3 modos numéricos (Q8.24, INT8, Ternário), QAT, política de acumulador e spec da SFU |
| 5 | [MEM_FORMAT.md](MEM_FORMAT.md) | Especificação do formato `.mem` (contrato Rust ↔ SystemVerilog) |
| 6 | [DECISIONS.md](DECISIONS.md) | Registro de decisões de arquitetura (ADRs) — o "porquê" de cada escolha |
| 7 | [VISUALIZATION.md](VISUALIZATION.md) | Arquitetura de visualização e benchmarking desacoplada (`tars-viz`) |

> 💡 **Perdeu o fio da meada?** Comece sempre pelo [STATUS.md](STATUS.md) — ele
> aponta onde o projeto parou e qual é o próximo passo mínimo.

---

## 🧭 Princípios do Projeto

1. **Código 100% humano** — agentes de IA não escrevem código aqui (ver [AGENTS.md](../AGENTS.md)).
2. **Zero bibliotecas externas de ML** — toda álgebra, otimizadores e ativações escritos à mão em Rust (Edition 2024) e SystemVerilog.
3. **Treino na precisão alvo (QAT)** — o modelo aprende já em Q8.24 / INT8 / Ternário, não é convertido depois.
4. **Paridade zero-erro como lei** — o que o Rust calcula, a NPU reproduz bit a bit.

---

## 📖 Glossário

| Termo | Significado |
| :--- | :--- |
| **ADR** | Architecture Decision Record — registro de uma decisão de arquitetura |
| **CfC** | Closed-form Continuous-time Network — rede de tempo contínuo com solução analítica (inferência O(1)) |
| **DoD** | Definition of Done — critérios objetivos de conclusão de uma versão |
| **GEMM** | General Matrix Multiply — multiplicação de matrizes otimizada |
| **LTC** | Liquid Time-Constant Network — rede de tempo contínuo com constante de tempo adaptativa |
| **LOD** | Level of Detail — níveis hierárquicos de detalhe do visualizador (1: modelo → 5: bit individual) |
| **LUT** | Lookup Table — tabela de consulta em hardware |
| **MAC** | Multiply-Accumulate — operação fundamental: acc ← acc + (a × b) |
| **NPU** | Neural Processing Unit — processador dedicado a redes neurais |
| **PE** | Processing Element — célula de processamento de um array sistólico |
| **PTQ** | Post-Training Quantization — quantização aplicada após o treino (não usamos como abordagem principal) |
| **QAT** | Quantization-Aware Training — treino simulando a quantização no forward pass |
| **Q8.24** | Ponto fixo de 32 bits: 1 bit sinal + 7 bits inteiros + 24 bits fracionários |
| **SFU** | Special Function Unit — bloco de hardware para ativações (ReLU, Sigmoid) |
| **STE** | Straight-Through Estimator — técnica para retropropagar gradientes por operações de arredondamento |
| **STE (2)** | *No contexto de hardware:* estado estático mantido entre eventos |
