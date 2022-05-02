#include <stdlib.h>
#include <string.h>

#include "index.h"
#include "map.h"
#include "list.h"
#include "document.h"


document_t *document_create()
{
    document_t *document = malloc(sizeof(document_t));
    document->hash = map_create(strcmp,djb2);
    document->name = list_create(strcmp);
    document->words = list_create(strcmp);
    return document;

}

void document_destroy(document_t *document)
{
    free(document->hash);
    free(document->name);
    free(document->words);
    free(document);
}