
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
    // må lage egen hash for hvert dokument.
    map_t* hash, *array_hash, *hit_words;
    list_t* temp_all_doc_names, *temp_size_list;
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
    list_t* words_found, *all_documents, *size_list, *temp_search_hit_words, *words_found_correct, *list_number_found;
    list_iter_t* next_word_iter, *next_docu_iter, *next_size_iter, *next_correct_iter;
    char** curr_array;
    int curr_size, *last_place, *curr_number_word_found;
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
    map_destroy(index->array_hash, NULL, NULL);
    map_destroy(index->hit_words, NULL, NULL);
    list_destroy(index->temp_all_doc_names);
    list_destroy(index->temp_size_list);
    list_destroy(index->res->words_found);
    list_destroy(index->res->words_found_correct);
    list_destroy(index->res->list_number_found);
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

    int placement = 0;
    int correct_placement = 0;

    iter = list_createiter (words);
    // Iterate through words in file
    while (list_hasnext(iter))
    {
        //placement ++;
        correct_placement ++;
        // Iterate through words in file
        char *word = list_next(iter);
        // Add word in array for "result_get_content" function
        document->word_array[placement] = word;
        // "SKIP" spaces,newlines, null terminations, commas and periods
        if (word == " " || word == "\0" || word == "\n")
        {
            correct_placement --;
        }
        // Push word last in linkedlist if the map already contains the word
        if (map_haskey(idx->hash, word)){
            list_addlast(map_get(idx->hash,word), placement);
            list_addlast(map_get(idx->hash,word), correct_placement);
        }
        // Enter here if the word is not contained within the hashtable
        else{
            // Create linkedlist incase of duplicate words
            combo = list_create(cmp_ints);
            list_addfirst(combo,placement);
            list_addlast(combo,correct_placement);
            map_put(idx->hash,word, combo);
            trie_insert(idx->trie, word, NULL);
        }
        placement ++;
    }
    // Hashmap inputing every word into an array for result_get_content
    map_put(idx->array_hash, document_name, document->word_array);

    // store everything where it's needed
    search_result->all_documents = idx->temp_all_doc_names;
    search_result->document->hash = idx->array_hash;
    search_result->size_list = idx->temp_size_list;
    idx->res = search_result;
    idx->trie = idx->trie;
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
        idx->res->words_found_correct = list_create(cmp_ints);
        idx->res->list_number_found = list_create(cmp_ints);
        // Get word length
        idx->res->word_size = strlen(query);
        int found = 0;
        int end = 0;

        // Iterate through the list and push placements to linkedlist
        while (list_hasnext(iter))
        {
            int index_placement = list_next(iter);
            int correct_index_placement = list_next(iter);
            // Check if the last word is bigger than the current one
            // Checks for end of current document
            if (index_placement < end)
            {
                list_addlast(idx->res->words_found, NULL);
                list_addlast(idx->res->words_found_correct, NULL);
                list_addlast(idx->res->list_number_found, found);
                found = 0;
                end = 0;
            }     
            list_addlast(idx->res->words_found, index_placement);
            list_addlast(idx->res->words_found_correct, correct_index_placement);
            found ++;
            end = index_placement;
        }
        list_addlast(idx->res->list_number_found, found);

        list_destroyiter(iter);
        // Iterators used by result
        idx->res->next_word_iter = list_createiter(idx->res->words_found);
        idx->res->next_correct_iter = list_createiter(idx->res->words_found_correct);
        idx->res->next_docu_iter = list_createiter(idx->res->all_documents);
        idx->res->next_size_iter = list_createiter(idx->res->size_list);
    }
    else{
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
    else if (list_hasnext(res->next_docu_iter))
    {
        // Get current document 
        res->document->current = list_next(res->next_docu_iter);
        // Get current size
        res->curr_size = list_next(res->next_size_iter);
        // Get current array based on document name
        res->curr_array = map_get(res->document->hash, res->document->current);
        // Get total words found
        res->curr_number_word_found = list_next(res->list_number_found);
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
    if (res == NULL){
    return NULL;
    }
    else{
    return res->curr_size;
    }
}


/*
 * Get the next result from the current query.
 * The result should be returned as an int, which is the index in the document content.
 * Should return NULL at the end of the search results.
 */
search_hit_t *result_next(search_result_t *res)
{
    search_hit_t *search_hit = malloc(sizeof(search_hit_t));
    list_iter_t *word_iter, *correct_iter;

    if (res == NULL)
    {
        return NULL;
    }
    word_iter = res->next_word_iter;
    correct_iter = res->next_correct_iter;
    int curr_word = list_next(word_iter);
    int correct_word = list_next(correct_iter);
    // If curr_word and correct_word are equal it means that we have hit the end of a document
    if (curr_word == correct_word){
        return NULL;
    }
    else{
        search_hit->location = curr_word;
        search_hit->word_placement = correct_word;
        search_hit->words_found = res->curr_number_word_found;
        search_hit->len = res->word_size;

        return search_hit;
    }
}