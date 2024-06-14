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
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // VAO object points to VBOs and EBOs, thus storing the vertex buffer attribute configs made when it is bound
    VertexArray VAO;
    VAO.bind();
    
    VertexBuffer VBO;
    VBO.bind();
    VBO.write_buffer_data(vertices, GL_STATIC_DRAW);

    // define vertex attribute format in VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    Shader shader("assets/shaders/vertex.glsl", "assets/shaders/box_fragment.glsl");

    VAO.bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Texture container_texture("assets/textures/container2.png", GL_RGBA);
    Texture specular_map("assets/textures/container2_specular.png", GL_RGBA);

    shader.use();

    glm::vec3 light_pos(1.2f, 1.0f, 2.0f);
    shader.set("lightPos", light_pos);

    shader.set("material.diffuse", container_texture.index);
    shader.set("material.specular", specular_map.index);

    objects.push_back({VAO, shader});

    VertexArray light_VAO;
    light_VAO.bind();
    VBO.bind(); // reuse container data

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader light_shader("assets/shaders/vertex.glsl", "assets/shaders/light_fragment.glsl");
    light_shader.use();

    glm::mat4 model(1.0f);
    model = glm::translate(model, light_pos);
    model = glm::scale(model, glm::vec3(0.2f));
    light_shader.set("model", model);

    objects.push_back({light_VAO, light_shader});

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
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
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

    for (size_t j = 0; j < objects.size(); j++) {
        const auto& object = objects[j];
        object.vao.bind();
        object.shader.use();

        object.shader.set("view", view);
        object.shader.set("projection", projection);

        if (j == 0) {
            object.shader.set("textureMix", window->state.mix);
            for (int i = 0; i < 10; i++) {
                float angle = 20.0f * i + 50.0f * glfwGetTime();

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

                object.shader.set("model", model);
                object.shader.set("material.shininess", window->state.shininess);
                object.shader.set("light.brightness", window->state.light_brightness);
                object.shader.set("light.ambient", window->state.light_ambient);
                object.shader.set("light.diffuse", window->state.light_diffuse);
                object.shader.set("light.specular", window->state.light_specular);
                object.shader.set("viewPos", window->state.camera_pos);

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        } else {
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
        ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.003f);
        
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

        ImGui::Text("Light Brightness");
        ImGui::SliderFloat("##LightBrightness", &window->state.light_brightness, 0.0f, 10.0f);
        ImGui::Text("Light Ambient");
        ImGui::ColorEdit3("##LightAmbient", &window->state.light_ambient[0]);
        ImGui::Text("Light Diffuse");
        ImGui::ColorEdit3("##LightDiffuse", &window->state.light_diffuse[0]);
        ImGui::Text("Light Specular");
        ImGui::ColorEdit3("##LightSpecular", &window->state.light_specular[0]);

        ImGui::Text("Material Ambient");
        ImGui::ColorEdit3("##Ambient", &window->state.ambient[0]);
        ImGui::Text("Material Diffuse");
        ImGui::ColorEdit3("##Diffuse", &window->state.diffuse[0]);
        ImGui::Text("Material Specular");
        ImGui::ColorEdit3("##Specular", &window->state.specular[0]);
        ImGui::Text("Shininess");
        ImGui::ColorEdit3("##Shininess", &window->state.shininess);

        ImGui::PopItemWidth();
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
