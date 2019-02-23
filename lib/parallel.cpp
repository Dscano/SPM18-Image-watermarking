

queue_t<std::pair<std::string*, cimg_library::CImg<int>* >* > to_stage1;
queue_t<std::pair<std::string*, cimg_library::CImg<int>* >* > to_stage2;

float proc_times;
float save_time= 0.0;
float load_time= 0.0;
std::vector<int> n_images_vect;
std::vector<std::string> files_par;

void Stage1( std::string input_path, int nw, int start_gap, int stop_gap){

	auto start   = std::chrono::high_resolution_clock::now();

	std::vector<std::pair<std::string*, cimg_library::CImg<int>*>*> v_pair;

  if(stop_gap <= files_par.size() && start_gap <= files_par.size()){

    for(int i=start_gap; i<stop_gap; i++) {

    	std::string *file_n = new std::string (input_path + files_par[i]);

    	cimg_library::CImg<int>* img = new cimg_library::CImg<int> (file_n->c_str());

    	std::pair<std::string*, cimg_library::CImg<int>*>* p = 
            	new std::pair<std::string*, cimg_library::CImg<int>*>(file_n, img);

    	v_pair.push_back(p);

    }}

    for (int a=0; a<v_pair.size(); a++) to_stage1.push(v_pair[a]);

    
    to_stage1.push(NULL);

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto load  = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count(); 
    load_time +=load;

    return;
}

void Stage2(std::string watermark_path, bool c_state, int nw, int nw_stage_1) { 

  float proc_time = 0.0;
  int n = nw;
  int n_1 = nw_stage_1;

	cimg_library::CImg<> watermark;
	watermark.load(watermark_path.c_str());
  
	while(true) {

		std::pair<std::string*, cimg_library::CImg<int>*>* pair= to_stage1.pop();

    if(pair == NULL && n_1 == 1)  { 
              for(int i=0; i<nw; i++) to_stage2.push(pair);
              break;
        }

		if( pair == NULL) {
      n_1--; 
		}

		else{ 	

				std::pair<std::string*, cimg_library::CImg<int>*>* p =  
							new std::pair<std::string*, cimg_library::CImg<int>*>(pair->first, pair->second);

				process_image(*(p->second), watermark, c_state, proc_time);
	  		to_stage2.push(p);

	  		}	
	}

  proc_times+= proc_time;

	return;
}

void Worker(std::string output_path){

  
    std::string outfile;

		while(true) {

			std::pair<std::string*, cimg_library::CImg<int>*>* pair= to_stage2.pop();

			if(pair == NULL)  {

				break;
			} 

			else {	

          auto start   = std::chrono::high_resolution_clock::now();
				
					std::string* outfile = new std::string(output_path + "DS_marked_" + get_filename(*(pair->first)));

  					(*(pair->second)).save(outfile->c_str()); 

  				delete outfile;
  				delete pair;  
  				auto elapsed = std::chrono::high_resolution_clock::now() - start;
   				auto save  = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count(); 
   					
          save_time +=save;

				}
			}

		return;
	}

void parallel(std::string input_path, std::string output_path, std::string watermark_path, bool c_state, int nw) {
  
  std::vector<std::thread> threads;


  char *out_dir = new char[output_path.length() + 1];

  std::strcpy(out_dir, output_path.c_str());

    struct stat st;
     
     //creating output directory if it doesn't exist.
    if(stat(out_dir,&st) != 0){

        int status = mkdir(out_dir, S_IRWXU);

           if(status!=0) {

             std::cout << "Error mkdir "<< status << std::endl;
        }

        std::cout << "Create Directory " << output_path <<std::endl;
    }

  read_dir(input_path, files_par); 

  std::cout << "##############################################################" << std::endl;
  std::cout << "Processing " << files_par.size() << " images in Parallel mode." << std::endl;

  float ratio = 0.0;
  //ratio evaluatet between sequential Stage 1 and Stage 3
  if(input_path.compare("image_800x600/ ")) ratio= 1.457087519;
  if(input_path.compare("image_1600x1200/")) ratio= 1.650527912;
  

  
  int nw_stage_1= std::round(nw/ratio);


  int gap = std::round(double(files_par.size())/nw_stage_1);
  int start_gap = 0;
  int stop_gap = gap;

  // create threads
  for(int d=0; d<nw_stage_1; d++){

  threads.push_back(std::thread(Stage1, input_path, nw_stage_1, start_gap, stop_gap));


  start_gap += gap;
  stop_gap += gap;

  if(stop_gap > files_par.size()) stop_gap=files_par.size();
  }

  threads.push_back(std::thread(Stage2, watermark_path, c_state, nw, nw_stage_1));

  for(int i=0; i<nw; i++){
  	
    threads.push_back(std::thread(Worker, output_path));

  }
 
  
  for(auto &th : threads) th.join();

  std::cout << "##############################################################" << std::endl;

  std::cout << "Load time "<< load_time << "msec."<< std::endl; 

  std::cout << "##############################################################" << std::endl;

  std::cout << "Processing time "<< proc_times << "msec."<< std::endl; 
  
  std::cout << "##############################################################" << std::endl;

  std::cout << "Save time: "<< save_time << "msec.\t" << std::endl;

  std::cout << "##############################################################" << std::endl;
}
