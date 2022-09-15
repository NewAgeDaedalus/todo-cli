#define LEFT  0
#define RIGHT 1


void initCurses();
int loadProject(char *fileName);
int loadProjects();
int parseCommandRight(int);
int parseCommandLeft(int);
void  highlightProj(int x_from, int x_to, int y_from, int y_to, int mode);
