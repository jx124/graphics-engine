#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float mixValue;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse);

    FragColor = vec4(result, 1.0);
    FragColor = FragColor * mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixValue);
}