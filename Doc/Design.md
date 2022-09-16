# DESIGN

## Todo file format
First line is the project name and it is not part of the todo list.\
Each task is prefixed with the charachter # multiple times coressponding\
to its position in the hierarchy. At the end of the task there is always\
appended a C which can be either 0 or 1. 1 indicates that the task was completed,\
0 was not completed.\
Example
```
#Task 1 C
##Task 1.1 C
#Task 2 C
##Task 2.1 C
##Task 2.2 C
###Task 2.2.1 C
##Task 2.3 C
```

