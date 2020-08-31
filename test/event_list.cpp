#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#define MAXLINE 2048
using namespace std;

string execute(string cmd){
    char result_buf[MAXLINE];
    string res = "";
    int ret = 0;
    FILE *fp;
    printf("command:【%s】", cmd.c_str());
    fp = popen(cmd.c_str(), "r");
    if(fp == NULL){
        perror("popen error");
        exit(1);
    }
    while(fgets(result_buf, sizeof(result_buf), fp) != NULL){
        res.append(result_buf);
    }

    ret = fclose(fp);
    if(ret == -1){
        perror("close file pointer failed");
        exit(1);
    }
    else{
        printf("exit with status【%d】command return【%d】\n", ret, WEXITSTATUS(ret));
    }
    return res;
}
int main(){
    string cmd = "hpcrun -L";
    string res = "";
    res = execute(cmd);
    fstream fs("./event_list.txt", ios::out);
    fs << res;
    fs.close();
    return 0;
}