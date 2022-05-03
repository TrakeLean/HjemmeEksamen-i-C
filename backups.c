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