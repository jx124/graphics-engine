#version 460 core

in vec2 TexCoord;

uniform sampler2D ImageTexture;

out vec4 FragColor;

void main()
{
    float gamma = 2.2;
    FragColor = pow(texture(ImageTexture, TexCoord), vec4(1 / gamma));
}