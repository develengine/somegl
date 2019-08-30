#version 460 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoords;

layout(location = 0) out vec2 o_texCoords;

void main() {
    gl_Position = vec4(i_position, 1.0);
    o_texCoords = i_texCoords;
}
