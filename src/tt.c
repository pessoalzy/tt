#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

#define MAX_LINES 255
#define MAX_LINE_LENGTH 81

/*
 * TODO
 * manually build a string one char at a time, checking along the way as
 *      we are currently doing.
 * strcmp input string and string being typed and hang until they are the same.
 * record the time to input each string. check number of words by stepping thru
 *      each string manually.
 * show stats on screen while typing.
 */

int main(int argc, char *argv[])
{
        char buf[MAX_LINES][MAX_LINE_LENGTH];
        char line_to_check[MAX_LINE_LENGTH];
        int lc, bad, good, space;
        FILE *fp;
        float total_words, total_minutes;

        time_t start_time, end_time;


        fp=fopen(argv[1], "r");

        lc = 0;
        while (fgets(buf[lc], MAX_LINE_LENGTH, fp) && lc < MAX_LINES - 1) {
                buf[lc][strlen(buf[lc]) - 1] = '\0';
                lc++;
        }
        fclose(fp);

        initscr();
	raw();
	keypad(stdscr, TRUE);
        noecho();
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);

        /* get the current time */
        time(&start_time);

        move(3,73);
        printw("--- WPM\n");
        move(0,0);

        bad = 0;
        good = 0;
        space = 0;
        for(int i = 0; i < lc; i++) {
                printw("%s\n", buf[i]);
                int ch;

                /* until the line to check matches the buf lets keep going */
                int j = 0;
                while (strcmp(line_to_check, buf[i])) {
                        
                        /* get the input char */
                        ch = getch();

                        /* quit if F4 is pressed */
                        if (ch == KEY_F(4))
                                goto end;
                        /* if backspace is pressed, and we're not at the start of the line */
                        else if (ch == KEY_BACKSPACE && j > 0) {
                                j--;
                                move(1, j);
                                addch(' ');
                                move(1, j);
                                time(&end_time);
                                line_to_check[j] = '\0';
                        }
                        else if (ch == buf[i][j]) {
                                addch(ch);
                                line_to_check[j] = ch;
                                /* always add null just in case we are at the end of the line */
                                line_to_check[j + 1] = '\0';
                                time(&end_time);
                                j++;
                        }
                        else {
                                attron(COLOR_PAIR(1));
                                addch(ch);
                                line_to_check[j] = ch;
                                line_to_check[j + 1] = '\0';
                                attroff(COLOR_PAIR(1));
                                j++;
                        }
                }

                /* check total number of non space characters in the line */
                for (int k = 0; k < (int)strlen(line_to_check); k++) {
                        if(line_to_check[k] != ' ')
                                good++;
                }

                time(&end_time);
                move(0,0);
                for (int j = 0; j <= 80; j++)
                        addch(' ');
                move(1,0);
                for (int j = 0; j <= 80; j++)
                        addch(' ');
                move(3,0);
                for (int j = 0; j <= 80; j++)
                        addch(' ');

                total_words = (good / 5.0);
                total_minutes = ((end_time - start_time) / 60.0);

                move(3,73);
                printw("%3.0f WPM\n", (total_words / total_minutes));

                move(0,0);
        }

end:
        endwin();
        return 0;
}
