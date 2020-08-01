#include "ECModel.hpp"
using namespace std;

ECModel::ECModel(){
    
}

ECModel::~ECModel(){

}

vector<string> ECModel::event_list = vector<string>();

bool ECModel::init(){
    ECModel::event_list.push_back("cycles");
    ECModel::event_list.push_back("instructions");
    ECModel::event_list.push_back("cache-misses");
    //ECModel::event_list.push_back("llc-load-misses");
    //ECModel::event_list.push_back("llc-store-misses");
    //ECModel::event_list.push_back("llc-prefetch-misses");
    return true;
}

double ECModel::energyCost(map<string, int> event_map, map<int, double> event_value){
    if(checkArgs())
        return calEnergyCost(event_map, event_value);
    else
        return -1;
        
}

bool ECModel::checkArgs(){
    return true;
}

double ECModel::calEnergyCost(map<string, int> event_map, map<int, double> event_value){
    double cycles = 0, ins = 0, cahmis = 0;
    // llc_load = 0, llc_store = 0, llc_prefetch = 0;
    try{
        cycles = event_value[event_map["cycles"]];
    }catch(exception e){
        cycles = 0;
    }
    try{
        ins = event_value[event_map["instructions"]];
    }catch(exception e){
        ins = 0;
    }
    try{
        cahmis = event_value[event_map["cache-misses"]];
    }catch(exception e){
        cahmis = 0;
    }
    /*
    try{
        llc_load = event_value[event_map["llc-load-misses"]];
    }catch(exception e){
        llc_load = 0;
    }
    try{
        llc_store = event_value[event_map["llc-store-misses"]];
    }catch(exception e){
        llc_store = 0;
    }
    try{
        llc_prefetch = event_value[event_map["llc-prefetch-misses"]];
    }catch(exception e){
        llc_prefetch = 0;
    }
    if(llc_prefetch+llc_load+llc_store<0.1){
        return 0;
    }*/
    return 47.675 + 23.834*cycles + 2.093*ins + 72.113*cahmis;
}
