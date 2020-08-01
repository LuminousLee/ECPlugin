#include "ECModel.hpp"
#include "Args.hpp"
#include "Executer.hpp"
#include "XMLParser.hpp"
#include "FindNewDir.hpp"

#include <iostream>

using namespace std;

static int real_main(int argc, char* argv[]);

int main(int argc, char* argv[]){
    try{
        return real_main(argc, argv);
    }
    catch(exception e){
        cout << "error occured: " << e.what() << endl;
    }
}

static void init(){
    ECModel::init();
}

static int real_main(int argc, char* argv[]){
    init();
    Args args;
    args.parse(argc, argv);
    //cout<<"pid: "<<getpid()<<endl;
    //args.printCmd();
    Executer exec;
    string ret = exec.exec(args.getCommand(0));
    if(ret == "error") {
        throw "hpcrun error, exit with " + ret;
        exit(-1);
    }
    args.setMeasurementsPath(findNewDir("measurements"));
    //int npos = ret.find("\n");
    //string pid = ret.substr(3, npos);
    //cout<<pid<<endl;
    //cout<<"res content:\n" << ret<<endl;
    
    ret = exec.exec(args.getCommand(1));
    if(ret == "error") {
        throw "hpcstruct error, exit with " + ret;
        exit(-1);
    }
    //cout<<"res content:\n" << ret<<endl;

    ret = exec.exec(args.getCommand(2));
    if(ret == "error") {
        throw "hpcprof error, exit with " + ret;
        exit(-1);
    }
    args.setDatabasePath(ret);
    cout<< ret;

    //XMLParser parser(args.getDatabasePath());
    cout << "DB: " <<args.getDatabasePath()<<endl;
    XMLParser parser(args.getDatabasePath());
    //XMLParser parser("./resources");
    parser.setECFunc(ECModel::energyCost);
    parser.loadECArgs(ECModel::event_list);
    parser.parse();
    return 0;
}
