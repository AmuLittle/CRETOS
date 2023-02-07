#include <ncurses.h>

int main() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    notimeout(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    char ch = -1;
    while (ch != 27) {
        printw("Press a Key to get it's key code: ");
        ch = getch();
        printw("\n%d\n", (int)ch);
    }
    endwin();
}