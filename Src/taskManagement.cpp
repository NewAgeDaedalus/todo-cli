#include <fstream>
#include <iostream>
#include <istream>
#include <string>
#include <stdio.h>
#include <utility>
#include <vector>
#include <stdlib.h>
#include <memory>


#ifndef TASK_MANAGEMENT
#define TASK_MANAGEMENT
#include "taskManagement.h"
#endif

#ifndef UTILS
#define UTILS
#include "utils.h"
#endif

using std::vector; 
using std::string;
using std::shared_ptr;


vector<shared_ptr<Task>> taskRoots;

//###########################[Task class methods]#########################

Task::Task(string Desc, bool completed){
        name = Desc;
        Completed = completed;
}

Task::Task(string Desc){
        name = Desc;
}

Task::Task(){
        name ="";
        Completed = false;
}

//###########################[Static functions]#############################

static Task * createTask_discon(string line){
        int prefixLen = line.find_first_not_of('#');
        bool done = false;
        string desc;
        desc = line.substr(prefixLen, line.length()-2-prefixLen);
        done = line.substr(line.length()-1, 1) == "1"? true:false;
        return new Task(desc, done);
}

static void printTaskTree(shared_ptr<Task> curTask, int d){
        for (int i = 0; i < d; i++)
                std::cout<<" ";
        std::cout << curTask->name <<"\n";
        for (vector<shared_ptr<Task>>::iterator it = curTask->subTasks.begin(); it != (curTask->subTasks).end(); it++){
                printTaskTree(*it, d+1);
        }
}

//###########################[Functions]####################################

vector<shared_ptr<Task>> createTaskTrees(vector<string> &lines){
        std::vector<shared_ptr<Task>> tasksRoots;
        vector <std::pair<shared_ptr<Task>, int>> stack;
        for (auto line = lines.begin(); line != lines.end(); line++){
                int d = line->find_first_not_of('#');
                if (d == 1){
                        stack.clear();
                        tasksRoots.push_back(shared_ptr<Task>(createTask_discon(*line)));
                        stack.push_back(std::pair<shared_ptr<Task>, int>(tasksRoots.back(), 0));
                }
                else if (d > stack.back().second){ //Can only be 1 bigger if file is correct
                        shared_ptr<Task>newTask = shared_ptr<Task>(createTask_discon(*line));
                        stack.back().first->subTasks.push_back(newTask);
                        stack.push_back(std::pair<shared_ptr<Task>, int>(newTask, d));
                }else{
                        stack.pop_back();
                        line--;
                }
        }
        return tasksRoots;
}

vector<shared_ptr<Task>> parseFile(string fileName){
        vector<shared_ptr<Task>> tasksRoots;
        vector<string> lines;
        char *linePtr = NULL;
        size_t lineSize = 0;
        FILE *fp;
        fp = fopen(fileName.c_str(), "r");
        getline(&linePtr, &lineSize, fp);//Skip the first line
        while ( getline(&linePtr, &lineSize, fp) != -1){
                string line(linePtr);
                line = line.substr(0, line.size()-1); //Get rid of \n at the end
                if (line != "\n"){
                        lines.push_back(line);
                }
        }
        tasksRoots = createTaskTrees(lines);
//        for (auto it = tasksRoots.begin(); it != tasksRoots.end(); it++){
//                printTaskTree(*it, 0);
//                if (*it != tasksRoots.back())
//                        printf("\n\n");
//        }
        fclose(fp);
        return tasksRoots;
}

//Should not be here
void saveProj(string projFile, string projName){
        std::ofstream file(projFile);
        file << projName;//proj name contains \n
	file << "\n\n\n" << "\n\n\n";
        auto func = [&](shared_ptr<Task> curTask, int d){
                for (int i = 0; i < d; i++)
                        file << "#";
                file << curTask->name << " " << (curTask->Completed? "1":"0")<<"\n";
        };
        for (auto it = taskRoots.begin(); it != taskRoots.end();it++){
                forEachNodeDo(*it, func, 1);
        }
}
