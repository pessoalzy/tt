#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

/*
 * TODO load each line of the document in as a char array, and get user input
 * one key at a time, then step along the char array, check if the char is
 * correct, and color it accordingly. After hitting return, keep score.
 */




int main(int argc, char *argv[])
{
        char *test;
        test = (char *) malloc(81);
        int lines;

        initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
        start_color();                  /* Start color                  */

        char s;
        FILE *fp;
        fp=fopen(argv[1], "r");

        lines = 0;
        while ((s=getc(fp)) != EOF) {
                if (s == '\n')
                        lines++;
        }
        fclose(fp);
 

        fp=fopen(argv[1], "r");
        int i = 0;
        while (i < lines) {
                int yes = 1;
                while ((s=getc(fp)) != '\n') {
                        if (isascii(s)) {
                                addch(s);
                                yes = 1;
                        }
                        else if (yes) {
                                addch('-');
                                yes = 0;
                        }
                }
                move(1,0);
                getstr(test);

                move(0,0);
                for (int i = 0; i < 80; i++)
                        addch(' ');

                move(1,0);
                for (int i = 0; i < 80; i++)
                        addch(' ');

                move(0,0);
                i++;
                if (strcmp(test, ":q") == 0)
                        break;
        }

        fclose(fp);
        endwin();
}
