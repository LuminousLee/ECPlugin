
#ifndef ECMODEL_H
#define ECMODEL_H

#include <vector>
#include <string>
#include <map>

using namespace std;

class ECModel{
public:
    ECModel();
    ~ECModel();
    static bool init();
    static double energyCost(map<string, int> event_map, map<int, double> event_value);

    static vector<string> event_list;
    
private:
    static bool checkArgs();
    static double calEnergyCost(map<string, int> event_map, map<int, double> event_value);
};

#endif //ECMODEL_H