#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <string>
#include "ModelLoader.hpp"

using namespace std;

queue<NodeType> ModelLoader::calQueue;
map<string, string> ModelLoader::calMap;
/*** FILE content example
 * event: $1:CPUTIME@5000
 * event: $2:instructions
 * event: $3:cache-misses
 * model: $1+$2+$3=MyModel
 * */
void ModelLoader::load(string filepath){
    fstream fs1(filepath, std::ios::in);
    if(!fs1) throw string("file not found");
    char line[100];
    while(!fs1.eof()){
        //read line
        fs1.getline(line,100);
        string lineStr = line;
        //remove ' '
        int pos = lineStr.find(" ");
        while(pos != string::npos){
            lineStr.replace(pos, 1, "");
            pos = lineStr.find(" ");
        }
        //skip '#'
        if(lineStr[0] == '#') continue;
        //load event ($1:CPUTIME@5000)
        pos = lineStr.find("event:");
        if(pos != string::npos){
            //remove substring "event:"
            string subStr = lineStr.substr(pos+6);
            int length = subStr.length();
            //cut string
            int center = subStr.find(":");
            string sub1 = subStr.substr(0, center);
            string sub2 = subStr.substr(center+1);
            //load into SampMap (map<string, string>)
            if(sub1[0]=='$'){
                ModelLoader::SampMap[sub1] = sub2;
            }else if(sub2[0]=='$'){
                ModelLoader::SampMap[sub2] = sub1;
            }
        }
    }
    //print
    /*
    map<string, string>::iterator it = ModelLoader::SampMap.begin();
    while(it != SampMap.end()){
        cout << "it->first:"<<it->first<< " length:" << it->first.length()<<endl;
        cout << "it->second:";cout<<it->second;cout<< " length:" ;cout<< it->second.length()<<endl;
        it++;
    }*/
    /*
    map<string, string>::iterator it = ModelLoader::SampMap.begin();
    while(it != SampMap.end()){
        cout << it->first << "|" << it->second << endl;
        it++;
    }*/
    fs1.close();
    fstream fs2(filepath, std::ios::in);//reload
    while(!fs2.eof()){
        //read line
        fs2.getline(line,100);
        string lineStr = line;
        //remove ' '
        int pos = lineStr.find(" ");
        while(pos != string::npos){
            lineStr.replace(pos, 1, "");
            pos = lineStr.find(" ");
        }
        //skip '#'
        if(lineStr[0] == '#') continue;
        //load model ($1+$2+$3=MyModel)
        pos = lineStr.find("model:");
        if(pos != string::npos){
            //remove substring "model:"
            lineStr = lineStr.substr(pos+6);
            //replace '$' with 'event' ($1 -> CPUTIME@5000)
            /*
            map<string, string>::iterator it = ModelLoader::SampMap.begin();
            while(it != SampMap.end()){
                string t = it->first;// "$?"
                cout << "it->first:"<<it->first<< " length:" << it->first.length()<<endl;
                cout << "it->second:"<<it->second<< " length:" << it->second.length()<<endl;
                int p = lineStr.find(t);
                
                while(p != string::npos){
                    cout << "before:" << lineStr << " length:" << lineStr.length() <<endl;
                    //replace1
                    //lineStr.replace(p, t.length(), it->second);
                    //replace2
                    string sub1 = lineStr.substr(0, p);
                    string sub2 = lineStr.substr(p+t.length(), lineStr.length()-p-t.length());
                    cout << "sub1:"<<sub1<<endl;
                    cout << "sub2:"<<sub2<<endl;
                    lineStr = "";
                    string lineStr = sub1 + it->second + sub2;
                    //replace2 end
                    cout << "after:" << lineStr << " length:" << lineStr.length() <<endl;
                    p = lineStr.find(t);
                }
                it++;
            }
            //load into modelVector (vector<string>)
            */
            ModelLoader::modelVector.push_back(lineStr);
        }
    }
    fs2.close();
    ModelLoader::iter = ModelLoader::modelVector.begin();
}

void ModelLoader::printModel(){
    vector<string>::iterator iter = ModelLoader::modelVector.begin();
    while(iter != ModelLoader::modelVector.end()){
        cout << "model:" << *iter << endl;
        iter++;
    }
}

Model ModelLoader::nextModel(){
    vector<string> event_list_t;
    map<string, string>::iterator iter_t = SampMap.begin();
    while(iter_t != SampMap.end()){
        string tstr = iter_t->second;
        event_list_t.push_back(tstr);
        iter_t++;
    }
    if(ModelLoader::iter != ModelLoader::modelVector.end()){
        string modelStr = *iter;
        VarType t_var;
        NodeType t_node;
        double base = 0.1;
        bool del = false;
        bool digit = false;
        bool endigit = true;
        double tmp = 0;
        string event = "";
        string modelName = "Model";
        queue<NodeType> q;
        stack<char> s;
        for(int i=0; i<modelStr.length(); i++){
            if(modelStr[i] == ' ') continue;
            
            if(in_set(modelStr[i])){
                if(digit){
                    t_var.isStr = false;
                    t_var.var1 = tmp;
                    t_node.isOp = false;
                    t_node.var = t_var;
                    q.push(t_node);
                }else if(event != ""){
                    t_var.isStr = true;
                    t_var.var2 = event;
                    t_node.isOp = false;
                    t_node.var = t_var;
                    q.push(t_node);
                }
                event = ""; digit = false; del = false; tmp = 0; base = 0.1;
                // '('
                if(modelStr[i] == '='){
                    endigit = del = false;
                    int pos = modelStr.find("=");
                    modelName = modelStr.substr(pos+1);
                    break;
                }
                else if(modelStr[i] == '(') {
                    s.push(modelStr[i]);
                    endigit = true; del = false;
                }
                else if(modelStr[i] == ')'){
                    endigit = false;del = false;
                    while(s.top()!='('){
                        t_node.isOp = true;
                        t_node.op = s.top();
                        s.pop();
                        q.push(t_node);
                    }
                    s.pop();
                }
                else if(s.empty()) {
                    s.push(modelStr[i]);
                    endigit = true;del = false;
                }
                else if(level(modelStr[i])>=level(s.top())) {
                    s.push(modelStr[i]);
                    endigit = true;del = false;
                }
                else{
                    endigit = true;del = false;
                    while(s.top()!='(' && level(s.top())>level(modelStr[i])){
                        t_node.isOp = true;
                        t_node.op = s.top();
                        s.pop();
                        q.push(t_node);
                    }
                    s.push(modelStr[i]);
                }
            }else if(is_digit(modelStr[i]) && endigit){
                digit = true;
                if(modelStr[i] == '.') {
                    del = true;
                    continue;
                }
                if(del){
                    tmp = 1.0*tmp + base * (modelStr[i] - '0'); base = base / 10;
                }else{
                    tmp = 1.0*tmp * 10 + modelStr[i] - '0';
                }
            }else{
                event += modelStr[i]; 
                digit = endigit = del = false;
            }
        }
        while(!s.empty()){
            t_node.isOp = true;
            t_node.op = s.top();
            s.pop();
            q.push(t_node);
        }
        /*
        queue<NodeType> qt = q;
        cout << "Queue:" << endl;
        while(!qt.empty()){
            NodeType t = qt.front();
            qt.pop();
            if(t.isOp) cout << " " << t.op;
            else if(t.var.isStr) cout << " " << t.var.var2;
            else cout << " " << t.var.var1;
        }cout << endl;
        */
        
        Model m(event_list_t, &model, modelName, q);
        ModelLoader::iter++;
        return m;
    }
    return Model(event_list_t);
}

double ModelLoader::model(map<string, int> event_map, map<int, double> event_value){
    stack<NodeType> s;
    NodeType t1, t2;
    queue<NodeType> Queue;
    if(ModelLoader::calQueue.empty()) {
        cout << "Empty model" << endl;
        return 0;
    }else {
        Queue = ModelLoader::calQueue;
    }

    /*
    cout << endl << "EventMap:" << endl;
    for(map<string, int>::iterator mit1=event_map.begin(); mit1!=event_map.end(); mit1++){
        cout << mit1->first << "\t|" << mit1->second;
        try{
            cout << "\t" << event_value[mit1->second] << endl;
        }catch(exception e){}
    }
    cout << "EventSamp:"<<endl;
    for(map<string, string>::iterator mit2=calMap.begin(); mit2!=calMap.end(); mit2++){
        cout << mit2->first << "\t|" << mit2->second << endl;
    }*/
    t1 = Queue.front();
    Queue.pop();
    t2 = Queue.front();
    Queue.pop();
    while(!Queue.empty()){
        while(!Queue.front().isOp && !Queue.empty()){
            s.push(t1);
            t1 = t2;
            t2 = Queue.front();
            Queue.pop();
        }
        //printNode(t1); printNode(t2); printNode(Queue.front()); cout << endl;
        if(t1.var.isStr){
            try{
                t1.var.var1 = event_value[event_map[calMap[t1.var.var2]]];
            }catch(exception e){
                t1.var.var1 = 0;
            }
            
            t1.var.isStr = false;
        }
        if(t2.var.isStr){
            try{
                t2.var.var1 = event_value[event_map[calMap[t2.var.var2]]];
            }catch(exception e){
                t2.var.var1 = 0;
            }
            t2.var.isStr = false;
        }
        Queue.front().isOp = false;
        Queue.front().var.isStr = false;
        switch(Queue.front().op){
            case '+': Queue.front().var.var1 = t1.var.var1 + t2.var.var1; break;
            case '-': Queue.front().var.var1 = t1.var.var1 - t2.var.var1; break;
            case '*': Queue.front().var.var1 = t1.var.var1 * t2.var.var1; break;
            case '/': Queue.front().var.var1 = t1.var.var1 + t2.var.var1; break;
            default : break;
        }
        t2 = Queue.front();
        Queue.pop();
        if(s.empty() && Queue.empty()){
            break;
        }
        if(!s.empty()){
            t1 = s.top();
            s.pop();
        }else{
            t1 = t2;
            t2 = Queue.front();
            Queue.pop();
        }
    }
    //cout << t2.var.var1 << endl;
    return t2.var.var1;
}