
void sequential(   std::string input_path, std::string output_path, std::string watermark_path, bool c_state){
    
    cimg_library::CImg<int> img; // "structure" for manipulate the immage
    cimg_library::CImg<int> watermark;

    float avg_load = 0.0, avg_proc = 0.0, avg_save = 0.0, read= 0.0;
    
    char *out_dir = new char[output_path.length() + 1];

    std::strcpy(out_dir, output_path.c_str());

    struct stat st;
     
     //creating output directory if it doesn't exist.
    if(stat(out_dir,&st) != 0){

        int status = mkdir(out_dir, S_IRWXU);

           if(status!=0) {

             std::cout << "Error mkdir" << status << std::endl;
        }

        std::cout << "Create Directory" << output_path <<std::endl;
    }


    std::vector<std::string> files;

    auto start_r = std::chrono::high_resolution_clock::now();
    //loading input file paths in a stringvec 
    read_dir(input_path, files); 
    auto elapsed_r= std::chrono::high_resolution_clock::now() - start_r;
    float msec_r   = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_r).count();   
    read += (float) msec_r;

    std::cout << "Processing" << files.size() << "images in Sequential mode." << std::endl;

    //loading watermark
    watermark.load(watermark_path.c_str());

    for ( int i = 0; i<files.size(); i++) {

        std::string f_name(input_path + files[i]);
        std::string outfile(output_path + "DS_marked_" + files[i]); // making the file name of the output file
       
       //Loading
        auto start_t0 = std::chrono::high_resolution_clock::now();

        img.load(f_name.c_str()); //Load image from a file.

        auto elapsed_t0 = std::chrono::high_resolution_clock::now() - start_t0;
        float msec_t0   = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_t0).count();   
        avg_load += (float) msec_t0;

        //Processing
        auto start_t1 = std::chrono::high_resolution_clock::now();

        process_seq(img, watermark, c_state);

        auto elapsed_t1 = std::chrono::high_resolution_clock::now() - start_t1;
        float msec_t1   = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_t1).count();   
        avg_proc += (float) msec_t1;

        //Saving
        auto star_t2  = std::chrono::high_resolution_clock::now();

        img.save(outfile.c_str());  

        auto elapsed_t2 = std::chrono::high_resolution_clock::now() - star_t2;
        float  msec_t2  = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_t2).count();   
        avg_save += (float) msec_t2;
    }
    std::cout << "Read time:\t"<< read/files.size() << "msec.\t"<< read   <<  std::endl;
    std::cout << "Load time:\t"<< avg_load/files.size() << "msec.\t"   <<  std::endl;
    std::cout << "Proc time:\t"<< avg_proc/files.size() << "msec.\t\t" << std::endl;
    std::cout << "Save time:\t"<< avg_save/files.size() << "msec.\t"   << std::endl;

}