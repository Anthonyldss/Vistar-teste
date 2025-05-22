#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "vistar.h"
#include "libvistar.h"

// Buffer de texto principal e estado do cursor (visíveis apenas neste arquivo)
static TextBuffer *buffer;
static Cursor cursor;

// Redesenha todo o conteúdo da tela com base no buffer e na posição do cursor
void draw_text(void) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int lines_on_screen = max_y - 1; // reserva última linha para barra de status

    clear();

    // Exibe apenas as linhas visíveis a partir de top_line
    for (int i = 0; i < lines_on_screen; i++) {
        int line_idx = cursor.top_line + i;
        if (line_idx >= buffer->nlines) break;
        mvprintw(i, 0, "%s", buffer->lines[line_idx]);
    }

    // Barra de status com nome do arquivo, posição do cursor e status de modificação
    move(max_y - 1, 0);
    clrtoeol();
    char status[256];
    snprintf(status, sizeof(status), "%s - Linha %d, Col %d %s",
             buffer->filename[0] ? buffer->filename : "[Sem Nome]",
             cursor.y + 1, cursor.x + 1,
             buffer->modified ? "(Modificado)" : "");
    mvprintw(max_y - 1, 0, status);

    // Posiciona cursor na tela (coordenadas relativas ao topo visível)
    move(cursor.y - cursor.top_line, cursor.x);
    refresh();
}

// Inicializa o modo ncurses e o estado inicial do editor
void vistar_init(void) {
    initscr();              // inicia ncurses
    raw();                  // entrada bruta, permite capturar Ctrl+X, etc.
    keypad(stdscr, TRUE);   // habilita teclas especiais como setas
    noecho();               // evita ecoar teclas digitadas
    curs_set(1);            // mostra o cursor
    start_color();          // se desejar adicionar cores futuramente

    buffer = buffer_create(); // cria novo buffer de texto vazio
    cursor.x = 0;
    cursor.y = 0;
    cursor.top_line = 0;
}

// Libera recursos usados pelo editor
void vistar_cleanup(void) {
    buffer_free(buffer); // libera memória usada pelo buffer
    endwin();            // encerra ncurses
}

// Loop principal do editor: entrada de teclado, manipulação de buffer e redesenho
void vistar_loop(void) {
    int ch;
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int lines_on_screen = max_y - 1;

    while (1) {
        draw_text(); // redesenha tela a cada iteração

        ch = getch(); // espera entrada do usuário

        if (ch == 24) { // Ctrl+X para sair
            if (buffer->modified) {
                // Previne perda de dados acidental
                mvprintw(max_y - 1, 0, "Arquivo modificado. Pressione Ctrl+X novamente para sair sem salvar.");
                int ch2 = getch();
                if (ch2 == 24) break;
            } else {
                break;
            }
        }
        // Movimentação com setas
        else if (ch == KEY_LEFT) {
            if (cursor.x > 0) cursor.x--;
            else if (cursor.y > 0) {
                cursor.y--;
                cursor.x = strlen(buffer->lines[cursor.y]);
            }
        }
        else if (ch == KEY_RIGHT) {
            int line_len = strlen(buffer->lines[cursor.y]);
            if (cursor.x < line_len) cursor.x++;
            else if (cursor.y < buffer->nlines - 1) {
                cursor.y++;
                cursor.x = 0;
            }
        }
        else if (ch == KEY_UP) {
            if (cursor.y > 0) cursor.y--;
            // Ajusta posição horizontal se for maior que o comprimento da nova linha
            int line_len = strlen(buffer->lines[cursor.y]);
            if (cursor.x > line_len) cursor.x = line_len;
        }
        else if (ch == KEY_DOWN) {
            if (cursor.y < buffer->nlines - 1) cursor.y++;
            int line_len = strlen(buffer->lines[cursor.y]);
            if (cursor.x > line_len) cursor.x = line_len;
        }
        else if (ch == KEY_BACKSPACE || ch == 127) {
            // Deleta caractere à esquerda do cursor
            if (buffer_delete_char(buffer, &cursor)) {
                // Reposiciona scroll se necessário
                if (cursor.y < cursor.top_line) cursor.top_line = cursor.y;
            }
        }
        else if (ch == '\n' || ch == '\r') {
            // Divide a linha atual ao pressionar Enter
            char *line = buffer->lines[cursor.y];
            int len = strlen(line);

            char *new_line = strdup(line + cursor.x); // parte direita
            line[cursor.x] = '\0';                    // trunca linha atual

            // Verifica se há espaço para nova linha
            if (buffer->nlines + 1 >= buffer->max_lines) continue;

            // Desloca linhas abaixo para abrir espaço
            for (int i = buffer->nlines; i > cursor.y + 1; i--) {
                buffer->lines[i] = buffer->lines[i - 1];
            }
            buffer->lines[cursor.y + 1] = new_line;
            buffer->nlines++;
            cursor.y++;
            cursor.x = 0;
            buffer->modified = 1;
        }
        else if (ch == 19) { // Ctrl+S para salvar
            if (buffer->filename[0] == '\0') {
                // "Salvar como..." se ainda não há nome
                echo();
                mvprintw(max_y - 1, 0, "Salvar como: ");
                char fname[256];
                getnstr(fname, 255);
                noecho();
                if (buffer_save_file(buffer, fname)) {
                    mvprintw(max_y - 1, 0, "Arquivo salvo com sucesso!                      ");
                } else {
                    mvprintw(max_y - 1, 0, "Erro ao salvar arquivo!                          ");
                }
            } else {
                // Salvamento direto se nome já existe
                if (buffer_save_file(buffer, buffer->filename)) {
                    mvprintw(max_y - 1, 0, "Arquivo salvo com sucesso!                      ");
                } else {
                    mvprintw(max_y - 1, 0, "Erro ao salvar arquivo!                          ");
                }
            }
            getch(); // pausa para mostrar mensagem
        }
        else if (ch == 15) { // Ctrl+O para abrir arquivo
            echo();
            mvprintw(max_y - 1, 0, "Abrir arquivo: ");
            char fname[256];
            getnstr(fname, 255);
            noecho();
            if (buffer_open_file(buffer, fname)) {
                // Após carregar, reinicializa posição do cursor e scroll
                cursor.x = 0;
                cursor.y = 0;
                cursor.top_line = 0;
                mvprintw(max_y - 1, 0, "Arquivo aberto com sucesso!                     ");
            } else {
                mvprintw(max_y - 1, 0, "Erro ao abrir arquivo!                           ");
            }
            getch();
        }
        else if (ch >= 32 && ch <= 126) {
            // Apenas caracteres imprimíveis (ASCII padrão)
            buffer_insert_char(buffer, &cursor, ch);
        }

        // Scroll vertical automático
        if (cursor.y < cursor.top_line) {
            cursor.top_line = cursor.y;
        }
        if (cursor.y >= cursor.top_line + lines_on_screen) {
            cursor.top_line = cursor.y - lines_on_screen + 1;
        }

        // Impede que cursor.x vá além do fim da linha
        int line_len = strlen(buffer->lines[cursor.y]);
        if (cursor.x > line_len) cursor.x = line_len;
    }
}
