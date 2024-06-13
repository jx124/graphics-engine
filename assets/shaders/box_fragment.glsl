#version 430 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float textureMix;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vec4 objectColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), textureMix);

    vec4 ambient = ambientStrength * vec4(lightColor, 1.0f);

    vec3 norm = normalize(Normal);
    vec3 lightDir = lightPos - FragPos;
    float distSquared = dot(lightDir, lightDir);
    lightDir = normalize(lightDir);

    float diff = max(dot(norm, lightDir), 0.0f);
    vec4 diffuse = vec4(diffuseStrength * diff * lightColor / distSquared, 1.0f);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec4 specular = vec4(specularStrength * spec * lightColor, 1.0f);

    FragColor = (ambient + diffuse + specular) * objectColor;
}
