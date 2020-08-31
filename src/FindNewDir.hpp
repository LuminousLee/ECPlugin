#ifndef FINDNEWDIR_H
#define FINDNEWDIR_H

#include <stdio.h>
#include <string>
#include "Executer.hpp"

using namespace std;

string findNewDir(string templet){
    Executer exec;
    //string cmd = "ls -lt";
    string ls = exec.exec("ls -lt", 1);
    string res = "";
    int pos = ls.find(templet);
    if(pos != string::npos){
        res = ls.substr(pos+templet.length(), ls.length());
        pos = res.find("\n");
        if(pos!=string::npos){
            res = res.substr(0, pos);
        }
    }
    return res;
}
#endif