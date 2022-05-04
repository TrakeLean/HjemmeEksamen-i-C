#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "list.h"
#include "index.h"
#include "document.h"
#include "printing.h"
#include "ui.h"



index_t *idx = NULL;


int mystrcmp(void *a, void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

static void interrupt_signal(int signo)
{
    index_destroy(idx);
    ui_deinit();
    INFO_PRINT("Interrupt signal caught (%d) - Exiting...\n", signo);
    exit(0);
}

void initialize_index(char *root_dir)
{
    char *relpath, *fullpath;
    list_t *files, *words;
    list_iter_t *iter;


    if (is_valid_directory(root_dir))
    {
        files = find_files (root_dir);
    }
    else if (is_valid_file(root_dir))
    {
        files = list_create(mystrcmp);

        list_addfirst(files, root_dir);
    }
    else
    {
        ERROR_PRINT("Root dir %s is not a valid directory or file\n", root_dir);
        exit(1);
    }

    idx = index_create ();
    if (idx == NULL)
    {
        ERROR_PRINT("Failed to create index\n");
    }

    iter = list_createiter (files);

    while (list_hasnext (iter))
    {
        relpath = (char *) list_next (iter);
        fullpath = concatenate_strings (2, root_dir, relpath);
        INFO_PRINT("Indexing %s\n", fullpath);

        words = list_create (mystrcmp);
        tokenize_file (fullpath, words);
        index_add_document (idx, relpath, words);

        free (fullpath);

        list_destroy (words);
    }
    //index_find(idx, "touch");
    list_destroyiter (iter);
    list_destroy (files);
}

void main_program_loop()
{
    if (signal(SIGINT, interrupt_signal) == SIG_ERR) 
    {
        ERROR_PRINT("An error occurred while setting a signal handler.\n");
        exit(1);
    }

    while (1)
    {
        char *query = ui_main(idx);
        search_result_t *res = index_find(idx, query);
        ui_result(res);
    }
}


int main(int argc, char **argv)
{
    // if (argc != 2)
    // {
    //     ERROR_PRINT("Usage: %s <root-dir>\n", argv[0]);
    //     return 1;
    // }

    //char *root_dir = argv[1];
    char *root_dir = "data2";
    initialize_index(root_dir);

    //ui_init();
    //main_program_loop();

    return 0;
}