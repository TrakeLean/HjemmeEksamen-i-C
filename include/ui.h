#ifndef UI_H
#define UI_H

#include "index.h"
#include "document.h"

#define MIN_SUGGESTION_LEN 3

void ui_init();

void ui_deinit();

char *ui_main(index_t *idx);

void ui_result(search_result_t *res);

#endif