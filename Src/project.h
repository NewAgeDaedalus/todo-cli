#include <string>
#include <iostream>
#include <vector>

#ifndef TASK_MANAGEMENT
#define TASK_MANAGEMENT
#include "taskManagement.h"
#endif

#ifndef UI
#define UI
#include "UI.h"
#endif

class Project{
	public:
	std::string name;
	std::string file_name;
	Project();
	Project(std::string name);
	Project(std::string file_name, std::string project_name);
	void save_project(std::vector<std::shared_ptr<Task>> &taskRoots);
	void save_project();
	void rename_project(std::string new_name);
};

void loadProjects(std::vector<std::shared_ptr<Project>> &projects);
