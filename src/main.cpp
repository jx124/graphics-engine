#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "vertexarray.h"
#include "vertexbuffer.h"
#include "elementbuffer.h"
#include "shader.h"
#include "texture.h"

#include <string>
#include <cstdio>
#include <vector>

int main(void) {
    Window window(800, 600, "Voxel Engine");

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

    while (!window.should_close()) {
        window.process_input();

        // rendering commands here
        glClearColor(0.2f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.set("textureMix", window.state.mix);
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // use EBO instead

        glfwSwapBuffers(window.ptr);
        glfwPollEvents();
    }

    // TODO: Add to destructors of wrapper classes when renderer class is completed
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);
    // glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

