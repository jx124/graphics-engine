#include "renderer.h"

Renderer::Renderer(Window* window) {
    if (window) {
        this->window = window;
    } else {
        std::cerr << "Invalid Window* passed into Renderer constructor" << std::endl;
        std::terminate();
    }
}

void Renderer::init() {
    std::vector<float> vertices = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    std::vector<GLuint> indices = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    
    // VAO object points to VBOs and EBOs, thus storing the vertex buffer attribute configs made when it is bound
    VertexArray VAO;
    VAO.bind();
    
    VertexBuffer VBO;
    VBO.bind();
    VBO.write_buffer_data(vertices, GL_STATIC_DRAW);

    ElementBuffer EBO;
    EBO.bind();
    EBO.write_buffer_data(indices, GL_STATIC_DRAW);

    // define vertex attribute format in VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // this is allowed since glVertexAttribPointer call already binds to VBO
    VBO.unbind();

    // do not unbind EBO while VAO is still active
    VAO.unbind();

    Shader shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

    VAO.bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Texture texture1("assets/textures/container.jpg", GL_RGB);
    Texture texture2("assets/textures/awesomeface.png", GL_RGBA);

    shader.use();
    shader.set("texture1", texture1.index);
    shader.set("texture2", texture2.index);

    objects.push_back({VAO, shader});

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window->ptr, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void Renderer::update() {

}

void Renderer::render() {
    glClearColor(0.2f, 0.6f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 trans = glm::mat4(1.0f);
    // each step is a right multiplication of a transformation matrix, so transformation is applied backwards
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

    for (auto &object : objects) {
        object.shader.set("textureMix", window->state.mix);
        object.shader.set("transform", trans);
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    trans = glm::mat4(1.0f);
    // each step is a right multiplication of a transformation matrix, so transformation is applied backwards
    trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
    trans = glm::scale(trans, glm::sin((float)glfwGetTime() * glm::vec3(1.0f)));

    for (auto &object : objects) {
        object.shader.set("textureMix", window->state.mix);
        object.shader.set("transform", trans);
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::render_ui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    static float prev_time = 0.0f;
    float curr_time = glfwGetTime();
    float frame_time = curr_time - prev_time;
    float fps = 1.0f / frame_time;
    prev_time = curr_time;

    ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

    if (window->state.show_debug) {
        ImGui::Begin("Debug Menu", &window->state.show_debug);
        ImGui::Text("Frame Time: %.1f ms (%.1f FPS)", frame_time * 1000.0f, fps);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
