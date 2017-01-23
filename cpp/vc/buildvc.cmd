del nautilusnet.obj NautilusMind.obj
cl /c /Oy /EHsc /D"DEBUG" /nologo ^
    /I"..\" /I"D:\programs\opencv\build\include\" ^
    imagehandler.cpp ^
    ../nautilusnet.cpp ^
    NautilusMind.cpp 

link /SUBSYSTEM:Windows ^
    nautilusnet.obj ^
    NautilusMind.obj ^
    "kernel32.lib" ^
    "user32.lib" ^
    "gdi32.lib" ^
    "winspool.lib" ^
    "comdlg32.lib" ^
    "advapi32.lib" ^
    "shell32.lib" ^
    "ole32.lib" ^
    /LIBPATH:"D:\programs\opencv\build\x64\vc11\lib" ^
    "opencv_core2413.lib" ^
    /out:NautilusMind.exe