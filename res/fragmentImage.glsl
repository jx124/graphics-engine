#version 450 core

in vec2 TexCoord;

uniform sampler2D ImageTexture;
uniform int count;

out vec4 FragColor;

void main()
{
    const float gamma = 2.2;
    vec4 color = texture(ImageTexture, TexCoord);

    color = color / count;

    const vec4 gammaCorrection = vec4(1.0 / gamma);
    FragColor = pow(color, gammaCorrection);
}
