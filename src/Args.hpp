
#ifndef Args_h
#define Args_h

//************************* System Include Files ****************************

#include <iostream>
#include <string>
#include <vector>

using namespace std;

//***************************************************************************

class Args {
public: 
  Args(); 
  Args(int argc, const char* const argv[]);
  ~Args(); 

  // Parse the command line
  void parse(int argc, const char* const argv[]);

  string getCommand(int n);
  string getDatabasePath();
  void setDatabasePath(string info);
  void setMeasurementsPath(string info);

  void printCmd();
private:
    void insertECcmd();
    void getAppCmd();
    void getProfCmd();
    void getMPICmd();
    string command[3];
    string databasePath;
    string mpi_cmd;
    string app_name;
    string app_cmd;
    vector<string> app_args;
}; 



#endif // Args_h
