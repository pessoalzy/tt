/* see LICENSE file for copyright and license details. */

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
        int buf_lines, total_char_count, bad_char_count;
        float total_word_count, total_minutes;
        time_t start_time, end_time;

        /*--------------------------------------------------------------------*/
        /* open the file, load its lines into an array of strings, close file */
        /*--------------------------------------------------------------------*/
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

        /*--------------------------------------------------------------------*/
        /* initialize ncurses screen and interface                            */
        /*--------------------------------------------------------------------*/

        initscr();
	raw();
	keypad(stdscr, TRUE);
        noecho();

        {
                int x, y;
                getmaxyx(stdscr, y, x);
                if (y < 24 || x < 80) {
                        endwin();
                        printf("Terminal too small, 80x24 required\n");
                        return 1;
                }
        }

	if (has_colors() == FALSE)
	{	endwin();
		printf("Your terminal does not support color\n");
		return 1;
	}

        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);

        /* add some initial information */
        mvprintw(6, 0, "To quit press F4 at any time...");
        mvprintw(3, MAX_LINE_LENGTH - 13, "LAST: --- WPM\n");
        mvprintw(4, MAX_LINE_LENGTH - 14, "TOTAL: --- WPM\n");

        /*--------------------------------------------------------------------*/
        /* main loop                                                          */
        /*--------------------------------------------------------------------*/

        total_char_count = 0;
        bad_char_count = 0;
        /* loop this for every line that has been loaded into the buffer */
        for(int buf_index = 0; buf_index < buf_lines; buf_index++) {
                time_t line_start_time;
                int line_char_count, input_index;
                float line_word_count, line_minutes;

                /* print the current line from the file */
                mvprintw(0, 0, "%s\n", buf[buf_index]);

                /*------------------------------------------------------------*/
                /* input loop                                                 */
                /*------------------------------------------------------------*/

                input_index = 0;
                line_char_count = 0;
                /* until the input string matches the buffer string, do not
                 * move on to the next one */
                while (strcmp(input_string, buf[buf_index])) {
                        int ch;
                        
                        /* get the input char */
                        ch = getch();

                        /* get line/total start times when first key is
                         * pressed */
                        if (buf_index == 0 && input_index == 0)
                                time(&start_time);
                        if (input_index == 0)
                                time(&line_start_time);

                        /* quit if F4 is pressed */
                        if (ch == KEY_F(4)) {
                                endwin();
                                if (total_word_count && total_minutes) {
                                        printf("     WPM: %.0f\n", (total_word_count / total_minutes));
                                        printf("Accuracy: %.0f%%\n", ((((float)total_char_count/(total_char_count+bad_char_count)))*100));
                                }
                                return 0;
                        }
                        /* if backspace is pressed, * and we're not at the start
                         * of the line.
                         * ch == 8 accounts for backspace being pressed while
                         * caps lock is on */
                        else if (((ch == KEY_BACKSPACE) || (ch == 8) || (ch == 127) || (ch == 263)) &&
                                        (input_index > 0)) {
                                input_index--;
                                mvaddch(1, input_index, ' ');
                                move(1, input_index);
                                time(&end_time);
                                input_string[input_index] = '\0';
                        }
                        /* if the key isnt F4 or BACKSPACE, and its outside of
                         * this range, then it is an invalid key */
                        else if ((ch < 32) || (ch > 126))
                                ;
                        /* input character is correct, and there is room in the
                         * input buffer */
                        else if (ch == buf[buf_index][input_index] &&
                                        (input_index < MAX_LINE_LENGTH)) {
                                attron(COLOR_PAIR(2));
                                addch(ch);
                                input_string[input_index] = ch;
                                /* always add null after the current position
                                 * just in case we are at the end of the line */
                                input_string[input_index + 1] = '\0';
                                time(&end_time);
                                attroff(COLOR_PAIR(2));
                                input_index++;
                        }
                        /* input character is incorrect and there is room in the
                         * input buffer */
                        else if (input_index < MAX_LINE_LENGTH) {
                                attron(COLOR_PAIR(1));
                                /* put a red underscore in places where space is
                                 * incorrectly placed, so that it is clear that
                                 * it is an error */
                                if (ch == ' ')
                                        addch('_');
                                else
                                        addch(ch);

                                input_string[input_index] = ch;
                                input_string[input_index + 1] = '\0';
                                attroff(COLOR_PAIR(1));
                                bad_char_count++;
                                input_index++;
                        }
                }

                /*------------------------------------------------------------*/
                /* get WPM data for the completed lines, and print it         */
                /*------------------------------------------------------------*/

                /* check total number of non-space characters in the line */
                for (int i = 0; i < (int)strlen(input_string); i++) {
                        if(input_string[i] != ' ') {
                                line_char_count++;
                                total_char_count++;
                        }
                }

                /* reset the end time after each line */
                time(&end_time);
                /* clear the screen for the next line */
                for (int x = 0; x <= MAX_LINE_LENGTH ; x++) {
                        for (int y = 0; y <= 4; y ++) {
                                mvaddch(y, x, ' ');
                        }
                }

                /* final calculations */
                total_word_count = (total_char_count / 5.0);
                total_minutes = ((end_time - start_time) / 60.0);

                line_word_count = (line_char_count / 5.0);
                line_minutes = ((end_time - line_start_time) / 60.0);

                mvprintw(3, MAX_LINE_LENGTH - 13, "LAST: %3.0f WPM\n", (line_word_count / line_minutes));
                mvprintw(4, MAX_LINE_LENGTH - 14, "TOTAL: %3.0f WPM\n", (total_word_count / total_minutes));
        }

        /* end ncurses screen, and exit if all lines in file have been typed */
        endwin();
        printf("     WPM: %.0f\n", (total_word_count / total_minutes));
        printf("Accuracy: %.0f%%\n", ((((float)total_char_count/(total_char_count+bad_char_count)))*100));
        return 0;
}
