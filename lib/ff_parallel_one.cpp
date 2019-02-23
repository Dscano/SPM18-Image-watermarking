#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
#include <ff/parallel_for.hpp>

using namespace ff;


cimg_library::CImg<int> *watermark_ff_one;
std::string path_in_ff_one, path_out_ff_one;
bool state_ff_one = false;
std::vector<std::string> files_ff_one;
int par_ff_one = 0;

struct Surce: ff_node_t<std::string>{

  std::string* svc(std::string *in)  {  

    read_dir( path_in_ff_one, files_ff_one);

    double gap = std::round(double(files_ff_one.size())/par_ff_one);

    int start_gap = 0;
    int stop_gap = gap;
    
    for(int i= 0; i < par_ff_one; i++){

      if(stop_gap <= files_ff_one.size() && start_gap <= files_ff_one.size()){

        std::pair<int, int>* intervall = new std::pair<int, int>(start_gap,stop_gap);
        start_gap += gap;
        stop_gap += gap;
        
        if(stop_gap > files_ff_one.size()) stop_gap=files_ff_one.size();

        ff_send_out(intervall);
    }
  }

  return EOS;

  }

};

struct Farm: ff_node_t<std::pair<int, int>, std::pair<std::string*, cimg_library::CImg<int>*>>{

   Farm(float &load_time, float &save_time, float &proc_time): 
         load_time(load_time), save_time(save_time), proc_time(proc_time){};

std::pair<std::string*, cimg_library::CImg<int>*>* svc(std::pair<int, int> *task) {  
  
  for(int i=task->first; i < task->second; i++) {
    
     float avg_proc = 0.0;

    	auto start   = std::chrono::high_resolution_clock::now();
    
    	std::string* path_file = new std::string(path_in_ff_one + files_ff_one[i]);
    
    	cimg_library::CImg<int>* img = new cimg_library::CImg<int>((*path_file).c_str());

               
    	auto elapsed= std::chrono::high_resolution_clock::now() - start;
    	float load = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    	load_time +=load;

      process_image(*(img), *watermark_ff_one, state_ff_one, avg_proc);

      proc_time += avg_proc;

      auto start_s   = std::chrono::high_resolution_clock::now();

    	if (*(img)) {

              	std::string output_file (path_out_ff_one + "DS_marked_" + get_filename(*(path_file)));
                  
              	(*(img)).save(output_file.c_str());
        } 

     	auto elapsed_one = std::chrono::high_resolution_clock::now() - start_s;
     	auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_one).count();

    	 save_time += msec;

    }

    	delete task;
    	return GO_ON;

    }

    float &save_time; 
    float &load_time;
    float &proc_time;
  };


int ff_parallel_one(std::string input_path, std::string output_path, std::string watermark_path, bool c_state, int nw){


  watermark_ff_one = new cimg_library::CImg<int>(watermark_path.c_str());

  path_in_ff_one = input_path;
  path_out_ff_one = output_path;
  state_ff_one = c_state;

  par_ff_one = nw;

 float save_time = 0.0;
 float load_time = 0.0;
 float proc_time = 0.0;

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

  for(int i = 0; i < nw; i++){

      W.push_back(   make_unique<Farm>  (load_time, save_time, proc_time));
  }

  Surce surce;

  ff_Farm<float> First_farm(std::move(W));
  First_farm.add_emitter(surce);
  First_farm.remove_collector();  


    if ( First_farm.run_and_wait_end()<0) {
         error("Running farm ");
         return -1;
    }

  
  std::cout << "##############################################################" << std::endl;
  std::cout << "Processing " << files_ff_one.size() << " images in Parallel mode." << std::endl;

  std::cout << "##############################################################" << std::endl;

  std::cout << "Load time "<< load_time << "msec."<< std::endl; 

  std::cout << "##############################################################" << std::endl;

  std::cout << "Processing time "<< proc_time << "msec."<< std::endl; 
  
  std::cout << "##############################################################" << std::endl;

  std::cout << "Save time: "<< save_time << "msec.\t" << std::endl;

  std::cout << "##############################################################" << std::endl;


return 0;

}
