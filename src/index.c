
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "index.h"
#include "map.h"
#include "printing.h"
#include "trie.h"
#include "document.h"

/*
 * Implement your index here.
 */
struct index
{
    map_t *hash;
    list_t *document_data;
    search_result_t *result;
    trie_t *trie;
};

/*
 * Struct to hold a single search result.
 * Implement this to work with the search result functions.
 */
struct search_result
{
    document_t *document;
    list_iter_t *docu_iter;
    char **array;
    char *name;
    int words_found, word_size, size;
};

static inline int cmp_ints(void *a, void *b)
{
    return *((int *)a) - *((int *)b);
}

/*
 * Compares two strings without case-sensitivity
 */
static inline int cmp_strs(void *a, void *b)
{
    return strcasecmp((const char *)a, (const char *)b);
}

/*
 * Creates a new, empty index.
 */
index_t *index_create()
{
    index_t *index = malloc(sizeof(index_t));
    index->hash = map_create(cmp_strs, djb2);
    index->result = malloc(sizeof(search_result_t));
    index->document_data = list_create(cmp_strs);
    index->trie = trie_create();
    return index;
}

/*
 * Destroys the given index.  Subsequently accessing the index will
 * lead to undefined behavior.
 */
void index_destroy(index_t *index)
{
    // Free from index
    map_destroy(index->hash, NULL, NULL);
    list_destroy(index->document_data);
    trie_destroy(index->trie);
    free(index);
}

/*
 * Adds all the words from the given document to the given index.
 * This function is responsible for deallocating the list and the document name after use.
 */
void index_add_document(index_t *idx, char *document_name, list_t *words)
{
    document_t *document = document_create();
    document->word_array = malloc(sizeof(char *) * list_size(words));
    list_iter_t *docu_iter;
    list_t *list;

    // Create docuement map and list
    document->hash = map_create(cmp_strs, djb2);
    document->words = list_create(cmp_strs);
    // Store document data for later
    document->name = document_name;
    document->size = list_size(words);

    // Variable used for program to show the right word
    int placement = 0;
    // Variable used to get the right placement number
    int correct_placement = 0;

    docu_iter = list_createiter(words);
    // Iterate through words in file
    while (list_hasnext(docu_iter))
    {
        correct_placement++;

        char *word = list_next(docu_iter);
        // Add word in array for "result_get_content" function
        document->word_array[placement] = word;

        // "SKIP" spaces,newlines, null terminations, commas and periods
        if (word == " " || word == "\0" || word == "\n")
        {
            correct_placement--;
        }
        // Check if word is in map, if it is add placement
        if (map_haskey(document->hash, word))
        {
            list_addlast(map_get(document->hash, word), placement);
            list_addlast(map_get(document->hash, word), correct_placement);
        }
        // Enter if the word isn't in the map, create list and add placement
        else
        {
            list = list_create(cmp_ints);
            list_addlast(list, placement);
            list_addlast(list, correct_placement);
            map_put(document->hash, word, list);
            // Add words for autocompletion
            trie_insert(idx->trie, word, NULL);
        }
        placement++;
    }
    list_addlast(idx->document_data, document);
    list_destroyiter(docu_iter);
}

/*
 * Finds a query in the documents in the index.
 * The result is returned as a search_result_t which is later used to iterate the results.
 */
search_result_t *index_find(index_t *idx, char *query)
{
    list_iter_t *docu_iter, *list_iter;
    list_t *list;

    docu_iter = list_createiter(idx->document_data);
    // Iterate through documents
    while (list_hasnext(docu_iter))
    {
        idx->result->document = list_next(docu_iter);
        // Create lists
        idx->result->document->word_placements = list_create(cmp_ints);
        idx->result->document->word_placements_correct = list_create(cmp_ints);
        // Get word length
        idx->result->word_size = strlen(query);
        int found = 0;

        // Open map
        list = map_get(idx->result->document->hash, query);
        // If there is no content in list add NULL to placements for later checks
        if (list == NULL)
        {
            list_addlast(idx->result->document->word_placements, NULL);
            list_addlast(idx->result->document->word_placements_correct, NULL);
        }
        else
        {
            list_iter = list_createiter(list);
            // Iterate through the list and pushes placements to linkedlist
            while (list_hasnext(list_iter))
            {
                int placement = list_next(list_iter);
                int correct_placement = list_next(list_iter);
                list_addlast(idx->result->document->word_placements, placement);
                list_addlast(idx->result->document->word_placements_correct, correct_placement);
                found++;
            }
            idx->result->document->words_found = found;
            // Tells us where the list ends
            list_addlast(idx->result->document->word_placements, NULL);
            list_addlast(idx->result->document->word_placements_correct, NULL);

            list_destroyiter(list_iter);
        }
    }
    list_destroyiter(docu_iter);
    idx->result->docu_iter = list_createiter(idx->document_data);
    return idx->result;
}

/*
 * Autocomplete searches the given trie for a word containing input.
 * The input string is NULL terminated and contains size letters (excluding null termination).
 * The output string MUST be null terminated.
 */

char *autocomplete(index_t *idx, char *input, size_t size)
{
    char *suggestion = trie_find(idx->trie, input);
    return suggestion;
}

/*
 * Return the content of the current document.
 * Subsequent calls to this function should return the next document.
 * This function should only be called once for each document.
 * This function should return NULL if there are no more documents.
 */
char **result_get_content(search_result_t *res)
{
    if (res == NULL)
    {
        return NULL;
    }
    else if (list_hasnext(res->docu_iter))
    {
        // Get current document
        res->document = list_next(res->docu_iter);
        // Get current size
        res->size = res->document->size;
        // Get current document name
        res->name = res->document->name;
        // Get current array based on document name
        res->array = res->document->word_array;
        return res->array;
    }
    else
    {
        return NULL;
    }
}

/*
 * Get the length of the current document.
 * Subsequent calls should return the length of the same document.
 */
int result_get_content_length(search_result_t *res)
{
    return res->size;
}

/*
 * Get the next result from the current query.
 * The result should be returned as an int, which is the index in the document content.
 * Should return NULL at the end of the search results.
 */
search_hit_t *result_next(search_result_t *res)
{
    search_hit_t *search_hit = malloc(sizeof(search_hit_t));

    if (res == NULL)
    {
        return NULL;
    }
    // Get current placements
    search_hit->location = list_next(res->document->word_placements);
    search_hit->word_placement = list_next(res->document->word_placements_correct);
    /* This is where the NULL checks from index_find comes in.
     * if word_placements and word_placements_correct are 0 at the same time
     * it means that we have hit the end of the list.
     */
    if (search_hit->location == 0 && search_hit->word_placement == 0)
    {
        search_hit = NULL;
    }
    else
    {
        search_hit->words_found = res->document->words_found;
        search_hit->len = res->word_size;
        search_hit->document_name = res->name;
    }
    return search_hit;
}