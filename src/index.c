
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
    map_t* hash, *array_hash, *hit_words;
    list_t* linkl, *temp_all_doc_names, *temp_size_list;
    search_result_t* res;
    trie_t* trie;
};

/*
 * Struct to hold a single search result.
 * Implement this to work with the search result functions.
 */
struct search_result
{
    document_t* document;
    list_t* words_found, *all_documents, *size_list, *temp_search_hit_words;
    list_iter_t* next_word_iter, *next_docu_iter, *next_size_iter;
    char** curr_array;
    int curr_size;
    int word_size;
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
    index->array_hash = map_create(cmp_strs,djb2);
    index->hit_words = map_create(cmp_strs,djb2);
    index->temp_all_doc_names = list_create(cmp_strs);
    index->temp_size_list = list_create(cmp_strs);

    return index;
}


/*
 * Destroys the given index.  Subsequently accessing the index will
 * lead to undefined behavior.
 */
void index_destroy(index_t *index)
{
    // free all parts of the table, free index, index = null
    //printf("%s",list_next(index->res->document->name));
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
    document_t *document = malloc(sizeof(document_t));
    document->word_array = malloc(sizeof(char*)*list_size(words));
    list_t *combo;

    // Add list sizes
    list_addlast(idx->temp_size_list,list_size(words));

    search_result->document = document_create();
    // Store document data for later
    list_addlast(idx->temp_all_doc_names, document_name);
    // viser hvor vi er i teksten
    // Sier hvor vi er nederst til høyre
    int placement = 0;


    iter = list_createiter (words);
    // Iterate through words in file
    while (list_hasnext(iter))
    {
        //placement ++;
        // Iterate through words in file
        char *word = list_next(iter);
        // Add word in array for "result_get_content" function
        document->word_array[placement] = word;
        // "SKIP" spaces,newlines and null terminations
        // if (word == " " || word == "\0" || word == "\n"){
        //     placement --;
        // }
        // Push word last in linkedlist if the map already contains the word
        if (map_haskey(idx->hash, word)){
            list_addlast(map_get(idx->hash,word), placement);
        }
        // Enter here if the word is not contained within the hashtable
        else{
            // Create linkedlist incase of duplicate words
            combo = list_create(cmp_ints);
            list_addfirst(combo,placement);
            map_put(idx->hash,word, combo);
        }
        placement ++;
    }
    // Hashmap inputing every word into an array for result_get_content
    map_put(idx->array_hash, document_name, document->word_array);

    search_result->all_documents = idx->temp_all_doc_names;
    search_result->document->hash = idx->array_hash;
    search_result->size_list = idx->temp_size_list;
    idx->res = search_result;
    list_destroyiter(iter);

}

/*
 * Finds a query in the documents in the index.
 * The result is returned as a search_result_t which is later used to iterate the results.
 */
search_result_t *index_find(index_t *idx, char *query)
{
    list_iter_t *iter;
    list_t *list;

    // Open map
    list = map_get(idx->hash,query);
    if (list != NULL){
        iter = list_createiter(list);
        idx->res->words_found = list_create(cmp_ints);
        // Get word length
        idx->res->word_size = strlen(query);
        // Iterate through the list and push indexes to linkedlist
        int test = 0;
        while (list_hasnext(iter))
        {
            int index_placement = list_next(iter);
            if (index_placement < test)
            {
                list_addlast(idx->res->words_found, NULL);
            }     
            list_addlast(idx->res->words_found, index_placement);
            test = index_placement;
        }

        list_destroyiter(iter);
        list_destroy(list);
        // Iterators used by result
        idx->res->next_word_iter = list_createiter(idx->res->words_found);
        idx->res->next_docu_iter = list_createiter(idx->res->all_documents);
        idx->res->next_size_iter = list_createiter(idx->res->size_list);

    }
    else{
        //printf("the word \"%s\" was not found in your document\n", query);
        idx->res = NULL;
    }
    return idx->res;
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
    if (list_hasnext(res->next_docu_iter))
    {
        // Get current document 
        res->document->current = list_next(res->next_docu_iter);

        res->curr_size = list_next(res->next_size_iter);
        // Get current array based on document name
        res->curr_array = map_get(res->document->hash, res->document->current);

        // printf("\nSize: %i\n", res->curr_size);

        // for (int i = 0; i < res->curr_size; i++)
        // {
        //     printf("%s", res->curr_array[i]);
        // }
        // printf("\n");
        return res->curr_array;
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
    return res->curr_size;
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

    iter = res->next_word_iter;
    int curr_word = list_next(iter);

    if (curr_word == NULL){
        //char curr_word = list_next(iter);
        return NULL;
    }
    else{
        search_hit->location = curr_word;
        search_hit->len = res->word_size;
        printf(" - %i", search_hit->location);
        //printf("\nlocation: %d ---- length: %d\n",search_hit->location, res->word_size);
        return search_hit;
    }
}