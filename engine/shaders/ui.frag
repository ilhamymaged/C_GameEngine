#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D uTexture;

in vec3 color;

void main() {
    //FragColor = texture(uTexture, TexCoords);
    FragColor = vec4(color, 1.0);
    // FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}