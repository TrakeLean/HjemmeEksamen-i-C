#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "index.h"
#include "map.h"
#include "printing.h"
#include "trie.h"

/*
 * Struct that holds the documents name and words
 */
typedef struct document document_t;

struct document
{
    list_t* *name, *words;
    map_t* hash;
    char** word_array;
    char* current;
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