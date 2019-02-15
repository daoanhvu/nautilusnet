CXX = clang++
CXXFLAGS += -std=c++0x -L/usr/lib/x86_64-linux-gnu/ -I/usr/include/
# CXXFLAGS += -std=c++0x
LIBS += -lOpenCL

SRC = main.cpp

all:	main

main:	$(SRC)
	$(CXX) $< $(CXXFLAGS) $(LIBS) -o $@

clean:
	rm main