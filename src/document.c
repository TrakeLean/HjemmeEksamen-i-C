#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "document.h"
#include "list.h"
#include "index.h"

document_t *document_create()
{
    document_t* document = malloc(sizeof(document_t));
    return document;

}

void document_destroy(document_t *document)
{
    free(document);
}