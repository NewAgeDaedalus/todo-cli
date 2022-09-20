#include <vector>
#include <string>
#include <memory>
class Task{
        public:
        bool Completed = false;
        std::string desc = ""; 
        std::vector<std::shared_ptr<Task>> subTasks;
//###################[Constructors]##########################
        Task();
        Task(std::string Desc,  bool completed);
        Task(std::string Desc);
};

std::vector<std::shared_ptr<Task>> parseFile(std::string fileName);
template <typename F>
void forEachNodeDo(std::shared_ptr<Task> curTask, F&& func, int d);
void saveProj(std::string projFile, std::string projName);
