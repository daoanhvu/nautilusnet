#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

int main(int argc, char** args) {
    
    if(!glfwInit()) {
        cout << "Failed to initialize GLFW!\n";
        return -1;
    }

    glfwTerminate();

    return 0;
}