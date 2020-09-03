#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <string>
#include <stack>

using namespace std;

typedef struct {
    bool isStr = false;
    double var1;
    string var2="null";
}VarType;

typedef struct {
    bool isOp = false;
    VarType var;
    char op;
}NodeType;

class Model{
public:
    Model(){emptyModel = true;}
    Model(vector<string> t):event_list(t),emptyModel(true){}
    Model(vector<string> t, double (*function)(std::map<std::string, int>, std::map<int, double>), string name, queue<NodeType> q )
    :event_list(t),function(function),modelName(name),q(q)
    {}
    void removeFreq(){
        for(vector<string>::iterator it = event_list.begin(); it!=event_list.end(); it++){
            int pos = (*it).find("@");
            if(pos != string::npos){
                (*it) = (*it).substr(0, pos);
            }
        }
    }
    bool emptyModel = false;
    queue<NodeType> q;
    vector<string> event_list;
    string modelName;
    double (*function)(std::map<std::string, int>, std::map<int, double> );
};

class ModelLoader{
public:
    void load(string filepath="model.hpcmodel");
    void printModel();
    map<string, string> getSampMap(){ return SampMap;};
    Model nextModel();
    static void setModel(queue<NodeType> t, map<string, string> m){
        calQueue = t; 
        for(map<string, string>::iterator it=m.begin(); it!=m.end(); it++){
            int pos = it->second.find("@");
            if(pos != string::npos) it->second = it->second.substr(0, pos);
        }
        calMap = m;
    }
private:
    static double model(map<string, int> event_map, map<int, double> event_value);
    int level(char thechar){ 
	    string s = "+-*/";
	    return s.find(thechar);
    }
    bool in_set(char thechar){
	    string s = "+-*/()=";
	    if(s.find(thechar) != string::npos) return true;
	    return false;
    }
    bool is_digit(char theChar){
        if(theChar>='0' && theChar<='9') return true;
        if(theChar=='.') return true;
        return false;
    }

    static void printNode(NodeType t){
        if(t.isOp) cout << t.op << " ";
        else if(t.var.isStr) cout << t.var.var2 << " ";
        else cout << t.var.var1 << " ";
    }

    static map<string, string> calMap;
    static queue<NodeType> calQueue;
    map<string, string> SampMap;
    vector<string> modelVector;
    vector<string>::iterator iter;
};
