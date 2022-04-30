/* 
 * Authors: 
 * Steffen Viken Valvaag <steffenv@cs.uit.no> 
 * Magnus Stenhaug <magnus.stenhaug@uit.no> 
 * Erlend Helland Graff <erlend.h.graff@uit.no> 
 */

#include "common.h"
#include "list.h"
#include "printing.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <ctype.h>

/*
 * parse_word takes a word token and reads out any special characters from it.
 * The special characters are treated as their own words.
 * Special characters and valid words are extracted and placed in the given list as individual elements.
 */
static void parse_word(char *token, list_t *l)
{
    // Special characters to match and include
    char match_chars[] = " \n\t\".,!:;?()-";

    // Not a valid string
    if (token == NULL)
    {
        return;
    }

    char *start = token;
    char *special_char = strpbrk(start, match_chars);
    
    // No special characters in the token
    if (special_char == NULL)
    {
        char *word = strdup(token);
        if (word == NULL)
        {
            goto error;
        }

        list_addlast(l, word);
        return;
    }
    
    // Handle special characters at the start and end of the token
    while (*start != '\0')
    {
        
        if (special_char == start)  // Encountered special character at the start of the token
        {
            // Allocate len + 1, which will null terminate the string when using calloc
            char *word = (char *)calloc(2, sizeof(char));
            word[0] = *special_char;

            list_addlast(l, word);
            start++;
            special_char = strpbrk(start, match_chars);
        }
        else if (special_char > start) // Encountered special character at the end of the token
        {
            int len = special_char - start;

            // Allocate len, which will null terminate the string when using calloc
            char *word = (char *)calloc(len+1, sizeof(char));
            memcpy(word, token, len);
            list_addlast(l, word);
            start = special_char;
        }
        else if (special_char == NULL) // Still a word left after token 
        {
            char *word = strdup(start);
            list_addlast(l, word);
            start += strlen(word);
        }
    }

    return;

error:
    ERROR_PRINT("Error occured parsing the token %s\n", token);
    return;
}


/*
 * tokenize_file opens and reads the given filename, including special characters.
 *
 * 
 */
void tokenize_file (const char *filename, list_t *list)
{
    FILE *fp;
    char *saveptr, *subsaveptr;

    fp = fopen (filename, "rb");
    if (!fp)
    {
        goto error;
    }

    // Find the filesize and reset filepointer
    fseek(fp, 0, SEEK_END);
    size_t filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *filebuffer = (char *)calloc(filesize, sizeof(char));
    if (filebuffer == NULL)
    {
        goto error;
    }

    fscanf(fp, "%[a-zA-Z0-9 \n\t\"\'.,!:;?()-]", filebuffer);
    filebuffer = realloc(filebuffer, strlen(filebuffer));
    if (filebuffer == NULL)
    {
        goto error;
    }

    fclose(fp);


    char *lines = strtok_r(filebuffer, "\n", &saveptr);

    while (lines  != NULL)
    {
        char *current_line = strtok_r(lines, " ", &subsaveptr);
        parse_word(current_line, list);
        list_addlast(list, " ");
        
        while (current_line != NULL)
        {
            current_line = strtok_r(NULL, " ", &subsaveptr);
            parse_word(current_line, list);
            list_addlast(list, " ");
            
        }
        // Newlines are consumed by the strtok function
        list_addlast(list, "\n");
        lines = strtok_r(NULL, "\n", &saveptr);
    }

    free(filebuffer);
    return;

error:
    if (!fp)
    {
        ERROR_PRINT("fopen() failed\n");
        return;
    }

    if (filebuffer != NULL)
    {
        free(filebuffer);
    }

    ERROR_PRINT("Out of memory or realloc failed\n");
    return;
}

char *concatenate_strings (int num_strings, const char *first, ...)
{
    int i, len;
    const char *str;
    char *ret;
    va_list args;

    /* Number of strings must be larger or equal to 1 */
    assert (num_strings >= 1);

    len = strlen (first);

    va_start (args, first);
    for (i = 1; i < num_strings; i++)
    {
        str = va_arg (args, const char *);
        len += strlen (str);
    }
    va_end (args);

    ret = malloc (len + 1);
    if (!ret)
        return NULL;

    /* Start by copying first string */
    strcpy (ret, first);

    /* Loop through the rest of the strings, concatinating them to the end */
    va_start (args, first);
    for (i = 1; i < num_strings; i++)
    {
        str = va_arg (args, const char *);
        strcat (ret, str);
    }
    va_end (args);

    return ret;
}


static int dir_filter (const struct dirent *entry)
{
    char *filename;
    struct stat statbuf;

    filename = (char *) entry->d_name;

    if (stat (filename, &statbuf) < 0)
        return 0;

    /* Exclude entries that are not directories */
    if ((entry->d_type != DT_DIR) && !S_ISDIR (statbuf.st_mode))
        return 0;

    /* Exclude current and parent directory */
    return (strcmp (filename, ".") && strcmp (filename, ".."));
}

static int file_filter (const struct dirent *entry)
{
    struct stat statbuf;

    if (stat (entry->d_name, &statbuf) < 0)
        return 0;

    /* Exclude entries that are not regular files */
    return ((entry->d_type == DT_REG) || S_ISREG (statbuf.st_mode));
}

static void _find_files (list_t *list, const char *dirname)
{
    char *path;
    int i, num_files, num_dirs;
    struct dirent **dirlist, **filelist;

    /* Scan directory 'dirname' for files and directories.
     * File entries are placed in the arrary 'filelist', and
     * directory entries are placed in the arrary 'dirlist'.
     *
     * Note: both arrays are allocated by scandir, and must be
     * destroyed afterwards.
     */
    num_dirs = scandir (dirname, &dirlist, dir_filter, alphasort);
    num_files = scandir (dirname, &filelist, file_filter, alphasort);

    /* Loop through file entries and add them to the list */
    for (i = 0; i < num_files; i++)
    {
        path = concatenate_strings (3, dirname + 1, "/", filelist[i]->d_name);
        list_addlast (list, path);

        free (filelist[i]);
    }

    free (filelist);

    /* Loop through directories, and add all contained files recursively. */
    for (i = 0; i < num_dirs; i++)
    {
        path = concatenate_strings (3, dirname, "/", dirlist[i]->d_name);
        _find_files (list, path);

        free (dirlist[i]);
    }

    free (dirlist);
}

list_t *find_files (const char *root_dir)
{
    char cwd[512];
    list_t *files = NULL;

    /* Get path to current directory, and change it */
    if (getcwd(cwd, 512) == NULL)
        ERROR_PRINT("Unable to determine current working directory\n");
    chdir (root_dir);

    files = list_create ((cmpfunc_t) strcmp);
    if (!files)
        goto end;

    _find_files (files, ".");

end:
    /* Restore current directory */
    chdir (cwd);
    return files;
}

int compare_strings(void *a, void *b)
{
    return strcmp(a, b);
}

unsigned long hash_string(void *str)
{
    unsigned char *p = str;
    unsigned long hash = 5381;
    int c;

    while ((c = *p++) != 0)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

int compare_pointers(void *a, void *b)
{
    if (a < b)
        return -1;
    if (a > b)
        return 1;
    return 0;	
}

int is_valid_directory (const char *dirpath)
{
    struct stat s;

    /* Try to get access to 'dirpath' */
    if (access (dirpath, F_OK) < 0)
    {
        DEBUG_PRINT("Could not open directory '%s'.\n", dirpath);
        return 0;
    }

    /* Try to get information about 'dirpath' */
    if (stat (dirpath, &s) < 0)
    {
        DEBUG_PRINT("Could not stat directory '%s'.\n", dirpath);
        return 0;
    }

    /* Check if 'dirpath' is a directory */
    if (!S_ISDIR (s.st_mode))
    {
        DEBUG_PRINT("'%s' is not a directory.\n", dirpath);
        return 0;
    }

    return 1;
}

int is_valid_file (const char *filepath)
{
    struct stat s;

    /* Try to get access to 'filepath' */
    if (access (filepath, F_OK) < 0)
        return 0;

    /* Try to get information about 'filepath' */
    if (stat (filepath, &s) < 0)
        return 0;

    /* Check if 'filepath' is a regular file */
    if (!S_ISREG (s.st_mode))
    {
        DEBUG_PRINT("'%s' is not a regular file.\n", filepath);
        return 0;
    }

    return 1;
}

unsigned long long gettime()
{
    // Get the time as a timeval struct
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1)
    {
        fprintf(stderr, "Could not get time\n");
        return -1;
    }

    // Convert the seconds to microseconds and add in the microsecond part of the time
    unsigned long long micros = 1000000 * tv.tv_sec + tv.tv_usec;

    return micros;
}