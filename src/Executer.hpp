#ifndef EXECUTER_H
#define EXECUTER_H

#include <cstdio>
#include <iostream>
#include <string>

using namespace std;

typedef struct{
    FILE* fp=NULL;
    int pid = 0;
}popen_res;

class Executer{
public:
    Executer();
    ~Executer();
    string exec(string cmd);
private:
    string execute(string cmd);
    void child(string cmd, int out);
    string parent(int in);
    popen_res mypopen(char *cmd,char type);
};

#endif