#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const std::string& image_path) : unit(num_textures) {
    glGenTextures(1, &this->id);
    glActiveTexture(GL_TEXTURE0 + num_textures++);
    glBindTexture(GL_TEXTURE_2D, this->id);
    
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(image_path.c_str(), &this->width, &this->height, &this->n_channels, 0);

    GLenum format;
    switch (n_channels) {
    case 1:
        format = GL_RED; break;
    case 3:
        format = GL_RGB; break;
    case 4: default:
        format = GL_RGBA; break;
    }

    if (data) {
        // generate a texture for the currently bound texture
        glTexImage2D(GL_TEXTURE_2D, 0, format, this->width, this->height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    } else {
        std::cerr << "Failed to load image " << image_path << std::endl;
        std::terminate();
    }
    stbi_image_free(data);
}

Texture::Texture(const ImageData& image_data) : unit(num_textures) {
    glGenTextures(1, &this->id);
    glActiveTexture(GL_TEXTURE0 + num_textures++);
    glBindTexture(GL_TEXTURE_2D, this->id);

    this->width = image_data.width;
    this->height = image_data.height;
    this->n_channels = image_data.n_channels;

    GLenum format;
    switch (image_data.n_channels) {
    case 1:
        format = GL_RED; break;
    case 3:
        format = GL_RGB; break;
    case 4: default:
        format = GL_RGBA; break;
    }

    if (image_data.data) {
        // generate a texture for the currently bound texture
        glTexImage2D(GL_TEXTURE_2D, 0, format, this->width, this->height, 0, format, GL_UNSIGNED_BYTE, image_data.data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    } else {
        std::cerr << "Failed to load image " << image_data.path << std::endl;
        std::terminate();
    }
}

Texture::Texture(int width, int height) : unit(num_textures) {
    glGenTextures(1, &this->id);
    glActiveTexture(GL_TEXTURE0 + num_textures++);
    glBindTexture(GL_TEXTURE_2D, this->id);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::set_type(Texture::Type type) {
    switch (type) {
    case Texture::Type::Diffuse: default:
        this->type = "texture_diffuse";
        break;
    case Texture::Type::Specular:
        this->type = "texture_specular";
        break;
    case Texture::Type::Normal:
        this->type = "texture_normal";
        break;
    case Texture::Type::Height:
        this->type = "texture_height";
        break;
    }
}

std::vector<Texture> Texture::load_textures(std::vector<std::string> image_paths) {
    size_t length = image_paths.size();
    std::vector<std::future<ImageData>> images(length);
    stbi_set_flip_vertically_on_load(true);

    // asynchronously read all images
    for (size_t i = 0; i < length; i++) {
        const std::string& image_path = image_paths[i];

        images[i] = std::async(std::launch::async, [image_path](){
            std::cout << "Loading " << image_path <<  std::endl;
            ImageData image;
            unsigned char* data = stbi_load(image_path.c_str(), &image.width, &image.height, &image.n_channels, 0);
            image.data = data;
            image.path = image_path;

            return image;
        });
    }

    std::vector<Texture> result;

    // wait for all images then create their textures
    for (size_t i = 0; i < length; i++) {
        ImageData image = images[i].get();
        result.emplace_back(image);
        stbi_image_free(image.data);
    }
    
    return result;
}

Texture Texture::load_cubemap(const std::vector<std::string>& faces, int texture_unit) {
    Texture texture{};
    glGenTextures(1, &texture.id);
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
    texture.unit = texture_unit;

#ifdef DEBUG
    if (faces.size() != 6) {
        std::cout << "Warning: number of cubemap images not equal to 6." << std::endl;
    }
#endif

    unsigned char* data;
    int width, height, n_channels;
    GLenum format;

    stbi_set_flip_vertically_on_load(false);
    for (size_t i = 0; i < faces.size(); i++) {
        data = stbi_load(faces[i].c_str(), &width, &height, &n_channels, 0);

        switch (n_channels) {
        case 1:
            format = GL_RED; break;
        case 3:
            format = GL_RGB; break;
        case 4: default:
            format = GL_RGBA; break;
        }

        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load cubemap face " << faces[i] << std::endl;
            std::terminate();
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}
