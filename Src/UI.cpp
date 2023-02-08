#include <curses.h> 
#include <dirent.h>
#include <sstream>
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

//Should be part of taskManagement
template <typename F>
void forEachNodeDo(std::shared_ptr<Task> curTask, F&& func, int d){
        func(curTask, d);
        for (std::vector<std::shared_ptr<Task>>::iterator it = curTask->subTasks.begin(); it != (curTask->subTasks).end(); it++){
                forEachNodeDo(*it, func, d+1);
        }
}

//########################################[COMP_DOMAIN METHODS]##############################

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

//########################################[EXTERNAL DATA]##############################
extern int focused, currentProjs, curentProjIndx;
extern std::vector<std::string> projectNames;
extern std::vector<std::string> projectFileNames;
extern std::vector<std::shared_ptr<Task>> taskRoots;

//########################################[INTERNAL DATA]##############################
std::vector<UI_Comp<Task>> taskComps;
static size_t currUiCompIndx = 0;

//########################################[STATIC FUNCTION PROTOTYPES]##############################
void drawTaskComp(UI_Comp<Task> comp);
void createUiComps();
void loadTodo(std::string projecFileName);
void displayTodo();
void renameTask(UI_Comp<Task> &comp);
void createNewTask(UI_Comp<Task> &comp, bool newRoot);
void deleteTask(UI_Comp<Task> &comp);

//########################################[INTERFACE FUNCTION DEFINITIONS]##############################

void initCurses(){
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        clear();
}

//Should not be here
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
        currentProjs = 0;
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

int parseCommandRight(int input_ch){
        int running = 1;
        int curx, cury;
        getyx(stdscr, cury, curx); //It's a macro pointers not needed
        switch (input_ch){
                //move up
                case 'k':
                        //Ugly should probably be put into a helper function
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
                //move down
                case 'j':
                        //Ugly should probably be put into a helper function
                        if ( currUiCompIndx < taskComps.size()-1){
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
                //set a task and all of its subtasks to complete
                case 't':{
                        auto func = [&](std::shared_ptr<Task> cur, int d){
                                cur->Completed = true;
                                (void) d;
                        };
                        forEachNodeDo(taskComps[currUiCompIndx].obj, func, 0);
                        move(0,0);
                        displayProjects();
                        move(0, LEFT_RIGHT_BORDER);
                        vline(ACS_VLINE, LINES);
                        move(cury, curx);
                        displayTodo();
                        break;
                }
                //Set a task to incomplete
                case 'n': 
                        taskComps[currUiCompIndx].obj->Completed = false;
                        move(0,0);
                        displayProjects();
                        move(0, LEFT_RIGHT_BORDER);
                        vline(ACS_VLINE, LINES);
                        move(cury, curx);
                        displayTodo();
                        break;
                case 's':
                        saveProj(projectFileNames[curentProjIndx],projectNames[curentProjIndx]);
                        break;
                case 'r':
                        renameTask(taskComps[currUiCompIndx]);
                        break;
                case 'a':
                        createNewTask(taskComps[currUiCompIndx], false);
                        break;
                case 'A':
                        createNewTask(taskComps[currUiCompIndx], true);
                        break;
                case 'd':
                        deleteTask(taskComps[currUiCompIndx]);
                        break;
                case KEY_F(1):
                        running = 0;
                        break;
                //Switch focus to select projects
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
                        highlightProj(0, LEFT_RIGHT_BORDER - 1, cury, cury, COLOR_WHITE, A_NORMAL);
                        cury--;
                        break;
                case 'j':
                        highlightProj(0, LEFT_RIGHT_BORDER - 1, cury, cury, COLOR_WHITE, A_NORMAL);
                        cury++;
                        break;
                case KEY_F(1):
                        running = 0;
                        break;
                case KEY_RIGHT:
                        highlightProj(0, LEFT_RIGHT_BORDER - 1, cury, cury, COLOR_WHITE, A_NORMAL);
                        curx = LEFT_RIGHT_BORDER + 1;
                        cury = 1;
                        focused = RIGHT;
                        break;
                case 'o':
                        clear();
                        move(0,0);
                        displayProjects();
                        move(0, LEFT_RIGHT_BORDER);
                        vline(ACS_VLINE, LINES);
                        move(cury, curx);
                        curentProjIndx = cury;
                        loadTodo(projectFileNames[cury]); //Index out of range danger
                        displayTodo();
                        focused = RIGHT;
                        highlightProj(0, LEFT_RIGHT_BORDER - 1, cury, cury, COLOR_WHITE, A_NORMAL);
                        cury = 0;
                        break;
		case 'n'://create a new project
			
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

//########################################[STATIC FUNCTION DEFINITIONS]##############################

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

void createUiComps(){
        int i = 0;
        taskComps.clear();
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

void loadTodo(std::string projecFileName){
        taskRoots.clear();
        taskRoots = parseFile(projecFileName);
        createUiComps();
}

void displayTodo(){
        for (auto it = taskComps.begin(); it != taskComps.end(); it++){
                drawTaskComp(*it);
        }
        refresh();
}

void renameTask(UI_Comp<Task> &comp){
        std::stringstream s;
        //save current pos
        int cury, curx; 
        getyx(stdscr, cury, curx);
        //move to beginning of the text
        move(comp.domain.y.first, comp.domain.x.first);
        curs_set(2);
        clrtoeol();
        move(comp.domain.y.first, comp.domain.x.first);
        int input_ch;
	//should all be a function for itself
        bool running = true;
        while (running){
                input_ch = getch();
                switch (input_ch) {
                        //Accept the rename, ENTER
                        case 10:
                                comp.obj->desc = s.str();
                                running = false;
                                break;
                        //Discard the rename, ESC
                        case 27:
                                running = false;
                                break;
                        case KEY_BACKSPACE:
                                int cury, curx;
                                getyx(stdscr, cury, curx);
                                move(cury, curx-1);
                                clrtoeol();
                                s.seekp(-1, s.cur);
                                break;
                        default:
                                if ( (input_ch >= 'a' && input_ch <= 'z') || 
                                                (input_ch >= 'A' && input_ch <= 'Z') ||
                                                (input_ch >= '0' && input_ch <= '9') ||
                                                input_ch == ' '
                                                ){
                                        s << (char)input_ch;
                                        addch(input_ch);
                                }
                                break;
                }
        }
        move(comp.domain.y.first, comp.domain.x.first);
        addstr(comp.obj->desc.c_str());
        addch(' ');
        addch(comp.obj->Completed? 'T':'F');
        curs_set(0);
        move(cury, curx);
        refresh();
}

void createNewTask(UI_Comp<Task> &comp, bool newRoot){
        if (newRoot){
                taskRoots.push_back(std::shared_ptr<Task>(new Task("", false)));
        }
        else{
                comp.obj->subTasks.push_back(std::shared_ptr<Task>(new Task("", false))); 
        }
        createUiComps();
        clear();
        move(0, 0);
        displayProjects();
        move(0, LEFT_RIGHT_BORDER);
        vline(ACS_VLINE, LINES);
        displayTodo();
        for (auto  it = taskComps.begin(); it != taskComps.end(); it++)
                if (it->obj->desc == ""){
                        renameTask(*it);
                        break;
                }
}

void deleteTask(UI_Comp<Task> &comp){
        auto func = [&,comp](std::shared_ptr<Task> curTask, int d){
                for (auto subTask = curTask->subTasks.begin(); subTask != curTask->subTasks.end();
                                subTask++)
                        if ((*subTask)->desc == comp.obj->desc){
                                curTask->subTasks.erase(subTask);
                                break;
                        }
        };
        for (auto it = taskRoots.begin(); it != taskRoots.end(); it++){
                if ( (*it)->desc ==comp.obj->desc){
                        taskRoots.erase(it);
                        break;
                }
                else{
                        forEachNodeDo(*it, func, 0);
                }
        }
        for (auto it = taskComps.begin(); it != taskComps.end(); it++){
                if ( it->obj == comp.obj){
                        taskComps.erase(it);
                        break;
                }
        }
        createUiComps();
        clear();
        move(0, 0);
        displayProjects();
        move(0, LEFT_RIGHT_BORDER);
        vline(ACS_VLINE, LINES);
        displayTodo();
}

