#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const std::string& image_path, GLenum format) : index(num_textures) {
    glGenTextures(1, &this->id);
    glActiveTexture(GL_TEXTURE0 + num_textures++);
    glBindTexture(GL_TEXTURE_2D, this->id);
    
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(image_path.c_str(), &this->width, &this->height, &this->n_channels, 0);

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

