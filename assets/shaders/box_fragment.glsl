#version 430 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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
    vec4 objectColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), textureMix);

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

    vec3 ambient = material.ambient * light.ambient;
    vec3 diffuse = material.diffuse * diff * light.diffuse / distSquared;
    vec3 specular = material.specular * spec * light.specular;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);// * objectColor;
}
