#include <curses.h> 
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#ifndef UI
#define UI
#include "UI.h"
#endif

extern int focused, currentProjs;

void initCurses(){
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        clear();
}

int loadProject(char *fileName){
        int retVal = 0;
        size_t n = 0;
        char *projName;
        char projNameR[256];
        FILE *fp = fopen(fileName, "r");
        if (fp == NULL){
                return 1;
        }
        getline(&projName, &n, fp);
        move(++currentProjs - 1, 0);
        strncpy(projNameR, projName, n);
        addstr(projNameR);
        move(0, 0);
        refresh();
        fclose(fp);
        free(projName);
        return retVal;
}

int loadProjects(){
        int retVal = 0;
        struct dirent **nameList;
        int n = scandir(".", &nameList, NULL, alphasort);
        if (n == -1){
                return 1;
        }
        while (n--){
                if (nameList[n]->d_name[0] == '.')
                        continue;
                loadProject(nameList[n]->d_name);
                free(nameList[n]);
        }
        free(nameList);
        return retVal;
}

void  highlightProj(int x_from, int x_to, int y_from, int y_to, int mode){
        int before_x, before_y;
        getyx(stdscr, before_y, before_x);
        for (int i = y_from; i <= y_to; i++){
                move(i, x_from);
                chgat(x_to -x_from+1, mode, COLOR_WHITE, NULL);
        }
        move(before_y, before_x);
}


int parseCommandRight(int input_ch){
        int running = 1;
        int curx, cury;
        getyx(stdscr, cury, curx); //It's a macro pointers not needed
        switch (input_ch){
                case 'h':
                        curx--;
                        break;
                case 'l':
                        curx++;
                        break;
                case 'k':
                        cury--;
                        break;
                case 'j':
                        cury++;
                        break;
                case KEY_F(1):
                        running = 0;
                        break;
                case KEY_LEFT:
                        curx = 0;
                        focused = LEFT;
                        break;
                default:
                        break;
        }
        if (curx <= COLS/4 && focused != LEFT)
                curx = COLS/4 + 1;
        move(cury, curx);
        refresh();
        return running;
}

int parseCommandLeft(int input_ch){
        int running = 1;
        int curx, cury;
        getyx(stdscr, cury, curx); //It's a macro pointers not needed
        switch (input_ch){
                case 'h':
                        curx--;
                        break;
                case 'l':
                        curx++;
                        break;
                case 'k':
                        highlightProj(0, COLS/4 - 1, cury, cury, A_NORMAL);
                        cury--;
                        break;
                case 'j':
                        highlightProj(0, COLS/4 - 1, cury, cury, A_NORMAL);
                        cury++;
                        break;
                case KEY_F(1):
                        running = 0;
                        break;
                case KEY_RIGHT:
                        highlightProj(0, COLS/4 - 1, cury, cury, A_NORMAL);
                        curx = COLS/4 + 1;
                        focused = RIGHT;
                        break;
                default:
                        break;
        }
        if (curx >= COLS/4 && focused != RIGHT)
                curx = COLS/4 - 1;
        move(cury, curx);
        highlightProj(0, COLS/4 -1, cury, cury, A_STANDOUT);
        refresh();
        return running;
}
