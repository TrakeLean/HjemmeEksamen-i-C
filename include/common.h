/* Author: Steffen Viken Valvaag <steffenv@cs.uit.no> */
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdarg.h>

struct list;

/*
 * The type of comparison functions.
 */
typedef int (*cmpfunc_t)(void *, void *);

/*
 * The type of comparison functions.
 */
typedef void (*exchfunc_t)(void *, void *);

/*
 * The type of hash functions.
 */
typedef unsigned long (*hashfunc_t)(void *);

/*
 * Prints an error message and terminates the program.
 * Use this to report fatal errors that prevent your program from proceeding.
 */
void fatal_error(char *msg, ...);


/*
 * Reads the given file, and parses it into words (tokens).
 * Adds the words to the given list, in the same order that they
 * occur.
 *
 * This tokenizer ignores punctuation and whitespace and converts text, so if the text is
 * contains the text "Hello! This is an example...." the recognized
 * words will be "hello", "this", "is", "an", and "example".
 */

void tokenize_file(const char *filepath, struct list *list);

/*
 * Recursively finds the names of all files under the given root directory.
 * Returns the file names as a list of strings.
 */
struct list *find_files(const char *root);

/* 
 * Compares two strings using strcmp().
 */
int compare_strings(void *a, void *b);

/*
 * Hashes a string.
 */
unsigned long hash_string(void *s);

/*
 * Compares two pointers using their natural ordering, i.e. by
 * comparing the actual addresses that they point to.
 */
int compare_pointers(void *a, void *b);

/*
 * Concatenates a given number of strings, and return it as
 * a new string (it will be allocated using malloc).
 */
char *concatenate_strings (int num_strings, const char *first, ...);

/*
 * Checks if the given 'dirpath' is a valid directory.
 * 1 = valid
 * 0 = invalid
 */
int is_valid_directory (const char *dirpath);

/*
 * Checks if the given 'filepath' is a valid regular file.
 * 1 = valid
 * 0 = invalid
 */
int is_valid_file (const char *filepath);

/*
 * Get the current time in microseconds
 */ 
unsigned long long gettime();

#endif
