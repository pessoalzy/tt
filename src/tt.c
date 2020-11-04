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
        int lc, bad, good, space;
        FILE *fp;
        char *test;
        float total_words, total_minutes;

        time_t start_time, end_time;

        test = (char *) malloc(81);

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
                for (int j = 0; j < (int)strlen(buf[i]); j++) {
                       ch = getch();
                       if (ch == KEY_F(4))
                               goto end;
                       else if (ch == KEY_BACKSPACE) {
                                j = j - 2;
                                move(1,j + 1);
                                addch(' ');
                                move(1,j + 1);
                                time(&end_time);
                        }
                        else if (ch == buf[i][j]) {
                                addch(ch);
                                time(&end_time);
                                if (ch != ' ')
                                        good++;
                                else
                                        space++;
                        }
                        else {
                                time(&end_time);
                                attron(COLOR_PAIR(1));
                                addch(ch);
                                attroff(COLOR_PAIR(1));
                                bad++;
                        }
                }
                move(0,0);
                for (int j = 0; j <= 80; j++)
                        addch(' ');
                move(1,0);
                for (int j = 0; j <= 80; j++)
                        addch(' ');
                move(3,0);
                for (int j = 0; j <= 80; j++)
                        addch(' ');

                total_words = ((good - bad) / 5.0);
                total_minutes = ((end_time - start_time) / 60.0);

                move(3,73);
                printw("%3.0f WPM\n", (total_words / total_minutes));

                move(0,0);
        }

end:
        endwin();
        /* printf("%d mistakes, %d total characters, %d total (normalized, 5 character) words, %d total spaces\n", bad, good - bad, (good - bad)/5, space); */
        /* printf("%d seconds spent typing \n", end_time - start_time); */
        printf("%f total words, %f total minutes\n", total_words, total_minutes);
        printf("%f WPM\n", (total_words / total_minutes));
        return 0;
}