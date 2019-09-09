#pragma once

#include <vector>

#include <stdint.h>

class VAO {
    uint32_t vaoId;
    std::vector<uint32_t> bufferIds;
    uint32_t iboId = 0;

    bool alive = true;

public:

    VAO(bool bind);

    void bind();
    void bindIndexBuffer();
    void add(uint32_t bufferId);
    void addIndexBuffer(uint32_t bufferId);

    uint32_t id();
    std::vector<uint32_t> &buffers();

    static void bindNone();

    void free();
    ~VAO();
};


uint32_t makeIndexBuffer(uint32_t *buffer, uint32_t length);
uint32_t makeVTNBuffer(uint32_t attributeOffset, float *buffer, uint32_t length);
uint32_t makeMatrixBuffer(uint32_t attributeOffset, float *buffer, uint32_t length, uint32_t usage, int divisor);
