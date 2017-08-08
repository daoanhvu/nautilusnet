PLATFORM = LINUX
CXX = g++
SOURCE = cap3d.cpp
#For Linux
CPFLAGS =-std=c++11 -g -I. -I./include -I/usr/include \
								-I/home/davu/projects/glm-0.9.8.4

#For MacOS
# CPFLAGS = -I. -I/usr/include \
# 								-I/usr/local/Cellar/glew/2.0.0/include \
# 								-I/usr/local/Cellar/glfw/3.2.1/include \
# 								-I/Users/davu/Documents/src/glm

CXXFLAGS = -std=c++11 -Wall -g
BUILD = ./build/camera.o \
				./build/cap3d.o \
				./build/glrenderer.o \
				./build/model3d.o \
				./build/plymodel3d.o \
				./build/pcdmodel3d.o \
				./build/vbo.o \
				./build/reader.o \
				./build/pcdreader.o \
				./build/plyreader.o \
				./build/utils.o \
				./build/shader.o \
				./build/viewer.o \
				./build/lexer_parser.o \
				./build/image_processor.o

#For Linux
OPENCV_LIB_DIR = -L./linuxlib -L/usr/lib -L/usr/local/lib

#For MacOS
# OPENCV_LIB_DIR = -L/usr/lib -L/usr/local/lib

LIBS = -lopencv_core -lopencv_calib3d -lopencv_highgui -lopencv_features2d \
								-lopencv_ml -lopencv_imgcodecs -lopencv_imgproc
#   stackoverflow.com/questions/33747874/glfw3-error-sdo-missing-from-command-line
# http://stackoverflow.com/questions/17768008/how-to-build-install-glfw-3-and-use-it-in-a-linux-project
$(BUILD): camera.cpp cap3d.cpp utils.cpp
	$(CXX) -c -o ./build/camera.o $(CPFLAGS) camera.cpp
	$(CXX) -c -o ./build/lexer_parser.o $(CPFLAGS) lexer_parser.cpp
	$(CXX) -c -o ./build/utils.o $(CPFLAGS) utils.cpp
	$(CXX) -c -o ./build/lexer_parser.o $(CPFLAGS) lexer_parser.cpp
	$(CXX) -c -o ./build/image_processor.o $(CPFLAGS) image_processor.cpp
	$(CXX) -c -o ./build/shader.o $(CPFLAGS) shader.cpp
	$(CXX) -c -o ./build/reader.o $(CPFLAGS) reader/reader.cpp
	$(CXX) -c -o ./build/plyreader.o $(CPFLAGS) reader/plyreader.cpp
	$(CXX) -c -o ./build/pcdreader.o $(CPFLAGS) reader/pcdreader.cpp
	$(CXX) -c -o ./build/model3d.o $(CPFLAGS) reader/model3d.cpp
	$(CXX) -c -o ./build/plymodel3d.o $(CPFLAGS) reader/plymodel3d.cpp
	$(CXX) -c -o ./build/pcdmodel3d.o $(CPFLAGS) reader/pcdmodel3d.cpp
	$(CXX) -c -o ./build/vbo.o $(CPFLAGS) vbo.cpp
	$(CXX) -c -o ./build/glrenderer.o $(CPFLAGS) glrenderer.cpp
	$(CXX) -c -o ./build/image_processor.o $(CPFLAGS) image_processor.cpp
	$(CXX) -c -o ./build/viewer.o $(CPFLAGS) viewer.cpp
	$(CXX) -c -o ./build/cap3d.o $(CPFLAGS) cap3d.cpp

linux: $(BUILD)
	$(CXX) -v -o ./build/cap3d $(BUILD) $(OPENCV_LIB_DIR) $(LIBS) -lGL -lGLU -lGLEW -lglut -lglfw3 -lX11 -lXxf86vm -lXrandr -lXi -lpthread -ldl -lXinerama -lXcursor

mac: $(BUILD)
	$(CXX) -v -o ./build/cap3d $(BUILD) $(OPENCV_LIB_DIR) $(LIBS) -framework OpenGL -framework GLUT -lGLEW -lglfw.3

clean:
	rm ./build/*
	cp *.shader ./build/

test: ./build/cap3d
	./build/cap3d data/simple_model_1.ply

pcd: ./build/cap3d
	./build/cap3d data/apple_1_4_1.pcd

# pcd: ./build/cap3d
# 	./build/cap3d data/NP1_0.pcd

teddy: ./build/cap3d
	./build/cap3d data/teddy1.ply teddy 0
