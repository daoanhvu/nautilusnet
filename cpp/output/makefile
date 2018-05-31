CC=C:\"Program Files (x86)"\"Microsoft Visual Studio"\2017\BuildTools\VC\Tools\MSVC\14.13.26128\bin\HostX64\x64\CL.exe
LINK=C:\"Program Files (x86)"\"Microsoft Visual Studio"\2017\BuildTools\VC\Tools\MSVC\14.13.26128\bin\HostX64\x64\link.exe
CFLAGS=/std:c++14 /I"D:\documents\teaching\datastructure\mst\include" /I"D:\projects\opencv-3.4.1\build\install\include"
FLAGS=/LIBPATH:"D:\projects\opencv-3.4.1\build\install\x64\vc15\lib" /MACHINE:X64 /machine:x64


compile:
	$(CC) /EHsc /c ..\test.cpp $(CFLAGS) /Fo:test.obj
	$(CC) /EHsc /c ..\graph.cpp /D DEBUG $(CFLAGS) /Fo:graph.obj

link:
	$(LINK) $(FLAGS) opencv_core341.lib opencv_imgcodecs341.lib opencv_highgui341.lib graph.obj test.obj /OUT:test.exe