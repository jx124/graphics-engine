#include "renderer.h"

Renderer::Renderer(Window* window) {
    if (window) {
        this->window = window;
    } else {
        std::cerr << "Invalid Window* passed into Renderer constructor" << std::endl;
        std::terminate();
    }
}

const std::vector<glm::vec3> window_positions = {
    glm::vec3(-1.5f,  0.0f, -0.48f),
    glm::vec3( 1.5f,  0.0f,  0.51f),
    glm::vec3( 0.0f,  0.0f,  0.7f),
    glm::vec3(-0.3f,  0.0f, -2.3f),
    glm::vec3( 0.5f,  0.0f, -0.6f),
};

void Renderer::init() {
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // SHADERS

    Shader model_shader("assets/shaders/model_vertex.glsl", "assets/shaders/model_fragment.glsl");
    this->shaders.push_back(std::move(model_shader));

    Shader container_shader("assets/shaders/vertex.glsl", "assets/shaders/box_fragment.glsl");

    Texture container_texture("assets/textures/container2.png");
    Texture specular_map("assets/textures/container2_specular.png");

    container_shader.use();
    container_shader.set("material.diffuse", container_texture.unit);
    container_shader.set("material.specular", specular_map.unit);
    this->shaders.push_back(std::move(container_shader));

    Shader outline_shader("assets/shaders/model_vertex.glsl", "assets/shaders/light_fragment.glsl");
    this->shaders.push_back(std::move(outline_shader));

    // MODELS

    Model plane_model;

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

    Model container_model;
    MeshData container_mesh = Mesh::generate_cube_mesh();
    container_model.add_mesh(Mesh(container_mesh));

    Model window_model;
    MeshData window_mesh = Mesh::generate_plane_mesh();

    Texture window_texture("assets/textures/blending_transparent_window.png");
    window_texture.set_type(Texture::Type::Diffuse);
    window_mesh.textures.push_back(window_texture);
    
    window_model.add_mesh(window_mesh);

    Model cube_model;
    MeshData cube_mesh = Mesh::generate_cube_mesh();

    Texture marble_texture("assets/textures/marble.jpg");
    marble_texture.set_type(Texture::Type::Diffuse);
    cube_mesh.textures.push_back(marble_texture);

    cube_model.add_mesh(Mesh(cube_mesh));
    
    this->models.push_back(std::move(plane_model));
    this->models.push_back(std::move(container_model));
    this->models.push_back(std::move(window_model));
    this->models.push_back(std::move(cube_model));

    // TRANSFORMS

    Transform plane_transform(1.0f);
    plane_transform = glm::translate(plane_transform, glm::vec3(0.0f, -0.5f, 0.0f));
    plane_transform = glm::scale(plane_transform, glm::vec3(5.0f));
    this->transforms.push_back(std::move(plane_transform));

    Transform container_transform(1.0f);
    this->transforms.push_back(std::move(container_transform));

    Transform cube1_transform(1.0f);
    cube1_transform = glm::translate(cube1_transform, glm::vec3(-1.0f, 0.0f, -1.0f));
    this->transforms.push_back(cube1_transform);

    Transform cube2_transform(1.0f);
    cube2_transform = glm::translate(cube2_transform, glm::vec3(2.0f, 0.0f, 0.0f));
    this->transforms.push_back(cube2_transform);

    // scaled cube transforms for outline
    cube1_transform = glm::scale(cube1_transform, glm::vec3(1.01f));
    cube2_transform = glm::scale(cube2_transform, glm::vec3(1.01f));
    this->transforms.push_back(std::move(cube1_transform));
    this->transforms.push_back(std::move(cube2_transform));

    for (const auto& pos : window_positions) {
        Transform window_transform(1.0f);
        window_transform = glm::translate(window_transform, pos);
        window_transform = glm::rotate(window_transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        this->transforms.push_back(std::move(window_transform));
    }

    // ENTITIES

    // Add plane
    this->entities.push_back({0, 0, 0});

    // Add container
    this->entities.push_back({1, 1, 1});
    
    // Add two marble cubes
    this->entities.push_back({0, 3, 2});
    this->entities.push_back({0, 3, 3});

    // Add upscaled cubes for stencil drawing
    this->entities.push_back({2, 3, 4});
    this->entities.push_back({2, 3, 5});

    // Add window
    for (size_t i = 0; i < window_positions.size(); i++) {
        float distance = glm::length(window->state.camera_pos - window_positions[i]);
        this->transparent_entities[distance] = {0, 2, 6 + i};
    }

    this->framebuffer = Framebuffer(this->window->width, this->window->height);

    const std::vector<std::string> faces = {
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg",
    };

    this->skybox = CubeMap(faces);

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

    const Shader& container_shader = this->shaders[1];
    container_shader.use();

    container_shader.set("material.shininess", window->state.shininess);

    // directional lights
    glm::vec3 light_dir(-0.2f, -1.0f, -0.3f);
    container_shader.set("dirLight.direction", light_dir);
    container_shader.set("dirLight.ambient", window->state.dirlight_ambient);
    container_shader.set("dirLight.diffuse", window->state.dirlight_diffuse);
    container_shader.set("dirLight.specular", window->state.dirlight_specular);
    container_shader.set("viewPos", window->state.camera_pos);

    // point lights
    for (int i = 0; i < 4; i++) {
        std::string pointLight = "pointLights[" + std::to_string(i) + "]";
        container_shader.set(pointLight + ".position", pointLightPositions[i]);
        container_shader.set(pointLight + ".ambient", window->state.pointlight_ambient);
        container_shader.set(pointLight + ".diffuse", window->state.pointlight_diffuse);
        container_shader.set(pointLight + ".specular", window->state.pointlight_specular);
        container_shader.set(pointLight + ".constant", 1.0f);
        container_shader.set(pointLight + ".linear", 0.09f);
        container_shader.set(pointLight + ".quadratic", 0.032f);
    }

    // spotlight
    container_shader.set("spotLight.position", window->state.camera_pos);
    container_shader.set("spotLight.direction", window->state.camera_front);
    container_shader.set("spotLight.cutoff", glm::cos(glm::radians(window->state.cutoff)));
    container_shader.set("spotLight.outerCutoff", glm::cos(glm::radians(window->state.outer_cutoff)));
    container_shader.set("spotLight.ambient", window->state.spotlight_ambient);
    container_shader.set("spotLight.diffuse", window->state.spotlight_diffuse);
    container_shader.set("spotLight.specular", window->state.spotlight_specular);
    container_shader.set("spotLight.constant", 1.0f);
    container_shader.set("spotLight.linear", 0.09f);
    container_shader.set("spotLight.quadratic", 0.032f);

    // TODO: replace with inplace sort?
    this->transparent_entities.clear();
    for (size_t i = 0; i < window_positions.size(); i++) {
        float distance = glm::length(window->state.camera_pos - window_positions[i]);
        this->transparent_entities[distance] = {0, 2, 6 + i};
    }
}

void Renderer::render() {
    // Draw to framebuffer we created
    this->framebuffer.bind();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    for (size_t i = 0; i < this->entities.size(); i++) {
        const Entity& entity = this->entities[i];
        const Shader& shader = this->shaders[entity.shader_id];
        const Model& model = this->models[entity.model_id];
        const Transform& transform = this->transforms[entity.transform_id];

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

        shader.use();
        shader.set("model", transform);
        shader.set("view", view);
        shader.set("projection", projection);

        model.draw(shader);
    }

    glEnable(GL_DEPTH_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // have fragments always pass the stencil test
    glStencilMask(0xFF); // enable writing to stencil buffer so it can be cleared
    glDisable(GL_CULL_FACE);

    // Render skybox
    this->skybox.draw(view, projection);

    for (auto it = this->transparent_entities.rbegin(); it != this->transparent_entities.rend(); it++) {
        const Entity& entity = it->second;
        const Shader& shader = this->shaders[entity.shader_id];
        const Model& model = this->models[entity.model_id];
        const Transform& transform = this->transforms[entity.transform_id];

        shader.use();
        shader.set("model", transform);
        shader.set("view", view);
        shader.set("projection", projection);

        model.draw(shader);
    }

    // Switch back to default framebuffer
    this->framebuffer.unbind();
    glDisable(GL_DEPTH_TEST);// we don't want any fragments to be discarded
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    this->framebuffer.draw_to_screen();
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
