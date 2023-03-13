#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "window.cpp"

int main() {
    Window window = Window(800, 600, "Graphics Engine");
    bool success = window.createWindow();
    if (success) {
        window.run();
    }
}