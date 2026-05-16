#version 460 core
out vec4 FragColor;

#define MAX_LIGHTS 10

uniform bool uUseAlbedo;

uniform sampler2D uAlbedo;
in vec2 TexCoords;

uniform vec3 uColor;

uniform vec3 uLightColor[MAX_LIGHTS];
uniform vec3 uLightPos[MAX_LIGHTS];
uniform float uLightRadius[MAX_LIGHTS];
uniform int uActiveLightCount;
uniform vec3 uViewPos;

in vec3 Normal;
in vec3 FragPos;

void main() {

    float ambientStrength = 0.1;
    float specularStrength = 0.5;
    vec3 norm = normalize(Normal);

    vec3 ambient = vec3(ambientStrength);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < uActiveLightCount; i++) {
        vec3 lightVec = uLightPos[i] - FragPos;
        float dist = length(lightVec);
        vec3 lightDir = lightVec / dist;
        
        float radius = uLightRadius[i];
        float attenuation = clamp(1.0 - (dist / radius), 0.0, 1.0);
        
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += uLightColor[i] * diff * attenuation;

        vec3 viewDir = normalize(uViewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        specular += specularStrength * spec * uLightColor[i]  * attenuation;
    }

    vec3 result = (ambient + diffuse + specular);

    if (uUseAlbedo) {
        result *= texture(uAlbedo, TexCoords).rgb;
    } else {
        result *= uColor;
    }
    FragColor = vec4(result, 1.0);
}