#include <iostream>
#include <cursesw.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#ifndef UI
#define UI
#include "UI.h"
#endif

int focused, currentProjs;

int main(int argc, char *argv[]){
        chdir("Data");
        initCurses();
        loadProjects();
        move(0, COLS/4);
        vline(ACS_VLINE, LINES);
        move(0, 0);
        highlightProj(0, COLS/4 -1, 0, 0, A_STANDOUT);
        refresh();
        int running = 1;
        focused = LEFT; 
        while (running){
                int input_ch = getch();
                switch (focused){
                        case RIGHT:
                                running = parseCommandRight(input_ch);
                                break;
                        case LEFT:
                                running = parseCommandLeft(input_ch);
                                break;
                }
        }
        endwin();
        return 0;
}
