#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "window.h"

int main() {
    Window window = Window(800, 600, "Graphics Engine");
    try {
        window.createWindow();
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    window.run();
}
