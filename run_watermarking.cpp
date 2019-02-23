#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>
#include <chrono>
#include <regex>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>


#include "CImg.h"
#include "util.cpp"
#include "sequential.cpp"
#include "parallel.cpp"
#include "ff_parallel.cpp"
#include "ff_parallel_one.cpp"



int main(int argc, char * argv[]) {

    if(argc < 1){ 
        initial_error(argv[0]);
        return -1;
    } 

    std::string input_path = fixing_path(argv[1]);
    std::string output_path = fixing_path(argv[2]);
    std::string watermark_path = argv[3];
    std::string mode = argv[4];
    std::string color = argv[5];
    int nw = atoi(argv[6]);
    bool c_state = false;

    //Starting Executions
    auto start   = std::chrono::high_resolution_clock::now();

    if( color.compare("-grey") == 0){
        c_state = true;
    }
    
    //sequential
    if( mode.compare("-seq") == 0){
        sequential(input_path, output_path, watermark_path, c_state);
    }
    
    //parallel
    else if( mode.compare("-par") == 0){
        parallel(input_path, output_path , watermark_path, c_state, nw);
    }
    
    //ff parallel
    else if( mode.compare("-ffpar") == 0){
        ff_parallel(input_path, output_path, watermark_path, c_state, nw);
    }

    //ff parallel one stage
    else if( mode.compare("-ffparone") == 0){
    
        ff_parallel_one(input_path, output_path, watermark_path, c_state, nw);
    }
   
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto msec    = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    std::cout << "Total time: " << msec << "msec." << std::endl;


    return 0;
}