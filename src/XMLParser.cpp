#include "XMLParser.hpp"
#include <fstream>
#include <sstream>
#include <stack>

using namespace tinyxml2;

XMLParser::XMLParser(std::string path){
    try{
        this->docPath = path + "/experiment.xml";
        XMLParser::loadXML(this->docPath);
    }catch(std::exception e){
        std::cout << e.what();
        exit(-2);
    }catch(int i){
        std::cout << "load xml error" << std::endl;
        exit(-1);
    }
}

bool XMLParser::loadXML(std::string path){
    std::cout<<"XML: "<<path<<std::endl;
    XMLError errXML = docXML.LoadFile(path.c_str());
    if (XML_SUCCESS == errXML){
        std::cout<<"XML: load success"<<std::endl;
        return true;
    }
    else {
        throw -1;
    }
}

void XMLParser::loadECArgs(std::vector<std::string> event_list){
    this->event_list = event_list;
    if( !XMLParser::findEvent()){
        std::cout << "event not found in experiment.xml" <<std::endl;
        exit(-3);
    }
}

bool XMLParser::parse(){
    try{
        this->insertHeader();
        this->findMetricDataAndCal(this->SecCallPathProfileData);
    }catch(std::exception e){
        std::cout << e.what() << std::endl;
        return false;
    }catch(int i){
        std::cout << "parse error with error code " << i << std::endl;
    }
    return true;
}

bool XMLParser::findEvent(){
    XMLElement *elmtSecCallPathProfile = docXML.RootElement()->FirstChildElement("SecCallPathProfile");
    if(elmtSecCallPathProfile == NULL) return false;
    XMLElement *elmtSecCallPathProfileData = elmtSecCallPathProfile->FirstChildElement("SecCallPathProfileData");
    if(elmtSecCallPathProfileData == NULL) return false;
    this->SecCallPathProfileData = elmtSecCallPathProfileData;
    XMLElement *elmtSecHeader = elmtSecCallPathProfile->FirstChildElement("SecHeader");
    if(elmtSecHeader == NULL) return false;
    XMLElement *elmtMetricTable = elmtSecHeader->FirstChildElement("MetricTable");
    if(elmtMetricTable == NULL) return false;
    XMLElement *elmtMetric = elmtMetricTable->FirstChildElement("Metric");
    if(elmtMetric == NULL) return false;
    this->Metric = elmtMetric;
    int nextI = 0;
    std::stringstream ss;
    try{
        while(elmtMetric != NULL){
            int i = atoi(elmtMetric->Attribute("i"));
            nextI = nextI <= i ? i+1 : nextI;
            //if(elmtMetric->Attribute("t") == "inclusive"){
                std::string elmt_name = elmtMetric->Attribute("n");
                std::cout << "XML: event found : " << elmt_name << std::endl;
                std::vector<std::string>::iterator it;
                for(it=XMLParser::event_list.begin(); it!=XMLParser::event_list.end(); it++){
                    if(elmt_name.find(*it) != std::string::npos){
                        if(this->event_map.count(*it)==1 && elmt_name.find("(I)") == std::string::npos)
                            continue;
                        this->event_map[*it] = i;
                        //std::cout<<"XML: push " << elmt_name << std::endl;
                    }
                }
            //}
            elmtMetric = elmtMetric->NextSiblingElement();
        }
        if(this->event_list.size() > this->event_map.size()){
            std::cout<<"no available event found"<<std::endl;
            return false;
        }
        this->nextI = nextI;
    }catch(std::exception e){
        std::cout<<e.what()<<std::endl;
        exit(-4);
    }
    return true;
}

bool XMLParser::insertHeader(){
    XMLNode *thisNode = this->Metric;
    XMLNode *parentNode = thisNode->Parent();

    while(thisNode->NextSiblingElement()!=NULL){
        thisNode = thisNode->NextSiblingElement();
    }

    XMLElement *newMetricElmt = docXML.NewElement("Metric");
    newMetricElmt->SetAttribute("i", this->nextI);
    newMetricElmt->SetAttribute("n", "Energy consumption");
    newMetricElmt->SetAttribute("v", "derived-incr");
    newMetricElmt->SetAttribute("em", "0");
    newMetricElmt->SetAttribute("es", "0");
    newMetricElmt->SetAttribute("ep", "0");
    newMetricElmt->SetAttribute("t", "inclusive");
    newMetricElmt->SetAttribute("show", "1");
    newMetricElmt->SetAttribute("show-percent", "1");
    
    XMLElement *newMetricFormula1 = docXML.NewElement("MetricFormula");
    //char buf[5];
    //itoa(this->nextI, buf, 10);
    std::string  id = "$" + std::to_string(this->nextI);
    newMetricFormula1->SetAttribute("t", "combine");
    std::string frm = "sum(" + id + "," + id + ")";
    newMetricFormula1->SetAttribute("frm", frm.c_str());

    XMLElement *newMetricFormula2 = docXML.NewElement("MetricFormula");
    newMetricFormula2->SetAttribute("t", "finalize");
    newMetricFormula2->SetAttribute("frm", id.c_str());

    XMLElement *newInfoElmt = docXML.NewElement("Info");

    XMLElement *newNVElmt1 = docXML.NewElement("NV");
    newNVElmt1->SetAttribute("n", "units");
    newNVElmt1->SetAttribute("v", "events");

    
    XMLElement *newNVElmt2 = docXML.NewElement("NV");
    newNVElmt2->SetAttribute("n", "period");
    newNVElmt2->SetAttribute("v", "1");
    

    newInfoElmt->InsertEndChild(newNVElmt1);
    newInfoElmt->InsertEndChild(newNVElmt2);
    newMetricElmt->InsertEndChild(newMetricFormula1);
    newMetricElmt->InsertEndChild(newMetricFormula2);
    newMetricElmt->InsertEndChild(newInfoElmt);
    parentNode->InsertAfterChild(thisNode, newMetricElmt);
    //newElmt->SetAttribute("n", nextI);
    
    //parentNode->InsertAfterChild(thisNode, newElmt);
    return true;
}

void XMLParser::findMetricDataAndCal(XMLElement *elmt){
    std::stack<XMLElement*> s;
    std::stringstream ss;
    s.push(elmt);  
    XMLElement* t_elmt; 

    long numberOfFound = 0;
  
    while(!s.empty())  {
        t_elmt = s.top();  
        s.pop();  
  
        if(std::string(t_elmt->Name()) == "M"){
            int tint = atoi(t_elmt->Attribute("n"));
            std::string name;
            std::map<std::string, int>::iterator iter;
            iter = this->event_map.begin();
            while(iter != this->event_map.end()){
                if(iter->second == tint){
                    name = iter->first;
                }
                iter++;
            }
            std::vector<std::string>::iterator viter = this->event_list.begin();
            bool avail_flag = false;
            while(viter != event_list.end()){
                if(name.find(*viter)!=std::string::npos){
                //if(*viter == name){
                    avail_flag = true; break;
                }
                viter++;
            }
            if(avail_flag == false) continue;
            /*
            std::map<int, bool> m_flag;
            iter = this->event_map.begin();
            while(iter != this->event_map.end()){
                m_flag[iter->second] = false;
                iter++;
            }
            XMLElement* t = t_elmt;
            while(t != NULL){
                //ss << t->Attribute("n");
                //int index; ss >> index;
                if(std::string(t->Name())!="M") break;
                int index = atoi(t->Attribute("n"));
                //std::cout<<"find n:" << index << std::endl;
                m_flag[index] = true;
                t = t->NextSiblingElement();
            }
            bool deal = true;
            iter = this->event_map.begin();
            while(iter != this->event_map.end()){  
                if(m_flag[iter->second] == false){
                    deal = false;
                    break;
                }
                iter++;
            }
            */
            
            //if(deal){
            if(avail_flag){
                this->getEventValues(t_elmt);
                double ec = this->calECvalues();
                numberOfFound++;
                XMLNode *thisNode = t_elmt;
                XMLNode *parentNode = t_elmt->Parent();
                XMLElement *newElmt = docXML.NewElement("M");
                newElmt->SetAttribute("n", nextI);
                newElmt->SetAttribute("v", ec);
                parentNode->InsertAfterChild(thisNode, newElmt);
            }
            
            XMLElement* t = t_elmt->NextSiblingElement();
            while(t != NULL){
                if(std::string(t->Name())!="M") break;
                t_elmt = t;
                t = t->NextSiblingElement();
            }
        }
        t_elmt = t_elmt->FirstChildElement();
        while(t_elmt != NULL){
            s.push(t_elmt);
            t_elmt = t_elmt->NextSiblingElement();
        } 
        
    } 
    docXML.SaveFile(this->docPath.c_str(), false);
    
    std::fstream fs(this->docPath, std::ios::in | std::ios::out);
	char lineData[1024] = {0};
    std::string line = "";
	while(fs.getline(lineData, sizeof(lineData))){
        line = lineData;
        int pos = line.find("]&gt;");
		if (pos != std::string::npos){
            fs.seekg(-6, std::ios_base::cur);
            fs << "]>   ";
		}
	}
	fs.close();

    std::cout << "number of ec : " << numberOfFound << std::endl;
}

bool XMLParser::getEventValues(XMLElement *elmt){
    this->event_value.clear();
    //std::stringstream ss;
    try{
        while(elmt != NULL && std::string(elmt->Name())=="M"){
            int index = atoi(elmt->Attribute("n"));
            double value = atof(elmt->Attribute("v"));
            //std::cout<<elmt->Attribute("n")<<": \t"<<elmt->Attribute("v")<<"\t "<<value<<std::endl;
            this->event_value[index] = value;
            elmt = elmt->NextSiblingElement();
        }
    }catch(std::exception e){
        return false;
    }
    return true;
}

double XMLParser::calECvalues(){
    double ec = -1;
    try{
        ec = (this->eneryCost)(event_map, event_value);
    }catch(std::exception e){
        std::cout << e.what() << std::endl;
    }
    return ec;
}