#include <curses.h> 
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>

#ifndef UI
#define UI
#include "UI.h"
#endif

extern int focused, currentProjs;
extern std::vector<std::string> projectNames;

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
        FILE *fp = fopen(fileName, "r");
        if (fp == NULL){
                return 1;
        }
        getline(&projName, &n, fp);
        projectNames.push_back(projName);
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

void displayProject(const char *projName){
        move(++currentProjs - 1, 0);
        addstr(projName);
        move(0, 0);
        refresh();
}

void displayProjects(){
        for (auto name =  projectNames.begin(); name != projectNames.end(); name++){
                displayProject(name->c_str());
        }
}

void highlightProj(int x_from, int x_to, int y_from, int y_to, int mode){
        int before_x, before_y;
        getyx(stdscr, before_y, before_x);
        for (int i = y_from; i <= y_to; i++){
                move(i, x_from);
                chgat(x_to -x_from+1, mode, COLOR_WHITE, NULL);
        }
        move(before_y, before_x);
        refresh();
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
                        if (cury >= currentProjs)
                                cury = currentProjs - 1;
                        highlightProj(0, LEFT_RIGHT_BORDER - 1, cury, cury, A_STANDOUT);
                        curs_set(0);
                        focused = LEFT;
                        break;
                default:
                        break;
        }
        if (curx <= LEFT_RIGHT_BORDER && focused != LEFT)
                curx = LEFT_RIGHT_BORDER + 1;
        move(cury, curx);
        refresh();
        return running;
}

int parseCommandLeft(int input_ch){
        int running = 1;
        int curx, cury;
        getyx(stdscr, cury, curx); //It's a macro pointers not needed
        switch (input_ch){
                case 'k':
                        highlightProj(0, LEFT_RIGHT_BORDER - 1, cury, cury, A_NORMAL);
                        cury--;
                        break;
                case 'j':
                        highlightProj(0, LEFT_RIGHT_BORDER - 1, cury, cury, A_NORMAL);
                        cury++;
                        break;
                case KEY_F(1):
                        running = 0;
                        break;
                case KEY_RIGHT:
                        highlightProj(0, LEFT_RIGHT_BORDER - 1, cury, cury, A_NORMAL);
                        curx = LEFT_RIGHT_BORDER + 1;
                        focused = RIGHT;
                        curs_set(2);
                        break;
                default:
                        break;
        }
        if (curx >= LEFT_RIGHT_BORDER && focused != RIGHT)
                curx = LEFT_RIGHT_BORDER - 1;
        if (cury >= currentProjs)
                cury = currentProjs-1;
        move(cury, curx);
        if (focused != RIGHT)
                highlightProj(0, LEFT_RIGHT_BORDER -1, cury, cury, A_STANDOUT);
        refresh();
        return running;
}