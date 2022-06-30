#include <iostream>
#include <stdlib.h>
#include <vector>
#include <atomic>
#include <queue>
#include <mutex>
#include <functional>
#include <algorithm>
#include <barrier>
#include <fstream>
#include <sstream>

#include "./utils/utimer.cpp"
#include "./utils/utility.h"


// execute with
//for((i=1;i<33;i*=2)); do ./test_threadsetup.out $i; done


using namespace std;


int main(int argc, char * argv[]) {
    if(argc != 2){
        cout << "number_of_threads" << endl;
        return (0);
    }

    int nw = atoi(argv[1]); // number of threads

    // fucntion executed by the threads
    auto body = [](){
        return;
    };

    vector<thread> tids(nw);

    string string1="Setup " + to_string(nw) + " threads";
    long u;
    {
        utimer tsetup(string1, &u);
        for(int tid=0; tid<nw; tid++){
            tids[tid] = thread(body);
        }

        for(int i=0; i<nw; i++)
            tids[i].join();
    }


    return 0;
}
