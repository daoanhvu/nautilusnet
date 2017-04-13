PLATFORM = APPLE
CXX = g++
SOURCE = myyolo.cpp
CPFLAGS =-I. -I/usr/local/include
CXXFLAGS = -std=c++11 -Wall -g
OPENCV_LIB_DIR = -L/usr/local/lib
LIBS = -lopencv_core -lopencv_calib3d -lopencv_highgui -lopencv_features2d -lopencv_ml -lopencv_imgcodecs -lopencv_imgproc
#   stackoverflow.com/questions/33747874/glfw3-error-sdo-missing-from-command-line
# http://stackoverflow.com/questions/17768008/how-to-build-install-glfw-3-and-use-it-in-a-linux-project
$(BUILD): camera.cpp cap3d.cpp
	$(CXX) -c -o ./build/camera.o $(CPFLAGS) camera.cpp
	$(CXX) -c -o ./build/plyfile.o $(CPFLAGS) plyfile.cpp
	$(CXX) -c -o ./build/glrenderer.o $(CPFLAGS) glrenderer.cpp
	$(CXX) -c -o ./build/cap3d.o $(CPFLAGS) cap3d.cpp

mac: $(BUILD)
	$(CXX) -o ./build/myyolo myyolo.cpp $(OPENCV_LIB_DIR) $(LIBS)

clean:
	rm ./build/*
	cp *.shader ./build/
