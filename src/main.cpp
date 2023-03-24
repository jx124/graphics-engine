#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "window.h"

int main() {
    Window window = Window(800, 600, "Graphics Engine");
    bool success = window.createWindow();
    if (!success) {
        return 1;
    }
    window.run();
    int i = 23;
    i <<= 32;
}