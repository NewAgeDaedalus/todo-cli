#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
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

int focused, current_project_index, highlighted_project_index;
std::vector<std::shared_ptr<Project>> projects;

int main(int argc, char *argv[]){
	struct stat st = {0};

	printf("%s\n", getenv("HOME"));
	int retval = chdir(getenv("HOME"));
	if (retval == -1){
		printf("Change to home failed\n");
		exit(1);
	}
	if ( stat(".local/etc/todo-cli/projects", &st) == -1 ){
		if(mkdir(".local/etc/todo-cli", 0700)){
			printf("failed to create dir\n");
			exit(1);
		}
		if (mkdir(".local/etc/todo-cli/projects", 0700)){
			printf("failed to create dir\n");
			exit(1);
		}
	}
        retval = chdir(".local/etc/todo-cli/projects");
	if (retval){
		printf("Change failed\n");
		exit(1);
	}
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
