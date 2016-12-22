CC=g++
AR=ar
CFLAGS=-I./
SRC=mainapp.cpp nautilusnet.cpp

all: mainapp.exe

mainapp.exe:
	$(CC) $(SRC) $(CFLAGS) -DDEBUG -o mainapp.exe
	
testm.exe:
	$(CC) testm.cpp $(CFLAGS) -DDEBUG -o testm.exe