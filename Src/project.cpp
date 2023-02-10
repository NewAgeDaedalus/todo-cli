#include <cstdio>
#include <exception>
#include <memory>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <dirent.h>

#ifndef PROJECT
#define PROJECT
#include "project.h"
#endif


#ifndef UTILS
#define UTILS
#include "utils.h"
#endif

using std::shared_ptr;

//Opens an existing project from a file
Project::Project(std::string file_name){
	this->file_name = file_name;
	//open the file name
	FILE *fp = fopen(file_name.c_str(), "r");
	if (fp == NULL){
		fclose(fp);
		throw "No such project file";
	}
	size_t n = 0;
	char *proj_name;
	getline(&proj_name, &n, fp);
	name = std::string(proj_name);
	fclose(fp);
}

Project::Project(std::string file_name, std::string project_name){
	//check if the file_name already exists
	FILE *fp = fopen(file_name.c_str(), "r");
	if (fp != NULL){
		fclose(fp);
		throw "Project file already exists";
	}
	fclose(fp);
	//Create a project file
	fp = fopen(file_name.c_str(), "w");
	fprintf(fp, "%s\n", project_name.c_str());
	fclose(fp);
	//Set the attribute values
	this->file_name = file_name;
	this->name = project_name;
}

void Project::save_project(std::vector<std::shared_ptr<Task>> &taskRoots){
	std::ofstream file(file_name);
        file << name;//proj name contains \n
        auto func = [&](shared_ptr<Task> curTask, int d){
                for (int i = 0; i < d; i++)
                        file << "#";
                file << curTask->desc << " " << (curTask->Completed? "1":"0")<<"\n";
        };
        for (auto it = taskRoots.begin(); it != taskRoots.end();it++){
                forEachNodeDo(*it, func, 1);
        }

}

void Project::rename_project(std::string new_name){
	name = new_name;
}


void loadProjects(std::vector<shared_ptr<Project>> &projects){
        struct dirent **nameList;
        int n = scandir(".", &nameList, NULL, alphasort);
        while (n--){
                if (nameList[n]->d_name[0] == '.')
                        continue;

                projects.push_back((shared_ptr<Project>(new Project(nameList[n]->d_name))));
                free(nameList[n]);
        }
        free(nameList);
}


