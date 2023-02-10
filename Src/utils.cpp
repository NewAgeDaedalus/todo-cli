#include <iostream>
#include <functional>
#include <utility>

#ifndef TASK_MANAGEMENT
#define TASK_MANAGEMENT
#include "taskManagement.h"
#endif


void forEachNodeDo(std::shared_ptr<Task> curTask, std::function<void(std::shared_ptr<Task>, int)> func, int d){
        func(curTask, d);
        for (std::vector<std::shared_ptr<Task>>::iterator it = curTask->subTasks.begin(); it != (curTask->subTasks).end(); it++){
                forEachNodeDo(*it, func, d+1);
        }
}

