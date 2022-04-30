#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "list.h"
#include "map.h"
#include "index.h"

/*
 * Struct that holds the documents name and words
 */
typedef struct document document_t;

struct document
{
    index_t* name;
    list_t* words;
};

/*
 * Locates memory for the document.
 */
document_t *document_create();

/*
 * Frees memory used by the document.
 */
void document_destroy(document_t *document);

#endif