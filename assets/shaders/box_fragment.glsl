#version 430 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float brightness;
};

out vec4 FragColor;

in vec2 texCoord;
in vec3 Normal;
in vec3 FragPos;

uniform Material material;
uniform Light light;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float textureMix;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // diffuse calculations
    vec3 norm = normalize(Normal);
    vec3 lightDir = lightPos - FragPos;
    float distSquared = dot(lightDir, lightDir);
    lightDir = normalize(lightDir);
    float diff = max(dot(norm, lightDir), 0.0f);

    // specular calculations
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = vec3(texture(material.diffuse, texCoord)) * light.ambient;
    vec3 diffuse = vec3(texture(material.diffuse, texCoord)) * light.brightness * diff * light.diffuse / distSquared;
    vec3 specular = vec3(texture(material.specular, texCoord)) * spec * light.specular;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
}
