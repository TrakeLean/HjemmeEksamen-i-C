#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "ui.h"
#include "trie.h"
#include "index.h"
#include "printing.h"

void PRINT_DEBUG_INFO(int inpos, int spos, int cur_word_len, char *input, char *suggestion)
{
    int x, y;
    int row = getmaxy(stdscr);
    
    getyx(stdscr, y, x);

    move(row-8, 0);
    clear();
    printw("inpos: %d\n", inpos);
    printw("spos: %d\n", spos);
    printw("cur_word_len: %d\n", cur_word_len);
    printw("x: %d\n", x);
    printw("y: %d\n", y);
    printw("buffer: %s\n", input);
    printw("current suggestion: %s\n", suggestion);
    if (suggestion != NULL)
    {
        printw("suggestion len: %d\n", strlen(suggestion));
    }
        

    move(row-1, x);
    refresh();
}

void ui_init()
{
    initscr();
    cbreak();

    keypad(stdscr, TRUE);    
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_BLUE, COLOR_WHITE);

    wbkgd(stdscr, COLOR_PAIR(3));

    noecho();
    refresh();
    return;
}

void ui_display_main_help()
{
    int row = getmaxy(stdscr);
    // Print menu
    move(row-2, 0);
    clrtoeol();

    printw("HOME - Reset buffer\t");
    printw("RIGHT ARROW/TAB - Accept suggestion\t");
}


void ui_display_input(char *input, char *suggestion, int sindex)
{
    int row = getmaxy(stdscr);

    // Print the promt and the current search term
    move(row-1, 0);
    clrtoeol();
    attron(COLOR_PAIR(2));
    printw("[Search Term]:");
    attroff(COLOR_PAIR(2));

    if (input != NULL)
    {
        printw(input);
        if (suggestion != NULL)
        {
            int x = getmaxx(stdscr);
            attron(COLOR_PAIR(1));
            printw((char *)&suggestion[sindex]);
            attroff(COLOR_PAIR(1));

            move(row-1, x);
        }
    }

    refresh();
}

char *ui_main(index_t *idx)
{
    int row, c;
    char *input = (char *)calloc(201, sizeof(char));
    char *suggestion = NULL;

    // inpos is the front of the input buffer
    // spos is the start of the current word
    // cur_word_len is the length of the current word
    int inpos = 0, spos = 0, cur_word_len = 0;

    clear();
    row = getmaxy(stdscr);
    DEBUG_PRINT("%d", row);
    ui_display_main_help(row);
    ui_display_input(NULL, NULL, cur_word_len);

    while ((c = getch()) != '\n')
    {
        // Maximum input size is 200 characters
        if (inpos >= 200)
        {
            ui_display_input("Too long input, only 200 characters allowed.", NULL, cur_word_len);
            input[200] = '\0';
            return input;
        }

        // Handle specific character input
        switch(c)
        {
            // Backspace character, deletes the last key input
            case KEY_BACKSPACE:
            case KEY_DC:
            case 127:
                // Only delete characters if the buffer contains characters
                if (inpos > 0)
                {
                    inpos--;
                    cur_word_len--;

                    // If we encounter a space char, we need to find the start of the previous word
                    if (input[inpos] == ' ')
                    {
                        spos = inpos;
                        
                        // Find the next space after the current word
                        for (;input[spos-1] != ' '; spos--)
                        {
                            // We're on the first word
                            if (spos <= 0)
                            {
                                spos = 0;
                                break;
                            }
                        }
                    }
                    input[inpos] = '\0';
                    cur_word_len = inpos - spos;
                }
                break;

            // Right arrow key puts the suggested word into the buffer
            case KEY_RIGHT:
            case 67:
            case '\t':
                if (suggestion != NULL && cur_word_len >= MIN_SUGGESTION_LEN)
                {
                    for (int i = cur_word_len; i < (int)strlen(suggestion); inpos++, i++)
                    {
                        input[inpos] = suggestion[i];
                    }
                    //inpos += strlen(suggestion) - cur_word_len;
                    c = ' ';
                }
                break;

            // Pressing the home key will reset the buffer
            case KEY_HOME:
                memset(input, '\0', 200);
                inpos = 0;
                spos = 0;
                cur_word_len = 0;
                suggestion = NULL;
                break;
        }

        // Copy the last ASCII key input into the buffer
        if ((c >= 97 && c <= 122))
        {
            // We also nullterminate the next character in the buffer
            input[inpos] = c;
            inpos++;
            input[inpos] = '\0';

            cur_word_len++;
        }
        else if (c == ' ')
        {
            // Only allow a single space character
            if (input[inpos-1] == ' ')
            {
                continue;
            }

            // Space resets the suggestion (since we have a new word)
            // We also need to nullterminate the next character in the buffer.
            input[inpos] = c;
            inpos++;
            input[inpos] = '\0';

            spos = inpos;
            cur_word_len = 0;
            suggestion = NULL;
        }

        if (cur_word_len >= MIN_SUGGESTION_LEN)
        {
            // Null terminate the current input string
            input[inpos] = '\0';
            
            // Get a suggestion from a given dictionary
            suggestion = autocomplete(idx, (char *)&input[spos], cur_word_len);
        }
        else
        {
            suggestion = NULL;
        }

        ui_display_input(input, suggestion, cur_word_len);

    }

    if (inpos > 0)
    {
        input = realloc(input, inpos+1);
        if (input[inpos-1] == ' ')
        {
            input[inpos-1] = '\0';
        }
        input[inpos] = '\0';
    }
    else
    {
        free(input);
    }

    return input;
}


static void ui_display_results_help(int rows, search_hit_t *cur_pos, int curr)
{
    move(rows-1, 0);
    printw("HOME - Go back to search\t");
    printw("ENTER - Next result\t");
    if (cur_pos != NULL)
    {
        printw("CURRENT WORD: %d", cur_pos->word_placement);
        //printw("\tCURRENT WORD: %d", cur_pos->location);
        printw("\t\tWORDS FOUND: %d/%d", curr,cur_pos->words_found);
    }
        
    move(0, 0);

    attron(COLOR_PAIR(2));
    printw("SEARCH RESULTS FOR %s", cur_pos->document_name);
    attroff(COLOR_PAIR(2));
    refresh();
}

static void ui_display_results_content(char **content, int content_length, search_hit_t *cur_pos)
{
    move(1, 0);
    int from = 0;
    int y;
    int maxy = getmaxy(stdscr);

    for (from = cur_pos->location; from > 0; from--)
    {
        if (strcmp(content[from], "\n") == 0)
        {
            break;
        }
    }

    for (int i = from; i < content_length; i++)
    {
        if (i == cur_pos->location)
        {
            attron(COLOR_PAIR(1));
            printw("%s", content[i]);
            attroff(COLOR_PAIR(1));
        }
        else
        {
            printw("%s", content[i]);
        }
        y = getcury(stdscr);
        maxy = getmaxy(stdscr);
        if (y >= maxy-2)
        {
            break;
        }
    }
    refresh();
}


void ui_result(search_result_t *res)
{
    int row, c, curr = 1;
    
    char **content = result_get_content(res);
    int content_length = result_get_content_length(res);
    search_hit_t *cur_pos = result_next(res);

    row = getmaxy(stdscr);
    clear();

    ui_display_results_help(row, cur_pos, curr);

    if (content == NULL || cur_pos == NULL)
    {
        attron(COLOR_PAIR(1));
        printw("\nQuery not found in document\n");
        attroff(COLOR_PAIR(1));
    }
    else
    {
        ui_display_results_content(content, content_length, cur_pos);
    }

    while ((c = getch()) != KEY_HOME)
    {
        switch(c)
        {
            case KEY_ENTER:
            case '\n':
                clear();
                curr ++;
                cur_pos = result_next(res);
                break;
        }

        if (cur_pos == NULL)
        {
            content = result_get_content(res);
            content_length = result_get_content_length(res);
            printw("END OF CURRENT DOCUMENT - PRESS ENTER TO SEARCH NEXT DOCUMENT");
            curr = 0;
            if (content == NULL)
            {
                clear();
                attron(COLOR_PAIR(1));
                printw("END OF RESULTS - PRESS HOME TO GO BACK");
                attroff(COLOR_PAIR(1));
                continue;
            }
        }
        else
        {
            ui_display_results_help(row, cur_pos, curr);
            ui_display_results_content(content, content_length, cur_pos);
        }
        
    }
    //free(res);
    return;
}


void ui_deinit()
{
    endwin();
    return;
}