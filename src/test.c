#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <ucontext.h>
#include <setjmp.h>

#include "trie.h"
#include "index.h"
#include "printing.h"
#include "common.h"
#include "list.h"

/* 
 * The unit test suite runs unit tests for the index and trie libraries.
 * It will catch segfaults and report where they occur.
 */

bool catch = true;
bool caught_segfault = false;
static jmp_buf jbuf;

int cmpfunc(void *a, void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

static int failed(char *msg)
{
    msg = (msg == NULL) ? "" : msg;
    fprintf(stderr, "%s", BRED);
    fprintf(stderr, "FAILED %s\n", msg);
    fprintf(stderr, "%s", reset);
    return 1;
}

static int ok()
{
    fprintf(stderr, "%s", BGRN);
    fprintf(stderr, "%s", "OK\n");
    fprintf(stderr, "%s", reset);
    return 0;
}


int test_trie()
{
    int num_failed = 0;
    int err = 0;

    // Test Create
    TEST_PRINT("trie_create(): ");
    trie_t *trie = trie_create(cmpfunc);
    num_failed += (trie != NULL) ? ok() : failed(NULL);

    // Test insert
    TEST_PRINT("trie_insert(): ");
    sigsetjmp(jbuf, !0);
    if (catch == true)
    {
        err = trie_insert(trie, "hello", NULL);
    }

    if (caught_segfault == true)
    {
        num_failed++;
        failed("Segmentation Fault");
    }
    else if (err != 0)
    {
        num_failed++;
        failed("Function returned non-zero");
    }
    else
    {
        ok();
    }
    catch = true;
    caught_segfault = false;


    // Test find
    TEST_PRINT("trie_find(): ");
    sigsetjmp(jbuf, !0);
    char *res;
    if (catch == true)
    {
        res = trie_find(trie, "hel");
    }

    if (caught_segfault == true)
    {
        num_failed++;
        failed("Segmentation Fault");
    }
    else if (res == NULL)
    {
        num_failed++;
        failed("Function returned NULL on existing word");
    }
    else if (strcmp(res, "hello") != 0)
    {
        num_failed++;
        failed("Trie find did not return expected value");
    }
    else
    {
        ok();
    }
    catch = true;
    caught_segfault = false;

    // Test destroy
    TEST_PRINT("trie_destroy(): ");
    sigsetjmp(jbuf, !0);
    if (catch == true)
    {
        trie_destroy(trie);
    }

    if (caught_segfault == true)
    {
        num_failed++;
        failed("Segmentation Fault");
    }
    else
    {
        ok();
    }
    catch = true;
    caught_segfault = false;

    TEST_PRINT("Trie -> Number of failed tests: %d\n\n", num_failed);

    return num_failed;
}


int test_index()
{
    int num_failed = 0;

    // Test Create
    TEST_PRINT("index_create(): ");
    index_t *idx = index_create();
    num_failed += (idx != NULL) ? ok() : failed("Index is NULL");

    // Test insert document
    TEST_PRINT("index_add_document(): ");
    sigsetjmp(jbuf, !0);
    if (catch == true)
    {
        list_t *words = list_create(cmpfunc);
        tokenize_file("data/hamlet.txt", words);
        index_add_document(idx, "hamlet", words);
    }

    if (caught_segfault == true)
    {
        num_failed++;
        failed("Segmentation Fault");
    }
    else
    {
        ok();
    }
    catch = true;
    caught_segfault = false;


    // Test find
    TEST_PRINT("index_find(): ");
    search_result_t *sr = NULL;
    sigsetjmp(jbuf, !0);
    if (catch == true)
    {
        sr = index_find(idx, "hamlet");
    }

    if (caught_segfault == true)
    {
        num_failed++;
        failed("Segmentation Fault");
    }
    else if (sr == NULL)
    {
        num_failed++;
        failed("Function returned NULL");
    }
    else
    {
        ok();
    }
    catch = true;
    caught_segfault = false;


    // Test autocomplete
    TEST_PRINT("autocomplete(): ");
    char *w = "haml";
    char *completed = NULL;
    sigsetjmp(jbuf, !0);
    if (catch == true)
    {
        completed = autocomplete(idx, w, 4);
    }

    if (caught_segfault == true)
    {
        num_failed++;
        failed("Segmentation Fault");
    }
    else if (completed == NULL)
    {
        num_failed++;
        failed("Function returned NULL");
    }
    else if (cmpfunc((void *)completed, "Hamlets") != 0)
    {
        num_failed++;
        failed("Function returned wrong value");
        fprintf(stderr, "\t\t\t%s -> %s\n", w, completed);
    }
    else
    {
        ok();
    }
    catch = true;
    caught_segfault = false;

    // Test result get content
    TEST_PRINT("result_get_content(): ");
    char **doc = NULL;
    sigsetjmp(jbuf, !0);
    if (catch == true)
    {
        doc = result_get_content(sr);
    }

    if (caught_segfault == true)
    {
        num_failed++;
        failed("Segmentation Fault");
    }
    else if (doc == NULL)
    {
        num_failed++;
        failed("Function returned NULL");
    }
    else if (cmpfunc((void *)doc[0], "The") != 0)
    {
        num_failed++;
        failed("Function returned wrong value");
    }
    else
    {
        ok();
    }
    catch = true;
    caught_segfault = false;


    // Test result get content length
    TEST_PRINT("result_get_content_length(): ");
    int length = 0;
    sigsetjmp(jbuf, !0);
    if (catch == true)
    {
        length = result_get_content_length(sr);
    }

    if (caught_segfault == true)
    {
        num_failed++;
        failed("Segmentation Fault");
    }
    else if (length == 0)
    {
        num_failed++;
        failed("Function returned 0");
    }
    else
    {
        ok();
    }
    catch = true;
    caught_segfault = false;


    // Test result next
    TEST_PRINT("result_next(): ");
    search_hit_t *next = NULL;
    sigsetjmp(jbuf, !0);
    if (catch == true)
    {
        next = result_next(sr);
    }

    if (caught_segfault == true)
    {
        num_failed++;
        failed("Segmentation Fault");
    }
    else if (next == NULL)
    {
        num_failed++;
        failed("Function returned NULL");
    }
    else if (next->location != 6)
    {
        num_failed++;
        failed("Function returned wrong location");
        fprintf(stderr, "\t\t\tnext->location -> %d\n", next->location);

    }
    else
    {
        ok();
    }
    catch = true;
    caught_segfault = false;


    // Test index destroy
    TEST_PRINT("index_destroy(): ");
    
    sigsetjmp(jbuf, !0);
    if (catch == true)
    {
        index_destroy(idx);
    }

    if (caught_segfault == true)
    {
        num_failed++;
        failed("Segmentation Fault");
    }
    else
    {
        ok();
    }
    catch = true;
    caught_segfault = false;

    free(sr);
    free(completed);

    TEST_PRINT("Index -> Number of failed tests: %d\n\n", num_failed);

    return num_failed;
}

void segfault_handler(int signo)
{
    if (signo != SIGSEGV)
    {
        fprintf(stderr, "SIGSEGV(%d) expected, got %d", SIGSEGV, signo);
    }
    caught_segfault = true;
    catch = false;
    siglongjmp(jbuf, 1);
}

int main()
{

    if (signal (SIGSEGV, segfault_handler) == SIG_ERR)
    {
        DEBUG_PRINT("Not allowed to catch SIGSEGV\n");
    }
    int total_failed = 0;

    TEST_PRINT("RUNNING TEST SUITE\n");
    total_failed += test_trie();
    total_failed += test_index();

    printf("\n");
    TEST_PRINT("%d functions failed the unit tests.\n\n", total_failed);
}