 
#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
#include <ff/parallel_for.hpp>

using namespace ff;


cimg_library::CImg<int> *watermark;
std::string path_in, path_out;
bool state = false;
float proc_time = 0.0;
std::vector<std::string> files;
int par = 0;


struct source_Farm: ff_node_t<std::pair<int, int>, std::pair<std::string*, cimg_library::CImg<int>*>>{

    source_Farm(float &load_time, int &start_gap, int &stop_gap): 
         load_time(load_time), start_gap(start_gap), stop_gap(stop_gap){};
  
std::pair<std::string*, cimg_library::CImg<int>*>* svc(std::pair<int, int> *task) {  

  if(stop_gap <= files.size() && start_gap <= files.size()){ 

    for(int i=start_gap; i < stop_gap; i++) { 

    auto start   = std::chrono::high_resolution_clock::now();
    
    std::string* path_file = new std::string(path_in + files[i]); 
    
    cimg_library::CImg<int>* img = new cimg_library::CImg<int>((*path_file).c_str());

    
    std::pair<std::string*, cimg_library::CImg<int>*>* p = 
                new std::pair<std::string*, cimg_library::CImg<int>*>(path_file, img); 
               
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    float load = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    load_time +=load;
    ff_send_out(p);
      }

    }

    return EOS;

    }
 float &load_time;
 int &start_gap;
 int &stop_gap;

};

struct proc_Node: ff_node_t<std::pair<std::string*, cimg_library::CImg<int>*>>{

  std::pair<std::string*, cimg_library::CImg<int>*>* svc(std::pair<std::string*, cimg_library::CImg<int>*> *task ) {

      float avg_proc = 0.0;

      process_image(*(task->second), *watermark, state, avg_proc);

      proc_time += avg_proc;
       
      return task;
  }

};

struct save_Farm: ff_node_t<std::pair<std::string*, cimg_library::CImg<int>*>>{

    save_Farm(float &save_time): 
    save_time(save_time)  {}

    std::pair<std::string*, cimg_library::CImg<int>*>* svc(std::pair<std::string*, cimg_library::CImg<int>*> *task ) { 

    std::pair<std::string*, cimg_library::CImg<int>*> &p = *task;

    auto start   = std::chrono::high_resolution_clock::now();

    if (*(p.second)) {

              std::string output_file (path_out + "DS_marked_" + get_filename(*(p.first)));
                  
              (*(p.second)).save(output_file.c_str());
        } 

      auto elapsed = std::chrono::high_resolution_clock::now() - start;
      auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    save_time += msec;

    delete p.second;
    delete task;
    return GO_ON;

  }

  std::string output_path;
  float &save_time;

};


int ff_parallel(std::string input_path, std::string output_path, std::string watermark_path, bool c_state, int nw){

   watermark = new cimg_library::CImg<int>(watermark_path.c_str());

  path_in = input_path;
  path_out = output_path;
  state = c_state;

  par = nw;

  float save_time = 0.0;
  float load_time = 0.0;
 

  char *out_dir = new char[output_path.length() + 1];

  std::strcpy(out_dir, output_path.c_str());

  struct stat st;

  if(stat(out_dir,&st) != 0){

      int status = mkdir(out_dir, S_IRWXU);

          if(status!=0) {

             std::cout << "Error mkdir "<< status << std::endl;
        }

        std::cout << "Create Directory " << output_path <<std::endl;
  }

  std::vector<std::unique_ptr<ff_node> > W;
  std::vector<std::unique_ptr<ff_node> > S;

 read_dir( path_in, files);

  float ratio = 0.0;
  //ratio evaluatet between sequential Stage 1 and Stage 3
  if(input_path.compare("image_800x600/ ")) ratio= 1.457087519;
  if(input_path.compare("image_1600x1200/")) ratio= 1.650527912;

  int nw_stage_1= std::round(nw/ratio);

  double gap = std::round(double(files.size())/nw_stage_1);
  int start_gap = 0;
  int stop_gap = gap;
  std::vector<int> intervals_start;
  std::vector<int> intervals_stop;

  for(int a = 0; a < nw_stage_1; a++){

    intervals_start.push_back(start_gap);
    intervals_stop.push_back(stop_gap);
    start_gap += gap;
    stop_gap  += gap;
    if(stop_gap > files.size()) stop_gap=files.size();

  }

  for(int f=0; f< nw_stage_1; f++){

     W.push_back(   make_unique<source_Farm>  (load_time,intervals_start[f],intervals_stop[f]));

  }

  for(int i = 0; i < nw; i++){

      S.push_back(   make_unique<save_Farm>    (save_time));
  }


  proc_Node processor;

  ff_Farm<float> First_farm(std::move(W));
  First_farm.remove_collector();

  ff_Farm<float> Second_farm(std::move(S));
  Second_farm.add_emitter(processor);
  Second_farm.setMultiInput(); 
  Second_farm.remove_collector();

  ff_Pipe<float> pipe(First_farm, Second_farm);    

    if ( pipe.run_and_wait_end()<0) {
         error("Running pipe ");
         return -1;
    }
  
  std::cout << "##############################################################" << std::endl;
  std::cout << "Processing " << files.size() << " images in Parallel mode." << std::endl;

  std::cout << "##############################################################" << std::endl;

  std::cout << "Load time "<< load_time << "msec."<< std::endl; 

  std::cout << "##############################################################" << std::endl;

  std::cout << "Processing time "<< proc_time << "msec."<< std::endl; 
  
  std::cout << "##############################################################" << std::endl;

  std::cout << "Save time: "<< save_time << "msec.\t" << std::endl;

  std::cout << "##############################################################" << std::endl;


return 0;

}

