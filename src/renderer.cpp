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
    // cube mesh
    std::vector<float> vertices = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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

    // define vertex attribute format in VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window->ptr, true);
    ImGui_ImplOpenGL3_Init();
}

void Renderer::update() {
    float prev_time = window->state.curr_time;
    float curr_time = glfwGetTime();
    float delta_time = curr_time - prev_time;

    window->state.curr_time = curr_time;
    window->state.prev_time = prev_time;
    window->state.delta_time = delta_time;
}

void Renderer::render() {
    glClearColor(0.2f, 0.6f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    glm::mat4 view = glm::lookAt(window->state.camera_pos,
                                 window->state.camera_pos + window->state.camera_front,
                                 window->state.camera_up);

    glm::mat4 projection;
    float aspect_ratio = static_cast<float>(window->width) / window->height;
    projection = glm::perspective(glm::radians(window->state.fov), aspect_ratio, 0.1f, 100.0f);

    for (auto &object : objects) {
        object.vao.bind();
        object.shader.set("textureMix", window->state.mix);
        object.shader.set("view", view);
        object.shader.set("projection", projection);

        for (int i = 0; i < 10; i++) {
            float angle = 20.0f * i + 50.0f * glfwGetTime();

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

            object.shader.set("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}

void Renderer::render_ui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    float fps = 1.0f / window->state.delta_time;

    ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

    if (window->state.show_debug) {
        ImGui::ShowDemoWindow();

        ImGui::Begin("Debug Menu", &window->state.show_debug);
        ImGui::Text("Frame Time: %.1f ms (%.1f FPS)", window->state.delta_time * 1000.0f, fps);
        ImGui::Text("Last mouse position: (%d, %d)", window->state.last_x, window->state.last_y);
        ImGui::Text("Pitch: %.1f, Yaw: %.1f", window->state.pitch, window->state.yaw);
        ImGui::Text("FOV: %.1f", window->state.fov);
        ImGui::Text("Camera Direction: (%.3f, %.3f, %.3f)",
                    window->state.camera_front.x,
                    window->state.camera_front.y,
                    window->state.camera_front.z);

        ImGui::SeparatorText("Settings");
        ImGui::Text("Camera Speed");
        ImGui::SliderFloat("##CameraSpeed", &window->state.camera_speed, 0.1f, 10.0f, "%.1f");
        ImGui::Text("Camera Sensitivity");
        ImGui::SliderFloat("##CameraSensitivity", &window->state.camera_sensitivity, 0.01f, 1.0f, "%.2f");

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
