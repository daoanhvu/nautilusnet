CC=g++
AR=ar
CFLAGS=-I./
SRC=mainapp.cpp nautilusnet.cpp

clean_files:
	rm testm.exe
	rm mainapp.exe
	
all: mainapp.exe

mainapp.exe:
	$(CC) $(SRC) $(CFLAGS) -g -DDEBUG -o mainapp.exe
	
testm.exe:
	$(CC) testm.cpp $(CFLAGS) -DDEBUG -o testm.exe