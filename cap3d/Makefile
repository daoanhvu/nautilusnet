PLATFORM=LINUX
CXX=g++
CPFLAGS=-I. -I/usr/include -I/usr/local/include
SOURCE=
CXXFLAGS=-std=c++17 -Wall -g
LIBS=-lOpenGL -lGL -lglfw -lGLEW -lglut

test.o: test.cpp
	$(CXX) -o ./build/test.o $(CXXFLAGS) $(CPFLAGS) test.cpp

rot: rot_quat.cpp
	$(CXX) -o ./build/rot.o $(CXXFLAGS) $(CPFLAGS) rot_quat.cpp $(LIBS)

rot1: rot_3d.cpp
	$(CXX) -o ./build/rot1.o $(CXXFLAGS) $(CPFLAGS) rot_3d.cpp $(LIBS)

clean:
	rm -f ./build/*