#include "Args.hpp"
#include "ECModel.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>

using namespace std;

Args::Args(){
    Args::command[0] = "hpcrun";
    Args::command[1] = "hpcstruct";
    Args::command[2] = "hpcprof";
}

Args::~Args(){

}

string Args::getCommand(int n){
    if(n > 2 || n < 0){
        return NULL;
    }
    return Args::command[n];
}

void Args::setDatabasePath(string info){
    string mode = "Populating Experiment database: ";
    int pos = info.find(mode);
    if(pos == string::npos){
        cout<<"database location not found! Checkout the 'hpcprof' command runnable or not.\n";
    }else{
        string dbpath = info.substr(pos+mode.length(), info.length()-pos-mode.length()-1);
        this->databasePath = dbpath;
    }
}

void Args::setMeasurementsPath(string info){
    string profcmd = this->command[2];
    profcmd.append(info);
    this->command[2] = profcmd;
}

string Args::getDatabasePath(){
    return this->databasePath;
}

bool Args::getStructFlag(){
    return this->structFlag;
}

void Args::getMPICmd(){
    if(Args::mpi_cmd != ""){
        string mpi_cmd_t = "mpiexec " + Args::mpi_cmd ;
        Args::command[0] = mpi_cmd_t + " " + Args::command[0];
    }
}

void Args::getAppCmd(){
    string hpccmd = command[0];
    bool flag = false;
    string subcmd;
    istringstream istr(hpccmd);
    while(istr >> subcmd){
        if(subcmd == "-t") continue;
        if(subcmd[0] == '-') flag = false;
        else if(flag) break;
        else flag = true;
    }
    Args::app_cmd = subcmd;
    Args::command[1].append(" "+Args::app_cmd);
    while (istr >> subcmd){
        app_args.push_back(subcmd);
    }
    
    subcmd = Args::app_cmd;
    int pos = subcmd.find("/");
    while(pos != string::npos){
        subcmd = subcmd.substr(pos+1, subcmd.length()-pos);
        pos = subcmd.find("/");
    }
    Args::app_name = subcmd;
}

void Args::getProfCmd(){
    string profcmd;
    string source_code = Args::command[2].substr(8, Args::command[2].length()-8);
    if(!Args::structFlag){
        profcmd = "hpcprof -S ./" + Args::app_name + ".hpcstruct";
    }else {
        profcmd = "hpcprof -S" + Args::structPath;
    }
    
    profcmd.append(" -I " + source_code);
    profcmd.append(" ./hpctoolkit-" + app_name + "-measurements");
    Args::command[2] = profcmd;
}

void Args::insertECcmd(vector<string> eventVec = ECModel::event_list){
    string hpccmd = Args::command[0];

    int pos = hpccmd.find("-e");
    if(pos == string::npos){
        pos = hpccmd.find("--event");
    }
    if(pos == string::npos){
        pos = hpccmd.find("-t");
    }
    if(pos == string::npos){
        pos = hpccmd.find("--trace");
    }
    if(pos == string::npos){
        pos = hpccmd.find("hpcrun");
    }
    if(pos == string::npos){
        throw "hpcrun command contains errors.";
    }

    vector<string>::iterator it;
    for(it=eventVec.begin(); it!=eventVec.end(); it++){
        if(hpccmd.find(*it) == string::npos)
            hpccmd.insert(pos, "-e " + *it + " ");
    }

    Args::command[0] = hpccmd;
}

void Args::printCmd(){
    for(int i=0; i<3; i++){
        cout << "command[" << i << "]:" << Args::command[i] << endl;
    }
    cout << "APP command:" << Args::app_cmd << endl;
    cout << "APP name:" << Args::app_name << endl;
    vector<string>::iterator it;
    cout << "APP args:";
    for(it=Args::app_args.begin(); it!=Args::app_args.end(); it++){
        cout << *it  << " ";
    }
    cout << endl;
}

void Args::parse(int argc, const char* const argv[]){
    if(argc < 2) {
        throw "no valid parameters got";
        return;
    }
    try{
        int cmdIndex = 0;
        string cmd[4];
        //string str;
        char str[500];
        for(int i=1; i<argc; i++){
            //str = argv[i];
            //cout << str << endl;
            strcpy(str,argv[i]);
            if(str==NULL|| strcmp(str,"\0")==0){
                continue;
            }

            if(strcmp(str, "--struct") == 0){
                cmdIndex = 1;
                Args::structFlag=true;
                continue;
            }

            if(strcmp(str,"--prof")==0){
                cmdIndex = 2;
                continue;
            }

            if(strcmp(str,"--mpi")==0){
                cmdIndex = 3;
                continue;
            }
        
            cmd[cmdIndex].append(string(" ").append(str));
        }
        Args::command[0].append(cmd[0]);
        Args::command[2].append(cmd[2]);
        Args::structPath.append(cmd[1]);
        Args::mpi_cmd.append(cmd[3]);
    
        Args::getAppCmd(); 
        Args::getProfCmd();
        //Args::insertECcmd();
        Args::getMPICmd();
    }catch(exception e){
        cout << "error in parsing:" << e.what() << endl;
    }
    
}

