
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
    list_t* temp_all_doc_names, *temp_size_list, *document_data;
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
    list_iter_t* docu_iter, *size_iter, *hit_iter;
    char** array;
    int curr_number_word_found, word_size;
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
    index->document_data = list_create(cmp_strs);
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
    list_destroy(index->document_data);
    free(index);
}


/*
 * Adds all the words from the given document to the given index.
 * This function is responsible for deallocating the list and the document name after use.
 */
void index_add_document(index_t *idx, char *document_name, list_t *words)
{
    list_iter_t *word_iter;
    document_t *document = document_create();
    list_t* list;
    document->word_array = malloc(sizeof(char*)*list_size(words));


    // Store document data for later
    document->name = document_name;
    document->size = list_size(words);

    int placement = 0;
    int correct_placement = 0;
    word_iter = list_createiter (words);

    // Iterate through words in file
    while (list_hasnext(word_iter))
    {
        char *word = list_next(word_iter);
        correct_placement++;
        // Add word in array for "result_get_content" function
        document->word_array[placement] = word;
        // "SKIP" spaces,newlines, null terminations, commas and periods
        if (word == " " || word == "\0" || word == "\n")
        {
            correct_placement --;
        }
        // Push word last in linkedlist if the map already contains the word
        if (map_haskey(document->hash, word))
        {
            list_addlast(map_get(document->hash, word), placement);
            list_addlast(map_get(document->hash, word), correct_placement);

        }
        // Enter here if the word is not contained within the hashtable
        else
        {
            // Create linkedlist incase of duplicate words
            list = list_create(cmp_ints);
            list_addfirst(list, placement);
            list_addlast(list, correct_placement);
            map_put(document->hash, word, list);
            trie_insert(idx->trie, word, NULL);
        }
        placement++;
    }
    list_addlast(idx->document_data, document);

}

/*
 * Finds a query in the documents in the index.
 * The result is returned as a search_result_t which is later used to iterate the results.
 */
search_result_t *index_find(index_t *idx, char *query)
{
    list_iter_t *docu_iter, *iter, *iter2, *iter3;
    search_result_t *result = malloc(sizeof(search_result_t));
    list_t *list;


    docu_iter = list_createiter(idx->document_data);
    while (list_hasnext(docu_iter))
    {
        result->document = list_next(docu_iter);

        result->document->word_placements = list_create(cmp_ints);
        result->document->word_placements_correct = list_create(cmp_ints);
        // Get word length
        result->word_size = strlen(query);
        int found = 0;
        // Open map
        list = map_get(result->document->hash,query);
        if (list == NULL){
            list_addlast(result->document->word_placements, NULL);
            list_addlast(result->document->word_placements_correct, NULL);
        }
        else{
            iter = list_createiter(list);
            // Iterate through the list and push placements to linkedlist
            while (list_hasnext(iter))
            {
                int placement = list_next(iter);
                int correct_placement = list_next(iter);
                list_addlast(result->document->word_placements, placement);
                list_addlast(result->document->word_placements_correct, correct_placement);
                found ++;
            }
            result->document->words_found = found;
        
            list_destroyiter(iter);
        }
    }

    iter = list_createiter(idx->document_data);
    while (list_hasnext(iter))
    {
    result->document = list_next(iter);

    printf("\n%s\n", result->document->name);
    // PRINT OUT COMPUTER PLACEMENTS
    iter2 = list_createiter(result->document->word_placements);
    printf("Computer placement ");
        while (list_hasnext(iter2))
        {
            printf("- %i ", list_next(iter2));
        }
        printf("\n");
    // PRINT OUT HUMAN PLACEMENTS FOUND
    iter3 = list_createiter(result->document->word_placements_correct);
    printf("Human placement ");
        while (list_hasnext(iter3))
        {
            printf("- %i ", list_next(iter3));
        }
        printf("\n");
        printf("Words found: %i\n", result->document->words_found);
    }



return result;
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

    if (res->document != NULL)
    {
        return res->document->word_array;
    }
    else{
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
    return res->document->size;
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
    document_t *document;


    if (res == NULL){
        return NULL;
    }

    else{
        search_hit->location = list_next(res->document->word_placements);
        search_hit->word_placement = list_next(res->document->word_placements_correct);
        search_hit->words_found = res->document->words_found;
        search_hit->len = res->word_size;

        return search_hit;
    }
    return NULL;
}