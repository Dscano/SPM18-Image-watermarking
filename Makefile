CXX      = g++
CXXFLAGS = -O3 -std=c++11 -I lib/ -D cimg_display=0
LDFLAGS  = -pthread
OBJS     = run_watermarking

all:	$(OBJS)

run_watermarking.cpp:	
		            util.cpp  sequential.cpp  parallel.cpp ff_parallel.cpp 



clean:	
	rm -f $(OBJS)
