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
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create a plane model and its associated shader
    Model plane_model;
    Shader plane_shader("assets/shaders/model_vertex.glsl", "assets/shaders/model_fragment.glsl");

    // Generate the mesh data for the plane, scaling its texture coordinates to tile the texture
    MeshData plane_mesh = Mesh::generate_plane_mesh();
    for (auto& vertex : plane_mesh.vertices) {
        vertex.tex_coords *= 2.0f;
    }

    // Create a texture and add it to the mesh
    Texture metal_texture("assets/textures/metal.png");
    metal_texture.set_type(Texture::Type::Diffuse);
    plane_mesh.textures.push_back(metal_texture);

    // Add the mesh to the model
    plane_model.add_mesh(Mesh(plane_mesh));

    // Add the model and shader to the list of models to be rendered
    this->model_objects.push_back({plane_shader, plane_model});

    // Add the transforms to apply to the plane model
    glm::mat4 plane_transform(1.0f);
    plane_transform = glm::translate(plane_transform, glm::vec3(0.0f, -0.5f, 0.0f));
    plane_transform = glm::scale(plane_transform, glm::vec3(5.0f));
    this->transforms.push_back(std::move(plane_transform));

    // container model
    Shader container_shader("assets/shaders/vertex.glsl", "assets/shaders/box_fragment.glsl");

    Texture container_texture("assets/textures/container2.png");
    Texture specular_map("assets/textures/container2_specular.png");

    container_shader.use();
    container_shader.set("material.diffuse", container_texture.unit);
    container_shader.set("material.specular", specular_map.unit);

    Model container_model;
    MeshData container_mesh = Mesh::generate_cube_mesh();
    container_model.add_mesh(Mesh(container_mesh));

    this->model_objects.push_back({container_shader, container_model});

    glm::mat4 container_transform(1.0f);

    this->transforms.push_back(std::move(container_transform));

    // Create a cube model and its associated shader
    Model cube_model;
    Shader cube_shader("assets/shaders/model_vertex.glsl", "assets/shaders/model_fragment.glsl");

    // Generate the mesh data for the cube
    MeshData cube_mesh = Mesh::generate_cube_mesh();

    // Create a texture and add it to the mesh
    Texture marble_texture("assets/textures/marble.jpg");
    marble_texture.set_type(Texture::Type::Diffuse);
    cube_mesh.textures.push_back(marble_texture);

    // Add the mesh to the model
    cube_model.add_mesh(Mesh(cube_mesh));

    // Add the model and shader to the list of models to be rendered
    this->model_objects.push_back({cube_shader, cube_model});

    // Add the transforms to apply to the cube model
    glm::mat4 cube1_transform(1.0f);
    cube1_transform = glm::translate(cube1_transform, glm::vec3(-1.0f, 0.0f, -1.0f));
    this->transforms.push_back(cube1_transform);

    // Add the second cube
    this->model_objects.push_back({cube_shader, cube_model});

    // Add the transforms to apply to the second cube model
    glm::mat4 cube2_transform(1.0f);
    cube2_transform = glm::translate(cube2_transform, glm::vec3(2.0f, 0.0f, 0.0f));
    this->transforms.push_back(cube2_transform);

    // Add upscaled cubes for stencil drawing
    Shader outline_shader("assets/shaders/model_vertex.glsl", "assets/shaders/light_fragment.glsl");
    cube1_transform = glm::scale(cube1_transform, glm::vec3(1.01f));
    cube2_transform = glm::scale(cube2_transform, glm::vec3(1.01f));

    this->model_objects.push_back({outline_shader, cube_model});
    this->model_objects.push_back({outline_shader, cube_model});
    this->transforms.push_back(cube1_transform);
    this->transforms.push_back(cube2_transform);

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

    constexpr glm::vec3 pointLightPositions[] = {
       glm::vec3( 0.7f,  0.2f,  2.0f),
       glm::vec3( 2.3f, -3.3f, -4.0f),
       glm::vec3(-4.0f,  2.0f, -12.0f),
       glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    auto& object = this->model_objects[1];
    object.shader.use();

    object.shader.set("material.shininess", window->state.shininess);

    // directional lights
    glm::vec3 light_dir(-0.2f, -1.0f, -0.3f);
    object.shader.set("dirLight.direction", light_dir);
    object.shader.set("dirLight.ambient", window->state.dirlight_ambient);
    object.shader.set("dirLight.diffuse", window->state.dirlight_diffuse);
    object.shader.set("dirLight.specular", window->state.dirlight_specular);
    object.shader.set("viewPos", window->state.camera_pos);

    // point lights
    for (int i = 0; i < 4; i++) {
        std::string pointLight = "pointLights[" + std::to_string(i) + "]";
        object.shader.set(pointLight + ".position", pointLightPositions[i]);
        object.shader.set(pointLight + ".ambient", window->state.pointlight_ambient);
        object.shader.set(pointLight + ".diffuse", window->state.pointlight_diffuse);
        object.shader.set(pointLight + ".specular", window->state.pointlight_specular);
        object.shader.set(pointLight + ".constant", 1.0f);
        object.shader.set(pointLight + ".linear", 0.09f);
        object.shader.set(pointLight + ".quadratic", 0.032f);
    }

    // spotlight
    object.shader.set("spotLight.position", window->state.camera_pos);
    object.shader.set("spotLight.direction", window->state.camera_front);
    object.shader.set("spotLight.cutoff", glm::cos(glm::radians(window->state.cutoff)));
    object.shader.set("spotLight.outerCutoff", glm::cos(glm::radians(window->state.outer_cutoff)));
    object.shader.set("spotLight.ambient", window->state.spotlight_ambient);
    object.shader.set("spotLight.diffuse", window->state.spotlight_diffuse);
    object.shader.set("spotLight.specular", window->state.spotlight_specular);
    object.shader.set("spotLight.constant", 1.0f);
    object.shader.set("spotLight.linear", 0.09f);
    object.shader.set("spotLight.quadratic", 0.032f);
}

void Renderer::render() {
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Replace stencil buffer value with 1 (stencil func ref value) when the stencil test passes
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // have fragments always pass the stencil test
    glStencilMask(0x00); // disable writing to stencil buffer

    glm::mat4 view = glm::lookAt(window->state.camera_pos,
                                 window->state.camera_pos + window->state.camera_front,
                                 window->state.camera_up);

    glm::mat4 projection;
    float aspect_ratio = static_cast<float>(window->width) / window->height;
    projection = glm::perspective(glm::radians(window->state.fov), aspect_ratio, 0.1f, 100.0f);

    for (size_t i = 0; i < this->model_objects.size(); i++) {
        auto& object = this->model_objects[i];

        // Write to stencil buffer for the 2 marble cubes
        if (i == 2) {
            glStencilMask(0xFF); // enable writing to stencil buffer
        }

        // Draw outlines of the 2 marble cubes
        if (i == 4) {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // only pass fragments not overlapping with cubes
            glStencilMask(0x00); // disable writing to stencil buffer
            glDisable(GL_DEPTH_TEST); // always draw outline regardless of depth
        }

        object.shader.use();
        object.shader.set("model", this->transforms[i]);
        object.shader.set("view", view);
        object.shader.set("projection", projection);

        object.model.draw(object.shader);
    }
    glStencilMask(0xFF); // enable writing to stencil buffer so it can be cleared
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
        ImGui::Text("Camera Position: (%.3f, %.3f, %.3f)",
                    window->state.camera_pos.x,
                    window->state.camera_pos.y,
                    window->state.camera_pos.z);

        ImGui::SeparatorText("Settings");
        ImGui::Text("Camera Speed");
        ImGui::SliderFloat("##CameraSpeed", &window->state.camera_speed, 0.1f, 10.0f, "%.1f");
        ImGui::Text("Camera Sensitivity");
        ImGui::SliderFloat("##CameraSensitivity", &window->state.camera_sensitivity, 0.01f, 1.0f, "%.2f");

        ImGui::Text("Dir Light Ambient");
        ImGui::ColorEdit3("##DirLightAmbient", &window->state.dirlight_ambient[0]);
        ImGui::Text("Dir Light Diffuse");
        ImGui::ColorEdit3("##DirLightDiffuse", &window->state.dirlight_diffuse[0]);
        ImGui::Text("Dir Light Specular");
        ImGui::ColorEdit3("##DirLightSpecular", &window->state.dirlight_specular[0]);

        ImGui::Text("Point Light Ambient");
        ImGui::ColorEdit3("##PointLightAmbient", &window->state.pointlight_ambient[0]);
        ImGui::Text("Point Light Diffuse");
        ImGui::ColorEdit3("##PointLightDiffuse", &window->state.pointlight_diffuse[0]);
        ImGui::Text("Point Light Specular");
        ImGui::ColorEdit3("##PointLightSpecular", &window->state.pointlight_specular[0]);

        ImGui::Text("Spot Light Ambient");
        ImGui::ColorEdit3("##SpotLightAmbient", &window->state.spotlight_ambient[0]);
        ImGui::Text("Spot Light Diffuse");
        ImGui::ColorEdit3("##SpotLightDiffuse", &window->state.spotlight_diffuse[0]);
        ImGui::Text("Spot Light Specular");
        ImGui::ColorEdit3("##SpotLightSpecular", &window->state.spotlight_specular[0]);
        ImGui::Text("Spot Light Cutoff");
        ImGui::SliderFloat("##Cutoff", &window->state.cutoff, 0.1f, 90.0f, "%.1f");
        ImGui::Text("Spot Light Outer Cutoff");
        ImGui::SliderFloat("##OuterCutoff", &window->state.outer_cutoff, 0.1f, 90.0f, "%.1f");

        ImGui::Text("Material Shininess");
        ImGui::SliderFloat("##Shininess", &window->state.shininess, 0.1f, 256.0f, "%.1f");

        ImGui::PopItemWidth();
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
