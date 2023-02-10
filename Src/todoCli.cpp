#include <iostream>
#include <cursesw.h>
#include <memory>
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

#ifndef PROJECT
#define PROJECT
#include "project.h"
#endif

int focused, currentProjs, current_project_index;
std::vector<std::shared_ptr<Project>> projects;

int main(int argc, char *argv[]){
        chdir("Data");
        initCurses();
        loadProjects(projects);
	std::cout<<projects.size()<<"\n";
	generate_project_comps(projects);
        displayProjects();
        move(0, LEFT_RIGHT_BORDER);
        vline(ACS_VLINE, LINES);
        move(0, 0);
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
