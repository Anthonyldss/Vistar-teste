#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libvistar.h"

TextBuffer *buffer_create(void) {
    TextBuffer *buf = malloc(sizeof(TextBuffer));
    buf->max_lines = MAX_LINES;
    buf->nlines = 1;
    buf->lines = malloc(sizeof(char*) * buf->max_lines);
    for (int i = 0; i < buf->max_lines; i++) {
        buf->lines[i] = NULL;
    }
    buf->lines[0] = calloc(1, 1); // linha vazia
    buf->modified = 0;
    buf->filename[0] = '\0';
    return buf;
}

void buffer_free(TextBuffer *buf) {
    if (!buf) return;
    for (int i = 0; i < buf->nlines; i++) {
        free(buf->lines[i]);
    }
    free(buf->lines);
    free(buf);
}

int buffer_insert_char(TextBuffer *buf, Cursor *cursor, int c) {
    if (cursor->y >= buf->nlines) return 0;

    char *line = buf->lines[cursor->y];
    int len = strlen(line);

    if (len + 2 > MAX_COLS) return 0; // limita tamanho linha

    char *new_line = malloc(len + 2);
    memcpy(new_line, line, cursor->x);
    new_line[cursor->x] = (char)c;
    memcpy(new_line + cursor->x + 1, line + cursor->x, len - cursor->x + 1);

    free(buf->lines[cursor->y]);
    buf->lines[cursor->y] = new_line;

    cursor->x++;
    buf->modified = 1;

    return 1;
}

int buffer_delete_char(TextBuffer *buf, Cursor *cursor) {
    if (cursor->y >= buf->nlines) return 0;
    if (cursor->x == 0 && cursor->y == 0) return 0; // início do texto

    char *line = buf->lines[cursor->y];
    int len = strlen(line);

    if (cursor->x > 0) {
        // Apaga caractere à esquerda
        char *new_line = malloc(len);
        memcpy(new_line, line, cursor->x - 1);
        memcpy(new_line + cursor->x - 1, line + cursor->x, len - cursor->x + 1);
        free(buf->lines[cursor->y]);
        buf->lines[cursor->y] = new_line;
        cursor->x--;
        buf->modified = 1;
        return 1;
    } else {
        // cursor->x == 0 e y > 0 : junta linha atual com anterior
        int prev_len = strlen(buf->lines[cursor->y - 1]);
        char *new_line = malloc(prev_len + len + 1);
        memcpy(new_line, buf->lines[cursor->y - 1], prev_len);
        memcpy(new_line + prev_len, line, len + 1);
        free(buf->lines[cursor->y - 1]);
        buf->lines[cursor->y - 1] = new_line;

        // remove linha atual
        free(buf->lines[cursor->y]);
        for (int i = cursor->y; i < buf->nlines - 1; i++) {
            buf->lines[i] = buf->lines[i + 1];
        }
        buf->lines[buf->nlines - 1] = NULL;
        buf->nlines--;
        cursor->y--;
        cursor->x = prev_len;
        buf->modified = 1;
        return 1;
    }
}

int buffer_open_file(TextBuffer *buf, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;

    // limpa buffer atual
    for (int i = 0; i < buf->nlines; i++) {
        free(buf->lines[i]);
        buf->lines[i] = NULL;
    }
    buf->nlines = 0;

    char line[MAX_COLS];
    while (fgets(line, MAX_COLS, f)) {
        // remove \n
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

        if (buf->nlines >= buf->max_lines) break;

        buf->lines[buf->nlines] = strdup(line);
        buf->nlines++;
    }
    if (buf->nlines == 0) {
        buf->lines[0] = calloc(1, 1);
        buf->nlines = 1;
    }
    fclose(f);

    strncpy(buf->filename, filename, sizeof(buf->filename) - 1);
    buf->modified = 0;
    return 1;
}

int buffer_save_file(TextBuffer *buf, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return 0;

    for (int i = 0; i < buf->nlines; i++) {
        fprintf(f, "%s\n", buf->lines[i]);
    }
    fclose(f);

    strncpy(buf->filename, filename, sizeof(buf->filename) - 1);
    buf->modified = 0;
    return 1;
}

