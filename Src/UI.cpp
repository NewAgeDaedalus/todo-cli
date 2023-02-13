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

#ifndef PROJECT
#define PROJECT
#include "project.h"
#endif

#ifndef UTILS
#define UTILS
#include "utils.h"
#endif

using std::shared_ptr;

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

template<>
void UI_Comp<Task>::draw_content(){
	int cury, curx;
        getyx(stdscr, cury, curx);
        //Draw the task
        move(domain.y.first, domain.x.first);
        addstr(obj->name.c_str());
        addstr(" ");
        if (obj->Completed)
                addstr("T");
        else
                addstr("F");
        //restore cursor position
        move(cury, curx);
}

template<>
void UI_Comp<Project>::draw_content(){
	int cury, curx;
        getyx(stdscr, cury, curx);
        //Draw the task
        move(domain.y.first, domain.x.first);
        addstr(obj->name.c_str());
        addstr(" ");
        //restore cursor position
        move(cury, curx);

}

template <typename T>
void UI_Comp<T>::rename(){
	std::stringstream s;
        //save current pos
        int cury, curx; 
        getyx(stdscr, cury, curx);
        //move to beginning of the text
        move(domain.y.first, domain.x.first);
        curs_set(2);
        clrtoeol();
        move(domain.y.first, domain.x.first);
        int input_ch;
	//should all be a function for itself
        bool running = true;
        while (running){
                input_ch = getch();
                switch (input_ch) {
                        //Accept the rename, ENTER
                        case 10:
                                obj->name = s.str();
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
	move(domain.y.first, domain.x.first);
        addstr(obj->name.c_str());
        curs_set(0);
        move(cury, curx);
        refresh();
}

template<typename T>
void UI_Comp<T>::highlight(int color, int mode){
        int before_x, before_y;
        getyx(stdscr, before_y, before_x);
        for (int i = domain.y.first; i < domain.y.second; i++){
                move(i, domain.x.first);
                chgat(domain.x.second - domain.x.first+1, mode, color, NULL);
        }
        move(before_y, before_x);
        refresh();
}

//########################################[EXTERNAL DATA]##############################
extern int focused, current_project_index, highlighted_project_index;

extern std::vector<std::shared_ptr<Task>> taskRoots;
std::vector<shared_ptr<UI_Comp<Project>>> project_comps;

//########################################[INTERNAL DATA]##############################
std::vector<UI_Comp<Task>> taskComps;
static size_t curr_task_comp_index = 0;

//########################################[STATIC FUNCTION PROTOTYPES]##############################
void drawTaskComp(UI_Comp<Task> comp);
void createUiComps();
void loadTodo(Project &project);
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

void generate_project_comps(std::vector<std::shared_ptr<Project>> projects){
	int i = 0;
	for (auto project:projects){
		struct comp_domain domain = comp_domain(0, COLS/4, i, i+1);
		i++;
		project_comps.push_back(shared_ptr<UI_Comp<Project>>(new UI_Comp<Project>(domain, project)));
	}
}

void displayProjects(){
	for (auto project_comp:project_comps){
		project_comp->draw_content();
	}
}

//Should maybe be in the main file
int parseCommandRight(int input_ch){
        int running = 1;
        int curx, cury;
        getyx(stdscr, cury, curx); //It's a macro pointers not needed
	UI_Comp<Task> *curComp = &taskComps[curr_task_comp_index];
        switch (input_ch){
                //move up
                case 'k':
                        //Ugly should probably be put into a helper function
                        if ( curr_task_comp_index > 0 && !taskComps.empty()){
				curComp->highlight(curComp->obj->Completed ? COLOR_GREEN:COLOR_RED, A_NORMAL);
                                curComp = &taskComps[curr_task_comp_index-1];
				curComp->highlight(curComp->obj->Completed ? COLOR_GREEN:COLOR_RED, A_STANDOUT);
                                cury = curComp->domain.y.first;
                                curr_task_comp_index--;
                        }
                        break;
                //move down
                case 'j':
                        //Ugly should probably be put into a helper function
                        if ( curr_task_comp_index < taskComps.size()-1){
				curComp->highlight(curComp->obj->Completed ? COLOR_GREEN:COLOR_RED, A_NORMAL);
                                curComp = &taskComps[curr_task_comp_index+1];
				curComp->highlight(curComp->obj->Completed ? COLOR_GREEN:COLOR_RED, A_STANDOUT);
                                cury = curComp->domain.y.first;
                                curr_task_comp_index++;
                        }
                        break;
                //set a task and all of its subtasks to complete
                case 't':{
                        auto func = [&](std::shared_ptr<Task> cur, int d){
                                cur->Completed = true;
                                (void) d;
                        };
                        forEachNodeDo(taskComps[curr_task_comp_index].obj, func, 0);
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
                        taskComps[curr_task_comp_index].obj->Completed = false;
                        move(0,0);
                        displayProjects();
                        move(0, LEFT_RIGHT_BORDER);
                        vline(ACS_VLINE, LINES);
                        move(cury, curx);
                        displayTodo();
                        break;
                case 's':
			project_comps[current_project_index]->obj->save_project(taskRoots);
                        break;
                case 'r':
//                         renameTask(taskComps[curr_task_comp_index]);
			curComp->rename();
			break;
                case 'a':
                        createNewTask(taskComps[curr_task_comp_index], false);
                        break;
                case 'A':
                        createNewTask(taskComps[curr_task_comp_index], true);
                        break;
                case 'd':
                        deleteTask(taskComps[curr_task_comp_index]);
                        break;
                case KEY_F(1):
                        running = 0;
                        break;
                //Switch focus to select projects
                case KEY_LEFT:
                        curx = 0;
                        if (cury >= current_project_index)
                                cury = current_project_index - 1;
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
	UI_Comp<Project> &cur_project = *project_comps[current_project_index];
        switch (input_ch){
                case 'k':
			if (highlighted_project_index == 0)
				break;
			project_comps[highlighted_project_index]->highlight(COLOR_WHITE, A_NORMAL);
                        cury--;
			highlighted_project_index--;
			project_comps[highlighted_project_index]->highlight(COLOR_WHITE, A_STANDOUT);
                        break;
                case 'j':
			if (highlighted_project_index == project_comps.size() - 1)
				break;
			project_comps[highlighted_project_index]->highlight(COLOR_WHITE, A_NORMAL);
                        cury++;
			highlighted_project_index++;
			project_comps[highlighted_project_index]->highlight(COLOR_WHITE, A_STANDOUT);
                        break;
                case KEY_F(1):
                        running = 0;
                        break;
                case KEY_RIGHT:
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
                        current_project_index = cury;
                        loadTodo(*cur_project.obj); //Index out of range danger
                        displayTodo();
                        focused = RIGHT;
                        cury = 0;
			curr_task_comp_index = 0;
			current_project_index = highlighted_project_index;
                        break;
                case 'r':
//                         renameTask(taskComps[curr_task_comp_index]);
			project_comps[highlighted_project_index]->rename();
			project_comps[highlighted_project_index]->obj->save_project();
			break;
		case 'n'://create a new project
			
                default:
                        break;
        }
        if (curx >= LEFT_RIGHT_BORDER && focused != RIGHT)
                curx = LEFT_RIGHT_BORDER - 1;
        if (cury >= current_project_index)
                cury = current_project_index-1;
        move(cury, curx);
//         if (focused != RIGHT)
//                 highlightProj(0, LEFT_RIGHT_BORDER -1, cury, cury,COLOR_WHITE,A_STANDOUT);
        refresh();
        return running;
}

//########################################[STATIC FUNCTION DEFINITIONS]##############################

//UI_Comp should have a draw or display method
void drawTaskComp(UI_Comp<Task> comp){
        //Save the current cursor position
        int cury, curx;
        getyx(stdscr, cury, curx);
        //Draw the task
        move(comp.domain.y.first, comp.domain.x.first);
        addstr(comp.obj->name.c_str());
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

void loadTodo(Project &project){
        taskRoots.clear();
        taskRoots = parseFile(project.file_name);
        createUiComps();
}

void displayTodo(){
        for (auto it = taskComps.begin(); it != taskComps.end(); it++){
                drawTaskComp(*it);
        }
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
        for (auto it = taskComps.begin(); it != taskComps.end(); it++)
                if (it->obj->name == ""){
			it->rename();
                        break;
                }
}

void deleteTask(UI_Comp<Task> &comp){
        auto func = [&,comp](std::shared_ptr<Task> curTask, int d){
                for (auto subTask = curTask->subTasks.begin(); subTask != curTask->subTasks.end();
                                subTask++)
                        if ((*subTask)->name == comp.obj->name){
                                curTask->subTasks.erase(subTask);
                                break;
                        }
        };
        for (auto it = taskRoots.begin(); it != taskRoots.end(); it++){
                if ( (*it)->name ==comp.obj->name){
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

