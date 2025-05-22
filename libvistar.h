#ifndef LIBVISTAR_H
#define LIBVISTAR_H

#include "vistar.h"

TextBuffer *buffer_create(void);
void buffer_free(TextBuffer *buf);

int buffer_insert_char(TextBuffer *buf, Cursor *cursor, int c);
int buffer_delete_char(TextBuffer *buf, Cursor *cursor);

int buffer_open_file(TextBuffer *buf, const char *filename);
int buffer_save_file(TextBuffer *buf, const char *filename);

#endif

