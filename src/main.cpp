#include "window.h"
#include "renderer.h"

int main(void) {
    Window window(1920, 1080, "Graphics Engine");
    Renderer renderer(&window);

    renderer.init();

    while (!window.should_close()) {
        window.process_input();

        renderer.update();
        renderer.render();
        renderer.render_ui();

        glfwSwapBuffers(window.ptr);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
