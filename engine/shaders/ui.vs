#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aUV;

uniform mat4 uProjection;
uniform mat4 uModel;

out vec3 color;

out vec2 TexCoords;

void main() {
    color = aColor;
    TexCoords = aUV;
    gl_Position = uProjection * uModel * vec4(aPos.xy, 0.0, 1.0);
}