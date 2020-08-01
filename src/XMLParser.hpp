#pragma once
#include "lib/tinyxml2.h"
#include <iostream>
#include <vector>
#include <map>

using namespace tinyxml2;

class XMLParser{

public:
    XMLParser(){};
    XMLParser(std::string path);
    void loadECArgs(std::vector<std::string> event_list);
    bool parse();
    void setECFunc(double (*eneryCost)(std::map<std::string, int>, std::map<int, double> )){this->eneryCost = eneryCost;}
private:
    bool loadXML(std::string path);
    bool findEvent();
    bool insertHeader();
    void findMetricDataAndCal(XMLElement *elmt);
    double calECvalues();
    bool getEventValues(XMLElement *elmt);

    XMLDocument docXML;
    XMLElement *Metric;
    XMLElement *SecCallPathProfileData;
    std::vector<std::string> event_list;
    std::map<std::string, int> event_map;
    std::map<int, double> event_value;

    int nextI;

    std::string docPath;

    double (*eneryCost)(std::map<std::string, int>, std::map<int, double> );
};