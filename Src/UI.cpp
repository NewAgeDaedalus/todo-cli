#include <curses.h> 
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#ifndef UI
#define UI
#include "UI.h"
#endif

#ifndef TASK_MANAGEMENT
#define TASK_MANAGEMENT
#include "taskManagement.h"
#endif

comp_domain::comp_domain(int x_from, int x_to, int y_from, int y_to){
        x.first = x_from;
        x.second = x_to;
        y.first = y_from;
        y.second = y_to;
}
comp_domain::comp_domain(){
        x.first = -1;
        x.second = -1;
        y.first = -1;
        y.second = -1;
}

template <typename T>
UI_Comp<T>::UI_Comp(struct comp_domain Domain, std::shared_ptr<T> Obj){
        domain = Domain;
        obj = Obj;
}

template <typename T>
UI_Comp<T>::UI_Comp(){
        domain = comp_domain();
        obj = nullptr;
}

extern int focused, currentProjs;
extern std::vector<std::string> projectNames;
extern std::vector<std::string> projectFileNames;
std::vector<UI_Comp<Task>> taskComps;

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
                projectFileNames.push_back(nameList[n]->d_name);
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

void highlightProj(int x_from, int x_to, int y_from, int y_to, int color, int mode){
        int before_x, before_y;
        getyx(stdscr, before_y, before_x);
        for (int i = y_from; i <= y_to; i++){
                move(i, x_from);
                chgat(x_to -x_from+1, mode, color, NULL);
        }
        move(before_y, before_x);
        refresh();
}

void drawTaskComp(UI_Comp<Task> comp){
        //Save the current cursor position
        int cury, curx;
        getyx(stdscr, cury, curx);
        //Draw the task
        move(comp.domain.y.first, comp.domain.x.first);
        addstr(comp.obj->desc.c_str());
        addstr(" ");
        if (comp.obj->Completed)
                addstr("T");
        else
                addstr("F");
        //restore cursor position
        move(cury, curx);
}

void loadTodo(std::string projecFileName){
        std::vector<std::shared_ptr<Task>> taskRoots;
        taskRoots = parseFile(projecFileName);
        taskComps.clear();
        //Create UI COMPONENT
        int i = 0;
        //Transverses every tree and creates a UI_Component for every tree node(Task)
        for (auto it = taskRoots.begin(); it != taskRoots.end(); it++){
                std::vector<std::pair<std::shared_ptr<Task>,int>> stack;
                stack.push_back(std::pair<std::shared_ptr<Task>,int>(*it, 0));
                while (!stack.empty()){
                        std::shared_ptr<Task> cur = stack.back().first;
                        int d = stack.back().second;
                        stack.pop_back();
                        struct comp_domain domain(LEFT_RIGHT_BORDER + 5 + d *4, COLS-5, i, i+2);
                        i+=2;
                        taskComps.push_back(UI_Comp<Task>(domain, cur));
                        for (auto it = cur->subTasks.begin(); it != cur->subTasks.end();it++){
                                stack.push_back(std::pair<std::shared_ptr<Task>,int>(*it, d+1));
                        }
                }
        }
}

void displayTodo(){
        for (auto it = taskComps.begin(); it != taskComps.end(); it++){
                drawTaskComp(*it);
        }
        refresh();
}

int parseCommandRight(int input_ch){
        static size_t currUiCompIndx = 0;
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
                        if ( currUiCompIndx > 0 && !taskComps.empty()){
                                UI_Comp<Task> *curComp = &taskComps[currUiCompIndx];
                                highlightProj(curComp->domain.x.first, curComp->domain.x.second,
                                                curComp->domain.y.first, curComp->domain.y.first,
                                                curComp->obj->Completed?COLOR_GREEN:COLOR_RED,
                                                A_NORMAL);
                                curComp = &taskComps[currUiCompIndx-1];
                                highlightProj(curComp->domain.x.first, curComp->domain.x.second,
                                                curComp->domain.y.first, curComp->domain.y.first,
                                                curComp->obj->Completed?COLOR_GREEN:COLOR_RED,
                                                A_STANDOUT);
                                cury = curComp->domain.y.first;
                                currUiCompIndx--;
                        }
                        break;
                case 'j':
                        if ( currUiCompIndx < taskComps.size()){
                                UI_Comp<Task> *curComp = &taskComps[currUiCompIndx];
                                highlightProj(curComp->domain.x.first, curComp->domain.x.second,
                                                curComp->domain.y.first, curComp->domain.y.first,
                                                curComp->obj->Completed?COLOR_GREEN:COLOR_RED,
                                                A_NORMAL);
                                curComp = &taskComps[currUiCompIndx+1];
                                highlightProj(curComp->domain.x.first, curComp->domain.x.second,
                                                curComp->domain.y.first, curComp->domain.y.first, 
                                                curComp->obj->Completed?COLOR_GREEN:COLOR_RED,
                                                A_STANDOUT);
                                cury = curComp->domain.y.first;
                                currUiCompIndx++;
                        }
                        break;
                case KEY_F(1):
                        running = 0;
                        break;
                case KEY_LEFT:
                        curx = 0;
                        if (cury >= currentProjs)
                                cury = currentProjs - 1;
                        highlightProj(0, LEFT_RIGHT_BORDER - 1, cury, cury, COLOR_WHITE, A_STANDOUT);
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
                        highlightProj(0, LEFT_RIGHT_BORDER - 1, cury, cury,COLOR_WHITE, A_NORMAL);
                        cury--;
                        break;
                case 'j':
                        highlightProj(0, LEFT_RIGHT_BORDER - 1, cury, cury,COLOR_WHITE, A_NORMAL);
                        cury++;
                        break;
                case KEY_F(1):
                        running = 0;
                        break;
                case KEY_RIGHT:
                        highlightProj(0, LEFT_RIGHT_BORDER - 1, cury, cury,COLOR_WHITE, A_NORMAL);
                        curx = LEFT_RIGHT_BORDER + 1;
                        cury = 1;
                        focused = RIGHT;
                        break;
                case 'o':
                        loadTodo(projectFileNames[cury]); //Index out of range danger
                        displayTodo();
                        focused = RIGHT;
                        highlightProj(0, LEFT_RIGHT_BORDER - 1, cury, cury,COLOR_WHITE, A_NORMAL);
                        cury = 0;
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
                highlightProj(0, LEFT_RIGHT_BORDER -1, cury, cury,COLOR_WHITE,A_STANDOUT);
        refresh();
        return running;
}
