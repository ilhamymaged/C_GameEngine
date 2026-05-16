#version 460 core
out vec4 FragColor;

uniform bool uUseAlbedo;

uniform sampler2D uAlbedo;
in vec2 TexCoords;

uniform vec3 uColor;

in vec3 Normal;
in vec3 FragPos;

void main() {
    if (uUseAlbedo) {
        FragColor = texture(uAlbedo, TexCoords);
    } else {
        FragColor = vec4(uColor, 1.0);
    }
}