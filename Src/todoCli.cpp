#include <iostream>
#include <cursesw.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <vector>

#ifndef UI
#define UI
#include "UI.h"
#endif

#ifndef TASK_MANAGEMENT
#define TASK_MANAGEMENT
#include "taskManagement.h"
#endif



int focused, currentProjs;
std::vector<std::string> projectNames;

int main(int argc, char *argv[]){
        projectNames = *new std::vector<std::string>();
        chdir("Data");
        initCurses();
        loadProjects();
        displayProjects();
        move(0, LEFT_RIGHT_BORDER);
        vline(ACS_VLINE, LINES);
        move(0, 0);
        highlightProj(0, LEFT_RIGHT_BORDER - 1, 0, 0, A_STANDOUT);
        curs_set(0);
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
