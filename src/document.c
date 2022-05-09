#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "document.h"
#include "list.h"
#include "index.h"

void document_destroy(document_t *document)
{
    free(document);
}