[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/A02q9cx_)
# ViStar: Editor de Texto Minimalista com ncurses

O objetivo deste exercício é desenvolver um editor de texto em Linux, chamado **ViStar**, utilizando a linguagem C e a biblioteca ncurses, inspirado no clássico WordStar, um editor de texto popular nas décadas de 1970 e 1980, conhecido por sua interface simples e teclas de atalho baseadas em combinações com `Ctrl`. O **ViStar** será um editor minimalista que implementa funcionalidades essenciais de edição, navegação, gerenciamento de arquivos, busca, marcas e uma interface com janela de ajuda superior e barra de status inferior, proporcionando uma experiência funcional e intuitiva.

Este projeto visa reforçar conceitos de programação em C, como manipulação de estruturas de dados dinâmicas, entrada/saída de arquivos e criação de interfaces de terminal interativas. Os alunos devem implementar comandos básicos do WordStar, uma janela de ajuda ativada/desativada com `Ctrl+J` que exibe teclas de atalho e uma barra de status com informações em tempo real, utilizando a biblioteca ncurses para gerenciar a interface no terminal Linux.

## Requisitos do Editor
O **ViStar** deve implementar as seguintes funcionalidades mínimas:

1. **Modo de Edição**:
   - Inserir texto no buffer (memória que armazena o texto editado).
   - Suportar caracteres alfanuméricos, espaços, quebras de linha (`Enter`) e caracteres especiais básicos.
   - Exibir texto na tela com rolagem vertical se exceder a altura da janela.

2. **Navegação**:
   - Mover cursor por caractere (esquerda, direita, cima, baixo).
   - Mover por palavras (anterior/próxima).
   - Mover para início/fim da linha ou documento.

3. **Gerenciamento de Arquivos**:
   - Abrir arquivo de texto existente e carregar no buffer.
   - Criar novo arquivo, limpando o buffer.
   - Salvar buffer em arquivo (sobrescrevendo ou criando novo).
   - Sair do editor, com aviso se houver alterações não salvas.

4. **Comandos de Edição**:
   - Deletar caracteres (backspace, delete), palavras e linhas.
   - Inserir quebras de linha.
   - Copiar, cortar e colar blocos de texto.
   - Buscar palavra no texto (destacar primeira ocorrência).
   - Inserir e navegar para marcas (0-9).

5. **Interface**:
   - **Janela de Ajuda Superior**: Ativada/desativada com `Ctrl+J`, exibe teclas de atalho em colunas `MOVE`, `EDIT`, `BLOCK`, `FILE`.
   - **Barra de Status Inferior**: Mostra nome do arquivo (ou "[Sem Nome]"), posição do cursor (linha/coluna) e status "Modificado" se alterado.
   - Usar cores para destacar janela de ajuda, barra de status e, opcionalmente, texto.

## Teclas de Atalho
As teclas são inspiradas no WordStar, usando combinações com `Ctrl`:

| Categoria | Tecla | Função |
|-----------|-------|--------|
| **MOVE** | `Ctrl+S` | Mover cursor à esquerda |
| | `Ctrl+D` | Mover cursor à direita |
| | `Ctrl+E` | Mover cursor para cima |
| | `Ctrl+X` | Mover cursor para baixo |
| | `Ctrl+A` | Mover para palavra anterior |
| | `Ctrl+F` | Mover para próxima palavra |
| | `Ctrl+QS` | Mover para início da linha |
| | `Ctrl+QD` | Mover para fim da linha |
| | `Ctrl+QR` | Mover para início do documento |
| | `Ctrl+QC` | Mover para fim do documento |
| **EDIT** | `Ctrl+G` | Deletar caractere à direita (Delete) |
| | `Ctrl+H` | Deletar caractere à esquerda (Backspace) |
| | `Ctrl+T` | Deletar palavra à direita |
| | `Ctrl+Y` | Deletar linha atual |
| | `Ctrl+QF` | Buscar palavra no texto |
| | `Ctrl+K[0-9]` | Definir marcador (0-9) na posição atual |
| | `Ctrl+Q[0-9]` | Ir para marcador (0-9) |
| **BLOCK** | `Ctrl+KB` | Marcar início do bloco |
| | `Ctrl+KK` | Marcar fim do bloco |
| | `Ctrl+KC` | Copiar bloco marcado |
| | `Ctrl+KV` | Colar bloco na posição do cursor |
| | `Ctrl+KY` | Recortar bloco marcado |
| **FILE** | `Ctrl+KS` | Salvar arquivo |
| | `Ctrl+KO` | Abrir arquivo |
| | `Ctrl+KN` | Criar novo arquivo (limpar buffer) |
| | `Ctrl+X` | Sair do programa (avisar se não salvo) |
| **HELP** | `Ctrl+J` | Alternar janela de ajuda |

## Janela de Ajuda Superior
- **Funcionalidade**: Ativada/desativada com `Ctrl+J`, exibe atalhos em colunas `MOVE`, `EDIT`, `BLOCK`, `FILE`. O comando `^J toggle help` é alinhado na coluna `FILE`.
- **Layout**:

```
  MOVE                        EDIT                    BLOCK                 FILE
  ^S left  ^D right           ^G del char            ^KB begin             ^KS save
  ^E up    ^X down            ^H backspace           ^KK end               ^KO open
  ^A <word ^F >word           ^T del word            ^KC copy              ^KN new
  ^QS bol  ^QD eol            ^Y del line            ^KV paste             ^X  exit
  ^QR top  ^QC bottom         ^QF search             ^KY cut               ^J toggle help
  ^K0-9 set mark ^Q0-9 goto mark
```

- **Implementação**:
  - Usar `newwin(7, COLS, 0, 0)` para a janela de ajuda.
  - Imprimir texto formatado com `wprintw`.
  - Controlar visibilidade com uma variável booleana.
  - Alternar visibilidade com `Ctrl+J`, redesenhando a tela principal quando oculta.

## Barra de Status Inferior
- **Conteúdo**: Mostra nome do arquivo (ou "[Sem Nome]"), posição do cursor (ex.: "Linha: 1 Col: 1") e "Modificado" se o buffer foi alterado.
- **Implementação**: Usar `newwin(1, COLS, LINES-1, 0)` para a barra de status, atualizada após cada ação.

## Especificações Técnicas
- **Linguagem**: C.
- **Biblioteca**: ncurses (instalar via `sudo apt-get install libncurses5-dev libncursesw5-dev`).
- **Ambiente**: Linux (ex.: Ubuntu).
- **Estruturas de Dados**:
  - Estrutura dinâmica para texto (ex.: lista ligada de linhas ou array dinâmico de caracteres).
  - Buffer para texto e outro para copiar/cortar/colar.
  - Array para marcadores (0-9) com posições de linha/coluna.
- **Entrada/Saída de Arquivos**:
  - Suportar arquivos de texto simples (`.txt`).
  - Usar `fopen`, `fread`, `fwrite` para operações de arquivo.
- **Saída**:
  - Interface baseada em terminal com atualizações em tempo real via ncurses.
  - Usar `initscr`, `curs_set`, `raw`, `keypad` para inicialização.

## Dicas de Implementação
1. **Configuração do ncurses**:
   - Inicializar com `initscr`, habilitar `raw`, `keypad(stdscr, TRUE)`, desabilitar eco.
   - Configurar cores com `start_color`, `init_pair`.

2. **Estrutura do Programa**:
   - Loop principal em `main` captura teclas com `getch` e processa ações.
   - Usar estrutura de dados dinâmica para buffer de texto.
   - Modularizar com `libvistar.c` para operações de buffer/arquivo.

3. **Janela de Ajuda**:
   - Criar com `newwin`, exibir atalhos em colunas.
   - Alternar visibilidade e atualizar tela.

4. **Barra de Status**:
   - Criar com `newwin`, atualizar com nome do arquivo, posição do cursor e status de modificação.

5. **Busca e Marcadores**:
   - Para `^QF`, solicitar palavra, buscar no buffer, destacar primeira ocorrência.
   - Para marcadores, armazenar linha/coluna em array, atualizar cursor com `^Q[0-9]`.

6. **Código Exemplo**:
   Ver `vistar.c` para esqueleto básico.

## Entregar (veja controllami)
- **Repositório**: Repositório Git com:
  - `AUTHORS`: Lista de autores (alunos).
  - `LICENSE`: Licença MIT.
  - `makefile`: Compila `vistar.c` e `libvistar.c`.
  - `vistar.c`: Programa principal.
  - `vistar.h`: Cabeçalho do programa principal.
  - `libvistar.c`: Funções utilitárias.
  - `libvistar.h`: Cabeçalho das utilitárias.
  - `README.md`: Este documento.
- **Documentação**:
  - `README.md` explica compilação, execução, atalhos, estruturas de dados e janela de ajuda.
- **Teste**:
  - Demonstrar editor com arquivo de texto de exemplo.
  - Mostrar 6 comandos: abrir, novo, salvar, buscar, definir/ir para marcador, alternar ajuda.

## Referências
- ncurses: `man ncurses` ou [NCURSES HOWTO](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/).
- WordStar: [WordStar Reference](http://www.wordstar.org/).
- Compilação: `gcc -o vistar vistar.c libvistar.c -lncurses`.

**Desafio Extra**: Implementar `Ctrl+L` para repetir a última busca.

**Boa sorte construindo o ViStar!**

## Coordenação

* Prof. Dr. Ruben Carlo Benante <rcb@upe.br>
* POLI/UPE 2025/05/13
* Engenharia de Controle e Automação / Mecatrônica

