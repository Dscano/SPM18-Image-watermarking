#include <iostream>
#include <mutex>
#include <algorithm>
#include <thread>
#include <condition_variable>
#include <deque>
#include <vector>
#include <chrono>
#include <cstddef>
#include <math.h>
#include <numeric>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>



/* 
Definition of a new type stringvec that is a vector of string, used to read the directory
passed in parameter. 
*/

std::regex base_regex("^(.(.*\\.jpg$|.*\\.JPG$))*$");
std::smatch base_match;

std::string fixing_path(const char* in){
  std::string out(in);
  if( in[strlen(in)-1] != '/')
     return std::string(out + "/");
  return out;
}


/*
The function iterates over the input directory and checks whether the file is an image or
not using a properly written regex. Then it inserts all the correct filenames into a stringvec.
*/
void read_dir(const std::string& name, std::vector<std::string>& v){
  

  DIR *dir;
  struct dirent *dp;
  char *open_dir = new char[name.length() + 1];

  std::strcpy(open_dir, name.c_str());


  if ((dir = opendir (open_dir)) != NULL){

    while ((dp = readdir(dir)) != NULL) {

      if (std::regex_match (dp->d_name, base_regex )){
              
              v.push_back(dp->d_name);
               
            }
          }
      closedir(dir);
  }
  else { /* could not open directory */
        std::cout << "could not open directory " << std::endl;

      }
}

void coloring_pixel(bool avg, cimg_library::CImg<int> &img, int h, int w){
    if (avg){
        // grey rgb(R + G + B + 255 / 4) 
        int rgb = ((int)img(h,w,0,0) + (int)img(h,w,0,1) + (int) img(h,w,0,2)+255)/4; 
        for(int ch = 0; ch < img.spectrum(); ch++)
            img(h,w,0,ch) = rgb;
    }

    else
        for(int ch = 0; ch < img.spectrum() ; ch++)
            img(h,w,0,ch) = 0;
}


//Processing immage for the sequential part
void process_seq( cimg_library::CImg<int> &img, cimg_library::CImg<int> &watermark, bool c_state ) {

  for (int h = 0; h < img.height(); h++){
    for (int w = 0; w < img.width(); w++){

      if(watermark(w, h) < 80){
        coloring_pixel(c_state, img, w, h);}
    }
  }
}

void process_image( cimg_library::CImg<int> &img, cimg_library::CImg<int> watermark, 
                    bool c_state, float &proc_time) {

  //PROCESSING IMAGE

  auto start = std::chrono::high_resolution_clock::now();

  for (int r = 0; r < img.height(); r++)
    for (int c = 0; c < img.width(); c++)
      if(watermark(c, r) < 80)
        coloring_pixel(c_state, img, c, r);  

  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  float msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
  proc_time +=(float) msec;   
}

void process_image_one( cimg_library::CImg<int> &img, cimg_library::CImg<int> watermark, 
                    bool c_state) {

  //PROCESSING IMAGE
  
  for (int r = 0; r < img.height(); r++)
    for (int c = 0; c < img.width(); c++)
      if(watermark(c, r) < 80)
        coloring_pixel(c_state, img, c, r);    
}

std::string get_filename(std::string s){

  std::string segment;
  std::vector<std::string> seglist;
  std::istringstream iss(s);

  while(std::getline(iss,segment,'/')){
        seglist.push_back(segment);
    }

  return seglist.back();
}

void initial_error(const char* arg){

    std::cerr << "Usage: " << arg << " input_dir output_dir watermark_file [-seq | -par ] [-balck/grey] [n_worker]" << std::endl;    
    std::cerr << "##########################################################" << std::endl;
    std::cerr << "input_dir:\t input directory where the images are located." << std::endl;
    std::cerr << "output_dir:\t output directory where the images have to be stored." << std::endl;
    std::cerr << "watermark_file:\t path of the image used to watermark the files." << std::endl;
    std::cerr << "-seq:\t\t sequential version. Specify n worker." << std::endl;
    std::cerr << "-par:\t\t parallel version in C++. Specify n worker." << std::endl;
    std::cerr << "-black/grey:\t watermark color." << std::endl;
    std::cerr << "nw:\t number of worker, seq case nw = 0." << std::endl;
    std::cerr << "Example of command composition: ./run_watermarking image_800x600/ out watermark_800x600.jpg -par 8 -black 3" << std::endl;
    std::cerr << "##########################################################" << std::endl;

    return;

}

template <typename T>
class queue_t
{
private:
  std::mutex              d_mutex;
  std::condition_variable d_condition;
  std::deque<T>           d_queue;
public:

  queue_t() {}

  
  void push(T const& value) {
    {
      std::unique_lock<std::mutex> lock(this->d_mutex);
      d_queue.push_front(value);
    }
    this->d_condition.notify_one();
  }
  
  T pop() {
    std::unique_lock<std::mutex> lock(this->d_mutex);
    this->d_condition.wait(lock, [=]{ return !this->d_queue.empty(); });
    T rc(std::move(this->d_queue.back()));
    this->d_queue.pop_back();
    return rc;
  }
};











