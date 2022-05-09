#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "index.h"
#include "map.h"
#include "index.h"




/*
 * Struct that holds the documents name and words
 */
typedef struct document document_t;

struct document
{
    map_t* hash;
    list_t* words, *word_placements, *word_placements_correct;
    list_iter_t* current;
    char** word_array;
    char* name;
    int size, words_found;
};


/*
 * Frees memory used by the document.
 */
void document_destroy(document_t *document);

#endif