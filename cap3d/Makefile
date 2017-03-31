CXX = g++
GM_SRC = gm/gm.hpp gm/quat.hpp gm/vec3.hpp gm/vec4.hpp gm/fmat4.hpp
SOURCE = cap3d.cpp
CPFLAGS =-I. -I./include -I./gm -I/usr/include -I/media/davu/data/projects/glm-0.9.8.4
CXXFLAGS = -std=c++11 -Wall -g
BUILD = ./build/camera.o ./build/cap3d.o
GLBUILD = ./build/glrenderer.o
OPENCV_LIB_DIR = -L./linuxlib -L/usr/lib -L/usr/local/lib
LIBS = -lopencv_core -lopencv_calib3d -lopencv_highgui -lopencv_features2d -lopencv_ml -lopencv_imgcodecs -lopencv_imgproc
#   stackoverflow.com/questions/33747874/glfw3-error-sdo-missing-from-command-line
# http://stackoverflow.com/questions/17768008/how-to-build-install-glfw-3-and-use-it-in-a-linux-project
$(BUILD): camera.cpp cap3d.cpp
	$(CXX) -c -o ./build/camera.o $(CPFLAGS) camera.cpp
	$(CXX) -c -o ./build/cap3d.o $(CPFLAGS) cap3d.cpp
	$(CXX) -c -o ./build/glrenderer.o $(CPFLAGS) glrenderer.cpp

all: $(BUILD)
	$(CXX) -o ./build/cap3d $(BUILD) $(OPENCV_LIB_DIR) $(LIBS) -lGL -lGLU -lGLEW -lglut -lglfw3 -lX11 -lXxf86vm -lXrandr -lXi -lpthread -ldl -lXinerama -lXcursor

clean:
	rm ./build/*
