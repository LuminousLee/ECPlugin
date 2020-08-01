#include "Executer.cpp"
#include <string>
#include <fstream>

using namespace std;

int main(){
    string cmd = "hpcrun -L";
    string res = "";
    Executer exec;
    res = exec.exec(cmd);
    fstream fs("./event_list.txt", ios::out);
    fs << res;
    fs.close();
    return 0;
}