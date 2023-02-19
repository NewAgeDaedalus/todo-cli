#include <vector>
#include <concepts>
#include <memory>
#include <string>


//########################################[Definitions]##############################

#define LEFT  0
#define RIGHT 1
#define LEFT_RIGHT_BORDER (COLS/4)

//########################################[Macros]##############################

//########################################[Structures]##############################
class Project;
struct comp_domain{
        std::pair<int, int> x;
        std::pair<int, int> y;
        comp_domain(int x_from, int x_to, int y_from, int y_to);
        comp_domain();
};

//########################################[Classes]##############################

template <typename T> 
class UI_Comp{
        public:
        struct comp_domain domain;
        std::shared_ptr<T> obj; 
        UI_Comp(struct comp_domain Domain, std::shared_ptr<T> Obj);
        UI_Comp();
	void draw_content();
	void highlight(int color, int mode);
	int rename();
};

//########################################[Functions]##############################

void initCurses();
void displayProjects();
void generate_project_comps(std::vector<std::shared_ptr<Project>> projects);
int parseCommandRight(int);
int parseCommandLeft(int);
