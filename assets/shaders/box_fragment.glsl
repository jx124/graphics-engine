#version 430 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float textureMix;
uniform float ambientStrength;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main() {
    vec4 objectColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), textureMix);

    vec4 ambient = ambientStrength * vec4(lightColor, 1.0f);

    vec3 norm = normalize(Normal);
    vec3 lightDir = lightPos - FragPos;
    float distSquared = dot(lightDir, lightDir);
    lightDir = normalize(lightDir);

    float diff = max(dot(norm, lightDir), 0.0f);
    vec4 diffuse = vec4(diff * lightColor / distSquared, 1.0f);

    FragColor = (ambient + diffuse) * objectColor;
}
