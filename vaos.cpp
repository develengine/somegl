#include "vaos.hpp"

#include <iostream>

#include <glad/glad.h>

VAO::VAO(bool bind) {
    glGenVertexArrays(1, &vaoId);
    if (bind) {
        glBindVertexArray(vaoId);
    }
}

void VAO::bind() {
    glBindVertexArray(vaoId);
}

void VAO::bindIndexBuffer() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
}

void VAO::add(uint32_t bufferId) {
    bufferIds.push_back(bufferId);
}

void VAO::addIndexBuffer(uint32_t bufferId) {
    iboId = bufferId;
}

void VAO::bindNone() {
    glBindVertexArray(0);
}

uint32_t VAO::id() {
    return vaoId;
}

std::vector<uint32_t> &VAO::buffers() {
    return bufferIds;
}

void VAO::free() {
    if (alive) {
        alive = false;
        glDeleteBuffers(bufferIds.size(), bufferIds.data());
        glDeleteBuffers(1, &iboId);
        glDeleteVertexArrays(1, &vaoId);
    }
}

VAO::~VAO() {
    free();
}

uint32_t makeIndexBuffer(uint32_t *buffer, uint32_t length) {
    uint32_t ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * sizeof(uint32_t), buffer, GL_STATIC_DRAW);

    return ibo;
}

uint32_t makeVTNBuffer(uint32_t attributeOffset, float *buffer, uint32_t length) {
    uint32_t vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, length * sizeof(float), buffer, GL_STATIC_DRAW);

    glVertexAttribPointer(attributeOffset, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(attributeOffset);

    glVertexAttribPointer(attributeOffset + 1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(attributeOffset + 1);

    glVertexAttribPointer(attributeOffset + 2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(attributeOffset + 2);

    return vbo;
}

uint32_t makeMatrixBuffer(uint32_t attributeOffset, float *buffer, uint32_t length, GLenum usage, int divisor) {
    uint32_t vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, length * 64, buffer, usage);

    glVertexAttribPointer(attributeOffset, 4, GL_FLOAT, GL_FALSE, 64, (void*)0);
    glEnableVertexAttribArray(attributeOffset);
    glVertexAttribPointer(attributeOffset + 1, 4, GL_FLOAT, GL_FALSE, 64, (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(attributeOffset + 1);
    glVertexAttribPointer(attributeOffset + 2, 4, GL_FLOAT, GL_FALSE, 64, (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(attributeOffset + 2);
    glVertexAttribPointer(attributeOffset + 3, 4, GL_FLOAT, GL_FALSE, 64, (void*)(12 * sizeof(float)));
    glEnableVertexAttribArray(attributeOffset + 3);

    glVertexAttribDivisor(attributeOffset, divisor);
    glVertexAttribDivisor(attributeOffset + 1, divisor);
    glVertexAttribDivisor(attributeOffset + 2, divisor);
    glVertexAttribDivisor(attributeOffset + 3, divisor);

    return vbo;
}
