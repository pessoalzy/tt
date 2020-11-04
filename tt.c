#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

#define MAX_LINES 255
#define MAX_LINE_LENGTH 81

int main(int argc, char *argv[])
{
        char buf[MAX_LINES][MAX_LINE_LENGTH];
        char line_to_check[MAX_LINE_LENGTH];
        int lc, bad, good, space;
        FILE *fp;
        float total_words, total_minutes;

        time_t start_time, end_time;


        /* open the file, load its lines into an array of strings, close file */
        fp=fopen(argv[1], "r");

        lc = 0;
        while (fgets(buf[lc], MAX_LINE_LENGTH, fp) && lc < MAX_LINES - 1) {
                buf[lc][strlen(buf[lc]) - 1] = '\0';
                lc++;
        }
        fclose(fp);

        /* start ncurses screen */
        initscr();
	raw();
	keypad(stdscr, TRUE);
        noecho();
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);

        /* add some initial information */
        move(4,0);
        printw("Press F4 to quit at any time.");
        move(3,73);
        printw("--- WPM\n");
        move(0,0);

        /* get the current time */
        time(&start_time);

        /* TODO remove all extra variables, there are a bunch */
        bad = 0;
        good = 0;
        space = 0;
        /* loop this for every line that has been loaded into the buffer */
        for(int i = 0; i < lc; i++) {
                printw("%s\n", buf[i]);
                int ch;

                int j = 0;
                /* until the input string matches the buffer string, do not
                 * move on to the next one */
                while (strcmp(line_to_check, buf[i])) {
                        
                        /* get the input char */
                        ch = getch();

                        /* quit if F4 is pressed */
                        if (ch == KEY_F(4)) {
                                endwin();
                                return 0;
                        }

                        /* if backspace is pressed,
                         * and we're not at the start of the line */
                        else if (ch == KEY_BACKSPACE && j > 0) {
                                j--;
                                move(1, j);
                                addch(' ');
                                move(1, j);
                                time(&end_time);
                                line_to_check[j] = '\0';
                        }
                        /* input character is correct */
                        else if (ch == buf[i][j]) {
                                addch(ch);
                                line_to_check[j] = ch;
                                /* always add null after the current position
                                 * just in case we are at the end of the line */
                                line_to_check[j + 1] = '\0';
                                time(&end_time);
                                j++;
                        }

                        /* TODO account for special key presses, etc,
                         * perhaps check if char is between 32 and 126
                         * perhaps allow arrow keys but that would be more work.
                         */

                        /* input character is incorrect */
                        else {
                                attron(COLOR_PAIR(1));
                                /* put a red period in places where space is
                                 * incorrectly placed, so that it is clear
                                 * that it is an error */
                                if (ch == ' ')
                                        addch('.');
                                else
                                        addch(ch);

                                line_to_check[j] = ch;
                                line_to_check[j + 1] = '\0';
                                attroff(COLOR_PAIR(1));
                                j++;
                        }
                }

                /* check total number of non-space characters in the line */
                for (int k = 0; k < (int)strlen(line_to_check); k++) {
                        if(line_to_check[k] != ' ')
                                good++;
                }

                /* check the end time after each line */
                time(&end_time);
                /* clear the screen for the next line */
                for (int j = 0; j <= MAX_LINE_LENGTH; j++) {
                        for (int k = 0; k <= 3; k ++) {
                                move(k,j);
                                addch(' ');
                        }
                }

                /* calculate total words typed, and ellapsed time since starting
                 * the program */
                total_words = (good / 5.0);
                total_minutes = ((end_time - start_time) / 60.0);

                /* print the cumulative WPM since starting the program */
                move(3,73);
                printw("%3.0f WPM\n", (total_words / total_minutes));

                /* reset position for next string */
                move(0,0);
        }
}
