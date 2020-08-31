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
    mloader.load();
    //mloader.printModel();
    Model m = mloader.nextModel();
    if(!m.event_list.empty()){
        args.insertECcmd(m.event_list);
        m.removeFreq();
    }else if(m.emptyModel) exit(0);
    //args.printCmd();

    Executer exec;
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

/****
 * yhrun -p th_mt -N 1 -n 1 -c 32 ./ECPlugin -t  ../lqcd-gcc8.2.0/compiled/chroma.20190402/bin/chroma -i data/hadspec_16_16_16_16.ini.xml       -o out_th_mt_1_16_16_16_16_16 --struct ./chroma.hpcstruct --prof ../lqcd-gcc8.2.0/chroma/
 * */