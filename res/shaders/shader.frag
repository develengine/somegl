#version 460 core

layout(location = 0) in vec2 o_texCoords;

layout(location = 0) out vec4 o_color;

uniform sampler2D textureSampler;

void main() {
    o_color = texture(textureSampler, o_texCoords);
}
