
#include <string.h>

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
    map_t* hash;
    list_t* linkl;
    search_result_t* res;
};

/*
 * Struct to hold a single search result.
 * Implement this to work with the search result functions.
 */
struct search_result
{
    document_t* document;
    list_t* linkl;
    int size;
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
    index->hash = map_create(cmp_strs,djb2);
    return index;
}


/*
 * Destroys the given index.  Subsequently accessing the index will
 * lead to undefined behavior.
 */
void index_destroy(index_t *index)
{
    free(index->hash);
    free(index);
}



/*
 * Adds all the words from the given document to the given index.
 * This function is responsible for deallocating the list and the document name after use.
 */
void index_add_document(index_t *idx, char *document_name, list_t *words)
{
    list_iter_t *iter;
    search_result_t *search_result = malloc(sizeof(search_result_t));
    list_t *combo;

    search_result->document = document_create();
    // Store document data for later
    list_addlast(search_result->document->name, document_name);
    search_result->document->words = words;
    map_put(search_result->document->hash, document_name, words);

    int placement = 0;
    iter = list_createiter (words);
    // Iterate through list of files
    while (list_hasnext(iter))
    {
        placement ++;
        // Iterate through words in file
        char *word = list_next(iter);
        // "SKIP" spaces
        if (word == " " || word == "\0" || word == "\n"){
            placement --;
        }
        // Create linkedlist for duplicate words
        combo = list_create(cmp_ints);
        // Push word last in linkedlist if the map already contains the word
        if (map_haskey(idx->hash, word)){
            list_addlast(map_get(idx->hash,word), placement);

        }
        // Create new linkedlist if map does not containt the word
        else{
            list_addfirst(combo,placement);
            map_put(idx->hash,word, combo);
        }
    //printf("%s",word);
    //printf("");
    }
    result_get_content(search_result);
    list_destroyiter(iter);
}

/*
 * Finds a query in the documents in the index.
 * The result is returned as a search_result_t which is later used to iterate the results.
 */
search_result_t *index_find(index_t *idx, char *query)
{
    list_iter_t *iter;
    search_result_t *search_result = malloc(sizeof(search_result_t));
    list_t *list;
    int length;

    // Open map
    list = map_get(idx->hash,query);
    if (list != NULL){
        iter = list_createiter(list);
        search_result->linkl = list_create(cmp_ints);
        // Get word length
        length = strlen(query);
        // Iterate through the list and push indexes to linkedlist
        while (list_hasnext(iter))
        {
            int *index_placement = list_next(iter);
            list_addlast(search_result->linkl, index_placement);
        }
        // Used for NULL (end) Checks
        list_addlast(search_result->linkl, NULL);

        // Useless, used to see if code is working or not, has nothing
        // to do with the code it self.
        iter = list_createiter(search_result->linkl);
        // printf("Length: %i\n", length);
        // printf("Index");
        // while (list_hasnext(iter))
        // {
        //     printf(" - %i", list_next(iter));
        // }
        // printf("\n");
        list_destroyiter(iter);
        list_destroy(list);

        //return search_result;
    }
    else{
        printf("the word \"%s\" was not found in your document\n", query);
        search_result->linkl = NULL;
    }
    return search_result;
}


/*
 * Autocomplete searches the given trie for a word containing input.
 * The input string is NULL terminated and contains size letters (excluding null termination).
 * The output string MUST be null terminated.
 */ 

char *autocomplete(index_t *idx, char *input, size_t size)
{
    return NULL;
}

/* 
 * Return the content of the current document.
 * Subsequent calls to this function should return the next document.
 * This function should only be called once for each document.
 * This function should return NULL if there are no more documents.
 */
char **result_get_content(search_result_t *res)
{
    list_iter_t *docu_iter, *word_iter;
    list_t *list;

    //printf("\n%s", list_next(res->document->name));
    res->document->current = list_next(res->document->name);

    printf("%s\n",res->document->name);
    //docu_iter = list_createiter(res->document->name);
    // list = map_get(res->document->hash, list_next(docu_iter));
    // word_iter = list_createiter(list);

    // while (list_hasnext(word_iter))
    // {
    //     printf("%s", list_next(word_iter));
    // }

    return NULL;
}


/*
 * Get the length of the current document.
 * Subsequent calls should return the length of the same document.
 */
int result_get_content_length(search_result_t *res)
{
    return 50;
}


/*
 * Get the next result from the current query.
 * The result should be returned as an int, which is the index in the document content.
 * Should return NULL at the end of the search results.
 */
search_hit_t *result_next(search_result_t *res)
{
    // hver gang man trykker enter så går den til neste lokasjon
    search_hit_t *search_hit = malloc(sizeof(search_hit_t));
    list_iter_t *iter;

    iter = list_createiter(res->linkl);

    if (res == NULL){
        return search_hit;
    }
    else{
        search_hit->location = list_next(iter);
        search_hit->len = res->size;
        //printf("\nlocation: %d ---- length: %d\n",search_hit->location, search_hit->len);

        return search_hit;
    }
}