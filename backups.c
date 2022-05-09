void index_add_document(index_t *idx, char *document_name, list_t *words)
{
    list_iter_t *iter;
    int placement = 0;
    iter = list_createiter (words);
    while (list_hasnext(iter))
    {
        placement ++;
        char *word = list_next(iter);
        map_put(idx,word, placement);
        printf("%s",word);
        //printf("%s -%i|",word, placement);
    }
    char *ord = "touch";
    char *test = map_get(idx,ord);
    printf("%s: %i\n", ord, test);
    list_destroyiter(iter);

}


search_result->document->word_array = malloc(sizeof(char*)*list_size(words));


char *trie_find(trie_t *trie, char *key)
{
    node_t *iter = trie->root;

    for (int i = 0; key[i] != '\0'; i++){
        if (iter == NULL){
            return NULL;
        }
    }
    if (iter->key != NULL){
        return iter->key;
    }
    int i;
    while (iter != NULL)
    {
        if (isleaf(iter)){
            return iter->key;
        }
        else{
            for (int i = 0; i < TRIE_RADIX; i++){
                if (iter->children[i] != NULL)
                {
                    iter = iter->children[i];
                }
            }
        }
    }
    return NULL;
}


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

