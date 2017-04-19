PLATFORM = APPLE
CXX = g++
SOURCE = cap3d.cpp
#CPFLAGS =-I. -I./include -I./gm -I/usr/include -I/media/davu/data/projects/glm-0.9.8.4
CPFLAGS =-I. -I./include -I/usr/include \
								-I/usr/local/Cellar/glew/2.0.0/include \
								-I/usr/local/Cellar/glfw/3.2.1/include \
								-I/Users/davu/Documents/src/glm
CXXFLAGS = -std=c++11 -Wall -g
BUILD = ./build/camera.o ./build/cap3d.o ./build/glrenderer.o ./build/utils.o ./build/plyfile.o
GLBUILD = ./build/glrenderer.o
OPENCV_LIB_DIR = -L/usr/local/Cellar/glew/2.0.0/lib -L/usr/local/Cellar/glfw/3.2.1/lib -L/usr/lib -L/usr/local/lib
LIBS = -lopencv_core -lopencv_calib3d -lopencv_highgui -lopencv_features2d \
								-lopencv_ml -lopencv_imgcodecs -lopencv_imgproc
#   stackoverflow.com/questions/33747874/glfw3-error-sdo-missing-from-command-line
# http://stackoverflow.com/questions/17768008/how-to-build-install-glfw-3-and-use-it-in-a-linux-project
$(BUILD): camera.cpp cap3d.cpp
	$(CXX) -c -o ./build/camera.o $(CPFLAGS) camera.cpp
	$(CXX) -c -o ./build/utils.o $(CPFLAGS) utils.cpp
	$(CXX) -c -o ./build/plyfile.o $(CPFLAGS) plyfile.cpp
	$(CXX) -c -o ./build/glrenderer.o $(CPFLAGS) glrenderer.cpp
	$(CXX) -c -o ./build/cap3d.o $(CPFLAGS) cap3d.cpp

mac: $(BUILD)
	$(CXX) -v -o ./build/cap3d $(BUILD) $(OPENCV_LIB_DIR) $(LIBS) -framework OpenGL -framework GLUT -lGLEW -lglfw.3

clean:
	rm ./build/*
	cp *.shader ./build/
