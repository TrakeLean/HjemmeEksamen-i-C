
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "index.h"
#include "list.h"
#include "printing.h"
#include "document.h"

const char *example[15] = {
    "\"",
    "Lorem",
    " ",
    "ipsum",
    " ", 
    "dolor",
    " ",
    "sit",
    " ", 
    "amet",
    ",",
    "consectetur",
    " ", 
    "adipiscing",
    "...",
    "\""
};


list_t *generate_document(int nwords)
{
    list_t *l = list_create(compare_strings);
    for (int i = 0; i < nwords; i++)
    {
        list_addfirst(l, strdup(example[i % 15]));
    }
    return l;
}

void destroy_doc(list_t *l)
{
    char *p = list_popfirst(l);
    while(p != NULL)
    {
        free(p);
        p = list_popfirst(l);
    }
    list_destroy(l);
}


int main(int argc, char **argv)
{
    unsigned long long before = 0, after = 0;
    int nwords = 0;
    int nruns = 0;
    double avg_add = 0;
    double avg_find = 0;
    double avg_find_miss = 0;

    if (argc < 3)
    {
        nwords = 100000;
        nruns = 10;
    }
    else
    {
        nwords = atoi(argv[1]);
        nruns = atoi(argv[2]);
    }

    list_t *words = NULL;
    char *doc_name = "test";


    TEST_PRINT("RUNNING BENCHMARKS\n");

    TEST_PRINT("Adding documents to index\n");
    fprintf(stdout, "N Words|Time(µs)|Time/Word(µs)|Search Hit Time(µs)|Search Miss Time(µs)\n");
    for (int i = 0; i < nruns; i++)
    {
        index_t *idx = index_create();
        document_t *document = document_create();
        if (idx == NULL)
        {
            ERROR_PRINT("Could not allocate index");
            goto error;
        }

        TEST_PRINT("Inserting %d words\n", nwords);
        words = generate_document(nwords);

        // Test inserts
        before = gettime();
        index_add_document(idx, doc_name, words);
        after = gettime();
        fprintf(stdout, "%d %llu %lf ", nwords, after - before, (double)(after - before) / nwords);
        avg_add += (double)(after - before) / nwords;

        // Test find on existing word
        before = gettime();
        index_find(idx, example[after % 15]);
        after = gettime();
        fprintf(stdout, "%llu ", after - before);

        avg_find += (double)(after - before);

        // Test find on non-existing word
        before = gettime();
        index_find(idx, "noexist");
        after = gettime();
        fprintf(stdout, "%llu\n", after - before);

        avg_find_miss += (double)(after - before);

        destroy_doc(words);

        nwords *= 2;
        index_destroy(idx);
    }

    TEST_PRINT("Average time to insert a single word: %lf µs\n", (double)avg_add / nruns);
    TEST_PRINT("Average time to find a single that exist (hit) word: %lf µs\n", (double)avg_find / nruns);
    TEST_PRINT("Average time to find a single that does not exist (miss) word: %lf µs\n", (double)avg_find_miss / nruns);

    return 0;

error:
    return 1;

}