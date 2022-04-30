#ifndef INDEX_H
#define INDEX_H

#include "list.h"
#include "trie.h"

typedef struct index index_t;

typedef struct search_result search_result_t;
/*
 * Used by the UI to display results.
 * You can add to this struct, but do not remove location and len variables.
 */
typedef struct search_hit search_hit_t;
struct search_hit
{
    int location;
    int len;
};

/*
 * Creates a new, empty index.
 */
index_t *index_create();


/*
 * Destroys the given index.  Subsequently accessing the index will
 * lead to undefined behavior.
 */
void index_destroy(index_t *index);


/*
 * Adds all the words from the given document to the given index.
 * This function is responsible for deallocating the list and the document name after use.
 */
void index_add_document(index_t *idx, char *document_name, list_t *words);


/*
 * Finds a query in the documents in the index.
 * The result is returned as a search_result_t which is later used to iterate the results.
 */
search_result_t *index_find(index_t *idx, char *query);


/*
 * Autocomplete searches the given trie for a word containing input.
 * The input string is NULL terminated and contains size letters (excluding null termination).
 * The output string MUST be null terminated.
 */ 
char *autocomplete(index_t *idx, char *input, size_t size);



// Result handling

/* 
 * Return the content of the current document.
 * Subsequent calls to this function should return the next document.
 * This function should only be called once for each document.
 * This function should return NULL if there are no more documents.
 */
char **result_get_content(search_result_t *res);

/*
 * Get the length of the current document.
 * Subsequent calls should return the length of the same document.
 */
int result_get_content_length(search_result_t *res);

/*
 * Get the next result from the current query.
 * The result should be returned as an int, which is the index in the document content.
 * Should return NULL at the end of the search results.
 */
search_hit_t *result_next(search_result_t *res);


#endif


