#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

#define MAX_LINES 255
#define MAX_LINE_LENGTH 80

int main(int argc, char *argv[])
{
        char buf[MAX_LINES][MAX_LINE_LENGTH + 1];
        char input_string[MAX_LINE_LENGTH + 1];
        int buf_lines, typed_char_count;
        float typed_word_count, total_minutes;
        time_t start_time, end_time;


        /* TODO check for non ascii characters, or non-ascii encoded files */
        /* open the file, load its lines into an array of strings, close file */
        {
                FILE *fp;
                fp=fopen(argv[1], "r");
                if (fp == NULL) {
                        printf("Bad or missing file\n");
                        return 1;
                }
                buf_lines = 0;
                while (fgets(buf[buf_lines], MAX_LINE_LENGTH + 1, fp) &&
                                buf_lines < MAX_LINES - 1) {
                        buf[buf_lines][strlen(buf[buf_lines]) - 1] = '\0';
                        buf_lines++;
                }
                fclose(fp);
        }

        /* start ncurses screen */
        initscr();
	raw();
	keypad(stdscr, TRUE);
        noecho();
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);

        /* add some initial information */
        move(5, 0);
        printw("To quit press F4 at any time...");
        move(3, MAX_LINE_LENGTH - 7);
        printw("--- WPM\n");
        move(0, 0);

        /* get the start time */
        time(&start_time);

        typed_char_count = 0;
        /* loop this for every line that has been loaded into the buffer */
        for(int buf_index = 0; buf_index < buf_lines; buf_index++) {
                int input_index;
                /* print the current line from the file */
                printw("%s\n", buf[buf_index]);

                input_index = 0;
                /* until the input string matches the buffer string, do not
                 * move on to the next one */
                while (strcmp(input_string, buf[buf_index])) {
                        int ch;
                        
                        /* get the input char */
                        ch = getch();

                        /* quit if F4 is pressed */
                        if (ch == KEY_F(4)) {
                                endwin();
                                return 0;
                        }
                        /* if backspace is pressed,
                         * and we're not at the start of the line */
                        else if (ch == KEY_BACKSPACE && input_index > 0) {
                                input_index--;
                                move(1, input_index);
                                addch(' ');
                                move(1, input_index);
                                time(&end_time);
                                input_string[input_index] = '\0';
                        }
                        /* if the key isnt F4 or BACKSPACE, and its outside of
                         * this range, then it is an invalid key */
                        else if ((ch < 32) || (ch > 126))
                                ;
                        /* input character is correct */
                        else if (ch == buf[buf_index][input_index]) {
                                attron(COLOR_PAIR(2));
                                addch(ch);
                                input_string[input_index] = ch;
                                /* always add null after the current position
                                 * just in case we are at the end of the line */
                                input_string[input_index + 1] = '\0';
                                time(&end_time);
                                input_index++;
                                attroff(COLOR_PAIR(2));
                        }
                        /* input character is incorrect */
                        else {
                                attron(COLOR_PAIR(1));
                                /* put a red period in places where space is
                                 * incorrectly placed, so that it is clear
                                 * that it is an error */
                                if (ch == ' ')
                                        addch('_');
                                else
                                        addch(ch);

                                input_string[input_index] = ch;
                                input_string[input_index + 1] = '\0';
                                attroff(COLOR_PAIR(1));
                                input_index++;
                        }
                }

                /* check total number of non-space characters in the line */
                for (int i = 0; i < (int)strlen(input_string); i++) {
                        if(input_string[i] != ' ')
                                typed_char_count++;
                }

                /* check the end time after each line */
                time(&end_time);
                /* clear the screen for the next line */
                for (int x = 0; x <= MAX_LINE_LENGTH ; x++) {
                        for (int y = 0; y <= 3; y ++) {
                                move(y, x);
                                addch(' ');
                        }
                }

                /* calculate total words typed, and ellapsed time since starting
                 * the program */
                typed_word_count = (typed_char_count / 5.0);
                total_minutes = ((end_time - start_time) / 60.0);

                /* print the cumulative WPM since starting the program */
                move(3, MAX_LINE_LENGTH - 7);
                printw("%3.0f WPM\n", (typed_word_count / total_minutes));

                /* reset position for next string */
                move(0, 0);
        }
}
