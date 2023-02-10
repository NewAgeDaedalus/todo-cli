#include <iostream>
#include <functional>
#ifndef TASK_MANAGEMENT
#define TASK_MANAGEMENT
#include "taskManagement.h"
#endif

void forEachNodeDo(std::shared_ptr<Task> curTask, std::function<void(std::shared_ptr<Task>, int)> func, int d);
