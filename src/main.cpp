#include "ECModel.hpp"
#include "Args.hpp"
#include "Executer.hpp"
#include "XMLParser.hpp"
#include "FindNewDir.hpp"
#include "ModelLoader.hpp"

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
    ModelLoader mloader;
    Executer exec;
    try{mloader.load();}
    catch(string s){
        if(s != "file not found") exit(0);
        args.insertECcmd(ECModel::event_list);
        int pos = args.getCommand(0).find(" ");
        string cmd = args.getCommand(0).substr(0, pos);
        string argvs = args.getCommand(0).substr(pos+1, args.getCommand(0).length());
        string ret = exec.exec(cmd, 0, argvs);
        //string ret = exec.exec(args.getCommand(0), 1);
        if(ret == "error") {
            throw "hpcrun error, exit with " + ret;
            exit(-1);
        }
        cout << ret;
        args.setMeasurementsPath(findNewDir("measurements"));
        if(!args.getStructFlag()){
            ret = exec.exec(args.getCommand(1), 1);
            if(ret == "error") {
                throw "hpcstruct error, exit with " + ret;
                exit(-1);
            }
            cout << ret;
        }
    
        ret = exec.exec(args.getCommand(2), 1);
        if(ret == "error") {
            throw "hpcprof error, exit with " + ret;
            exit(-1);
        }
        args.setDatabasePath(ret);
        cout<< ret;
        cout << "DB: " <<args.getDatabasePath()<<endl;
        XMLParser parser(args.getDatabasePath());
        parser.loadECArgs(ECModel::event_list);
        parser.setECFunc(ECModel::energyCost);
        parser.setModelName("EneryConsumption");
        parser.parse();
        return 0;
    }
    
    //mloader.printModel();
    Model m = mloader.nextModel();
    if(!m.event_list.empty()){
        args.insertECcmd(m.event_list);
        m.removeFreq();
    }else if(m.emptyModel) exit(0);
    //args.printCmd();

    
    int pos = args.getCommand(0).find(" ");
    string cmd = args.getCommand(0).substr(0, pos);
    string argvs = args.getCommand(0).substr(pos+1, args.getCommand(0).length());
    string ret = exec.exec(cmd, 0, argvs);
    //string ret = exec.exec(args.getCommand(0), 1);
    if(ret == "error") {
        throw "hpcrun error, exit with " + ret;
        exit(-1);
    }
    cout << ret;
    args.setMeasurementsPath(findNewDir("measurements"));
    if(!args.getStructFlag()){
        ret = exec.exec(args.getCommand(1), 1);
        if(ret == "error") {
            throw "hpcstruct error, exit with " + ret;
            exit(-1);
        }
        cout << ret;
    }
    
    ret = exec.exec(args.getCommand(2), 1);
    if(ret == "error") {
        throw "hpcprof error, exit with " + ret;
        exit(-1);
    }
    args.setDatabasePath(ret);
    cout<< ret;
    cout << "DB: " <<args.getDatabasePath()<<endl;
    XMLParser parser(args.getDatabasePath());
    parser.loadECArgs(m.event_list);
    parser.setECFunc(m.function);
    do{
        ModelLoader::setModel(m.q, mloader.getSampMap());
        parser.setModelName(m.modelName);
        parser.parse();
        m = mloader.nextModel();
    }while(!m.emptyModel);
    
    return 0;
}
