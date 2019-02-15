PLATFORM = LINUX
CXX = g++
SOURCE = main.cpp
#For Linux
CPFLAGS +=-std=c++11 -I. -I/usr/include -I/home/vdao/programs/opencv/3.4/include

#For Linux
OPENCV_LIB_DIR = -L/usr/lib -L/usr/local/lib -L/home/vdao/programs/opencv/3.4/lib
BUILD = ./build/graph.o

LIBS += -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lXi -lpthread -ldl -lXinerama -lXcursor

$(BUILD): graph.cpp
	$(CXX) -c -o ./build/graph.o $(CPFLAGS) $<

all: main.cpp $(BUILD)
	$(CXX) main.cpp $(BUILD) $(CPFLAGS) $(OPENCV_LIB_DIR) $(LIBS) -o ./build/selective
	# $(CXX) `pkg-config --cflags --libs /home/vdao/programs/opencv/3.4/lib/pkgconfig/opencv.pc` main.cpp -o ./build/selective
	# g++ `pkg-config --cflags --libs /home/vdao/programs/opencv/3.4/lib/pkgconfig/opencv.pc` main.cpp -o build/selective