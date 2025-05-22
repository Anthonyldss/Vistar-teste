#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "vistar.h"
#include "libvistar.h"

static TextBuffer *buffer;
static Cursor cursor;

void draw_text(void) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int lines_on_screen = max_y - 1; // linha inferior para status

    clear();

    for (int i = 0; i < lines_on_screen; i++) {
        int line_idx = cursor.top_line + i;
        if (line_idx >= buffer->nlines) break;
        mvprintw(i, 0, "%s", buffer->lines[line_idx]);
    }

    // desenha barra de status
    move(max_y - 1, 0);
    clrtoeol();
    char status[256];
    snprintf(status, sizeof(status), "%s - Linha %d, Col %d %s",
             buffer->filename[0] ? buffer->filename : "[Sem Nome]",
             cursor.y + 1, cursor.x + 1,
             buffer->modified ? "(Modificado)" : "");
    mvprintw(max_y - 1, 0, status);

    move(cursor.y - cursor.top_line, cursor.x);
    refresh();
}

void vistar_init(void) {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(1);
    start_color();

    buffer = buffer_create();
    cursor.x = 0;
    cursor.y = 0;
    cursor.top_line = 0;
}

void vistar_cleanup(void) {
    buffer_free(buffer);
    endwin();
}

void vistar_loop(void) {
    int ch;
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int lines_on_screen = max_y - 1;

    while (1) {
        draw_text();

        ch = getch();

        if (ch == 24) { // Ctrl+X para sair
            if (buffer->modified) {
                mvprintw(max_y - 1, 0, "Arquivo modificado. Pressione Ctrl+X novamente para sair sem salvar.");
                int ch2 = getch();
                if (ch2 == 24) break;
            } else {
                break;
            }
        }
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
            int line_len = strlen(buffer->lines[cursor.y]);
            if (cursor.x > line_len) cursor.x = line_len;
        }
        else if (ch == KEY_DOWN) {
            if (cursor.y < buffer->nlines - 1) cursor.y++;
            int line_len = strlen(buffer->lines[cursor.y]);
            if (cursor.x > line_len) cursor.x = line_len;
        }
        else if (ch == KEY_BACKSPACE || ch == 127) {
            if (buffer_delete_char(buffer, &cursor)) {
                if (cursor.y < cursor.top_line) cursor.top_line = cursor.y;
            }
        }
        else if (ch == '\n' || ch == '\r') {
            // Quebra de linha simples: divide a linha atual na posição do cursor
            char *line = buffer->lines[cursor.y];
            int len = strlen(line);

            char *new_line = strdup(line + cursor.x);
            line[cursor.x] = '\0';

            // inserindo nova linha no buffer
            if (buffer->nlines + 1 >= buffer->max_lines) continue; // limite max

            // desloca linhas para baixo
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
                if (buffer_save_file(buffer, buffer->filename)) {
                    mvprintw(max_y - 1, 0, "Arquivo salvo com sucesso!                      ");
                } else {
                    mvprintw(max_y - 1, 0, "Erro ao salvar arquivo!                          ");
                }
            }
            getch();
        }
        else if (ch == 15) { // Ctrl+O para abrir arquivo
            echo();
            mvprintw(max_y - 1, 0, "Abrir arquivo: ");
            char fname[256];
            getnstr(fname, 255);
            noecho();
            if (buffer_open_file(buffer, fname)) {
                cursor.x = 0;
                cursor.y = 0;
                cursor.top_line = 0;
                mvprintw(max_y - 1, 0, "Arquivo aberto com sucesso!                     ");
            } else {
                mvprintw(max_y - 1, 0, "Erro ao abrir arquivo!                           ");
            }
            getch();
        }
        else if (ch >= 32 && ch <= 126) { // caracteres imprimíveis
            buffer_insert_char(buffer, &cursor, ch);
        }

        // Ajusta scroll vertical (top_line)
        if (cursor.y < cursor.top_line) {
            cursor.top_line = cursor.y;
        }
        if (cursor.y >= cursor.top_line + lines_on_screen) {
            cursor.top_line = cursor.y - lines_on_screen + 1;
        }

        // Ajusta cursor x para linha atual
        int line_len = strlen(buffer->lines[cursor.y]);
        if (cursor.x > line_len) cursor.x = line_len;
    }
}

