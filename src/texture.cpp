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
    } else {
        std::cerr << "Failed to load image " << image_data.path << std::endl;
        std::terminate();
    }
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

