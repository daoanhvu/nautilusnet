CC:=g++
GM_SRC:=gm/gm.hpp gm/quat.hpp gm/vec3.hpp gm/vec4.hpp gm/fmat4.hpp
SOURCE:= cap3d.cpp
CPFLAGS:=-I. -I./gm -I/usr/include -I/media/davu/data/projects/glm-0.9.8.4
BUILD := ./build/camera.o ./build/cap3d.o
GLBUILD := ./build/glrenderer.o
OPENCV_LIB_DIR := -L/usr/lib -L/usr/local/lib
LIBS := -lopencv_core -lopencv_calib3d -lopencv_highgui -lopencv_features2d -lopencv_ml -lopencv_imgcodecs -lopencv_imgproc

$(BUILD): camera.cpp cap3d.cpp
	$(CC) -c -o ./build/camera.o $(CPFLAGS) camera.cpp
	$(CC) -c -o ./build/cap3d.o $(CPFLAGS) cap3d.cpp
	#$(CC) -c -o ./build/glrenderer.o $(CPFLAGS) glrenderer.cpp -lGL -lGLU -lglut -lGLEW

all: $(BUILD)
	$(CC) -o ./build/cap3d $(BUILD) $(OPENCV_LIB_DIR) $(LIBS)

clean:
	rm ./build/*
