#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

#define MAX_LINES 255
#define MAX_LINE_LENGTH 81

/*
 * TODO load each line of the document in as a char array, and get user input
 * one key at a time, then step along the char array, check if the char is
 * correct, and color it accordingly. After hitting return, keep score.
 */




int main(int argc, char *argv[])
{
        char buf[MAX_LINES][MAX_LINE_LENGTH];
        int lc, bad, good;
        FILE *fp;
        char *test;
        test = (char *) malloc(81);

        fp=fopen(argv[1], "r");

        lc = 0;
        while (fgets(buf[lc], MAX_LINE_LENGTH, fp) && lc < MAX_LINES - 1) {
                buf[lc][strlen(buf[lc]) - 1] = '\0';
                lc++;
        }
        fclose(fp);

        initscr(); 			/* Start curses mode 		*/
	raw(); 				/* Line buffering disabled	*/
	keypad(stdscr, TRUE); 		/* We get F1, F2 etc..		*/
        noecho();
        start_color();                  /* Start color                  */
        init_pair(1, COLOR_RED, COLOR_BLACK);

        bad = 0;
        good = 0;
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
                        }
                        else if (ch == buf[i][j]) {
                                addch(ch);
                                good++;
                        }
                        else {
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
                move(0,0);
        }

end:
        endwin();
        printf("%d mistakes, %d total characters\n", bad, good - bad);
        return 0;
}
