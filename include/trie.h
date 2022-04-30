#ifndef TRIE_H
#define TRIE_H

#include "common.h"

typedef struct trie trie_t;

/*
 * Creates an empty trie
 */
trie_t *trie_create();


/*
 * Destroys the given trie and its nodes
 */
void trie_destroy(trie_t *trie);


/*
 * Inserts a key and value into the trie
 */
int trie_insert(trie_t *trie, char *key, void *value);


/*
 * Finds a word in the trie.
 * IMPLEMENT THIS TO WORK WITH YOUR DESIGN.
 */
char *trie_find(trie_t *t, char *prefix);

#endif

