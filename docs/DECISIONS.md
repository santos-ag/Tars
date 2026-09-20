# tars-ml — Registro de Decisões de Arquitetura (ADRs)

Este documento registra as principais decisões de arquitetura e design tomadas pela equipe,
seus contextos, razões e consequências. Ele garante que o projeto preserva o "fio da meada"
mesmo com o passar do tempo.

---

## ADR-001: Adotar Ponto Fixo Q8.24 como Formato Numérico Âncora

### Contexto
Para evitar a complexidade, área de silício e consumo de energia de multiplicadores de ponto
flutuante (IEEE 754 float32) na NPU em SystemVerilog, precisávamos escolher uma representação
em Ponto Fixo de 32 bits.

### Alternativas Consideradas
- **Q16.16**: 16 bits inteiros, 16 bits fracionários. Concede faixa de -32768 a +32767, mas perde
  precisão fracionária (resolução de 0.000015). Desperdício de bits inteiros para redes neurais.
- **Q4.28**: 4 bits inteiros, 28 bits fracionários. Altíssima resolução (10^-9), mas faixa
  muito estreita (-8.0 a +7.99), com risco severo de overflow no acúmulo de produtos escalares.
- **Q8.24**: 8 bits inteiros (-128.0 a +127.99), 24 bits fracionários (resolução de 5.9 x 10^-8).

### Decisão
Adotar **Q8.24** como formato âncora obrigatório de todas as versões do projeto.

### Consequências
- A resolução de 24 bits fracionários é **praticamente idêntica à mantissa do float32** do C++
  (23 bits), garantindo que a perda de precisão em relação ao ponto flutuante é imperceptível.
- A faixa de -128 a +127 dá margem segura para pesos, biases e ativações.
- Para evitar overflow no acúmulo de somatórios longos, a NPU usará um acumulador estendido de 48 bits.

---

## ADR-002: Manter C++ e SystemVerilog no Mesmo Repositório (Monorepo)

### Contexto
Houve dúvida sobre se a NPU em SystemVerilog deveria ser movida para um repositório separado
do engine C++.

### Decisão
**Manter tudo em um único repositório (Monorepo).**

### Razões
1. **Atomicidade no Co-Design**: qualquer alteração no formato de exportação de pesos no C++
   quebraria a NPU no mesmo instante se os projetos fossem separados.
2. **Co-Simulação Automatizada**: o testbench `tb.sv` roda no mesmo pipeline do C++, executando
   o treino, exportando o `.mem` e validando a paridade de bits em um único comando.
3. **Identidade do Projeto**: o `tars-ml` é uma solução completa de Co-Design Hardware/Software;
   separá-lo reduziria o C++ a "mais uma biblioteca" e o Verilog a "mais um multiplicador".

---

## ADR-003: Treino Consciente da Precisão (QAT) em Todos os Níveis

### Contexto
Quantização aplicada após o treino (Post-Training Quantization - PTQ) degrada a acurácia,
especialmente em modelos ternários.

### Decisão
Implementar **Quantization-Aware Training (QAT)** desde o engine de treino em C++, aplicando
a discretização no forward pass com Straight-Through Estimator (STE) no backward pass.

### Razões
- Permite que o C++ treine modelos em Q8.24, INT8 e Ternário que já nascem adaptados às limitações
  do hardware, preservando a acurácia final.

---

## ADR-004: NPU Parametrizada via `parameter MODE` em SystemVerilog

### Contexto
Para evitar duplicação de código (*boilerplate*) com arquivos separados como `npu_q8.sv`, `npu_int8.sv`
e `npu_ternary.sv`, precisávamos de uma estrutura limpa no hardware.

### Decisão
Usar o recurso nativo `parameter string MODE = "Q8_24"` e blocos `generate` em SystemVerilog
para adaptar a lógica aritmética (multiplicadores de 32b, 8b ou Muxes 3:1) em tempo de síntese.

---

## ADR-005: Inserir o Marco Intermediário v0.5 (Fechamento do Loop de Co-Design)

### Contexto
O salto da v0 (produto escalar de 4 elementos) para a v1 (Array Sistólico 8x8 + MNIST) era grande
demais e trazia risco de travamento do desenvolvimento.

### Decisão
Criar a **v0.5**: execução da rede XOR (2->2->1) **completa** na NPU, camada por camada,
validando o arquivo `.mem` e a paridade zero-erro antes de implementar estruturas complexas.

---

## ADR-006: Re-escopo da v7 para Sparsidade e Eficiência Energética

### Contexto
Com a migração da quantização/ternarização para todas as versões do roadmap, a v7 (anteriormente
chamada "Quantized CfC") perdeu seu propósito original.

### Decisão
Re-escopar a v7 para **"v7 — Sparsidade & Eficiência Energética (CfC Otimizada)"**, focando em:
- Indução de pesos nulos (Sparsity QAT) no C++.
- Lógica de **Zero-Value Skipping** (pular ciclos com peso 0) e empacotamento denso de bits na NPU.

---

## ADR-007: Contrato de Memória `.mem` em Texto Hexadecimal Padrão

### Contexto
O C++ precisava de um formato simples e universal para enviar os pesos treinados para o
simulador SystemVerilog.

### Decisão
Usar arquivos em formato de texto hexadecimal legíveis nativamente pelo `$readmemh` do Verilog,
com um cabeçalho de palavras contendo o número de camadas e a topologia (ver [MEM_FORMAT.md](MEM_FORMAT.md)).
