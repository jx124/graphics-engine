#pragma once

#include <glad/glad.h>

#include <string>
#include <iostream>
#include <vector>
#include <future>
#include <thread>

struct ImageData {
    unsigned char* data;
    int width, height, n_channels;
    std::string path;
};

class Texture {
public:
    Texture() = default;
    Texture(const std::string& image_path);
    Texture(const ImageData& image_data);
    Texture(int width, int height); // allocate empty texture as memory

    enum class Type {
        Diffuse,
        Specular,
        Normal,
        Height
    };

    void set_type(Texture::Type type);

    static std::vector<Texture> load_textures(std::vector<std::string> image_paths);
    static Texture load_cubemap(const std::vector<std::string>& faces, int texture_unit = 0);

    GLuint id;
    int width, height, n_channels;
    int unit; // active shader slot for setting shader uniforms
    static inline int num_textures = 0;
    std::string type;
    std::string path;
};
