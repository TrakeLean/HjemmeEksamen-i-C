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

