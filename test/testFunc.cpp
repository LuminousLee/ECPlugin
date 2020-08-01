#include <iostream>

using namespace std;

int main(){
    int i, j;
    i = j = 1;
    long long int result = 1;
    for(i=1; i<6550; i++){
        for(j=1; j<6550; j++){
                result *=i;
                result %= 1234567890;
                result *=j;
                result %= 1234567890;
        }
    }
    cout << "result:" << result << endl;
    return 0;
}