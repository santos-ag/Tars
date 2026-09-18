# AGENTS.md — Política de Agentes e IA neste Repositório

## Regra inviolável

**Qualquer e todo agente (ou modelo de IA) neste repositório NÃO PODE encostar no código.**

Isso significa que nenhum agente pode:

- Criar, editar, renomear ou deletar **qualquer arquivo de código-fonte**
  (`.cpp`, `.h`, `.hpp`, `.c`, `.cc`), incluindo `main.cpp`;
- Modificar arquivos de build ou configuração do projeto
  (`CMakeLists.txt`, `run.sh`, `shell.nix`, `.clang-format`, `.gitignore`);
- Aplicar refatorações, "correções", formatação automática ou
  reescritas de código, **mesmo que detecte bugs, warnings ou más práticas**;
- Gerar patches, commits ou pull requests que alterem código.

## O que um agente PODE fazer

- **Ler** todo o repositório livremente (código, build, histórico, issues);
- **Escrever e editar apenas documentação** (`.md`, `.txt` em `docs/`) —
  e somente quando o mantenedor pedir explicitamente;
- Trabalhar **sempre em uma branch separada** (ex.: `docs/*`), para que o
  mantenedor revise e aprove antes de qualquer merge;
- Reportar problemas encontrados no código **por escrito**
  (issue, relatório, comentário em revisão), sem corrigi-los por conta própria.

## Propriedade do código

Todo o código de `tars-ml` é escrito **exclusivamente por humanos**.
Agentes de IA são ferramentas de consulta, pesquisa e documentação —
nunca autores ou editores do código.

Se um agente precisa apontar um problema, o fluxo é:

1. Documentar o problema (o quê, onde, por quê);
2. Aguardar o humano corrigir manualmente;
3. Nunca "adiantar" a correção.

---

*Esta política é deliberada: `tars-ml` é uma biblioteca de machine learning
construída do zero com propósito educacional/de pesquisa. O valor do projeto
está no aprendizado humano profundo — o código deve refletir decisão humana
em cada linha.*
