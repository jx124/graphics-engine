#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "window.h"

int main() {
    try {
        Window window = Window(1200, 800, "Graphics Engine");
        window.createWindow();
        window.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
