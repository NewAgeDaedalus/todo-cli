#include <vector>
#include <memory>
//########################################[Definitions]##############################

#define LEFT  0
#define RIGHT 1
#define LEFT_RIGHT_BORDER (COLS/4)

//########################################[Macros]##############################

//########################################[Structures]##############################
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
};

//########################################[Functions]##############################

void initCurses();
int loadProject(char *fileName);
int loadProjects();
void displayProject(const char*);
void displayProjects();
int parseCommandRight(int);
int parseCommandLeft(int);
void highlightProj(int x_from, int x_to, int y_from, int y_to, int mode);
