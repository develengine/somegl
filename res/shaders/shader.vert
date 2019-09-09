#version 460 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoords;
layout(location = 2) in vec3 i_normals;
layout (location = 3) in mat4 i_instanceMat;

layout(location = 0) out vec2 o_texCoords;
layout(location = 1) out vec3 o_normals;

uniform mat4 u_mvpMat;

void main() {
    gl_Position = u_mvpMat * i_instanceMat * vec4(i_position, 1.0);
    o_texCoords = i_texCoords;
    o_normals = i_normals;
}
