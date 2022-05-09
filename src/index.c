
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
    list_t* document_data;
    search_result_t* result;
    trie_t* trie;
};

/*
 * Struct to hold a single search result.
 * Implement this to work with the search result functions.
 */
struct search_result
{
    document_t* document;
    list_iter_t* docu_iter;
    char** array;
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
    index->result = malloc(sizeof(search_result_t));
    index->hash = map_create(cmp_strs,djb2);
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
    list_destroyiter(word_iter);
}

/*
 * Finds a query in the documents in the index.
 * The result is returned as a search_result_t which is later used to iterate the results.
 */
search_result_t *index_find(index_t *idx, char *query)
{
    list_iter_t *docu_iter, *list_iter, *iter, *iter2;
    list_t *list;


    docu_iter = list_createiter(idx->document_data);
   
    while (list_hasnext(docu_iter))
    {
        idx->result->document = list_next(docu_iter);
        
        idx->result->document->word_placements = list_create(cmp_ints);
        idx->result->document->word_placements_correct = list_create(cmp_ints);
        // Get word length
        idx->result->word_size = strlen(query);
        int found = 0;
        // Open map
        list = map_get(idx->result->document->hash,query);
        if (list == NULL){
            list_addlast(idx->result->document->word_placements, NULL);
            list_addlast(idx->result->document->word_placements_correct, NULL);
        }
        else{
            list_iter = list_createiter(list);
            // Iterate through the list and pushes placements to linkedlist
            while (list_hasnext(list_iter))
            {
                int placement = list_next(list_iter);
                int correct_placement = list_next(list_iter);
                list_addlast(idx->result->document->word_placements, placement);
                list_addlast(idx->result->document->word_placements_correct, correct_placement);
                found ++;
            }
            idx->result->document->words_found = found;
            // Tells us where the list ends
            list_addlast(idx->result->document->word_placements, NULL);
            list_addlast(idx->result->document->word_placements_correct, NULL);
        
            list_destroyiter(list_iter);
        }
    }

    // iter = list_createiter(idx->document_data);
    // while (list_hasnext(iter))
    // {
    // idx->result->document = list_next(iter);

    // printf("\n%s\n", idx->result->document->name);
    // // PRINT OUT COMPUTER PLACEMENTS
    // iter2 = list_createiter(idx->result->document->word_placements);
    // printf("Computer placement ");
    //     while (list_hasnext(iter2))
    //     {
    //         printf("- %i ", list_next(iter2));
    //     }
    //     printf("\n");
    // // PRINT OUT HUMAN PLACEMENTS FOUND
    // iter2 = list_createiter(idx->result->document->word_placements_correct);
    // printf("Human placement ");
    //     while (list_hasnext(iter2))
    //     {
    //         printf("- %i ", list_next(iter2));
    //     }
    //     printf("\n");
    //     printf("Words found: %i\n", idx->result->document->words_found);
    // }

    // iter = list_createiter(idx->document_data);
    // while (list_hasnext(iter))
    // {
    // result->document = list_next(iter);
    //     for (int i = 0; i < result->document->size; i++)
    //     {
    //         printf("%s", result->document->word_array[i]);
    //     }
    //     printf("\n");
    // }
    
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
    // Get current array based on document name
    res->array = res->document->word_array;
    return res->array;
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

    search_hit->location = list_next(res->document->word_placements);
    search_hit->word_placement = list_next(res->document->word_placements_correct);
    search_hit->words_found = res->document->words_found;
    search_hit->len = res->word_size;
    /* if word_placements and word_placements_correct are 0 at the same time
     * it means that we have hit the end of the list.
     */
    if (search_hit->location == 0 && search_hit->word_placement == 0)
    {
        //free(search_hit);
        return NULL;
    }
    return search_hit;
}