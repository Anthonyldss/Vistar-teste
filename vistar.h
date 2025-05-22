#ifndef VISTAR_H
#define VISTAR_H

#include <ncurses.h>

#define MAX_LINES 1000
#define MAX_COLS  1024

typedef struct {
    char **lines;       // array de strings (linhas)
    int nlines;         // número de linhas
    int max_lines;      // capacidade atual
    char filename[256]; // nome do arquivo aberto
    int modified;       // flag modificado
} TextBuffer;

typedef struct {
    int x, y;  // posição do cursor (coluna, linha)
    int top_line; // linha do topo da tela para rolagem vertical
} Cursor;

void vistar_init(void);
void vistar_cleanup(void);
void vistar_loop(void);

#endif

