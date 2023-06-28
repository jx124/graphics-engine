#include "sampler.h"

uint64_t xorshift64(struct xorshift64_state *state) noexcept {
    uint64_t x = state->a;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return state->a = x;
}

float randomFloat(struct xorshift64_state *state) noexcept {
    return static_cast<float>(xorshift64(state)) / static_cast<float>(std::numeric_limits<uint64_t>::max());
}

float randomFloat(xorshift64_state *state, float min, float max) noexcept {
    return min + (max - min) * randomFloat(state);
}

float randomFloat(float min, float max) noexcept {
    thread_local std::uniform_real_distribution<float> distribution(min, max);
    thread_local std::mt19937 generator(std::random_device{}());
    return distribution(generator);
}

glm::vec3 randomInUnitSphere(struct xorshift64_state *state) noexcept {
    while (true) {
        glm::vec3 vec(randomFloat(state, -1.0f, 1.0f),
                      randomFloat(state, -1.0f, 1.0f), 
                      randomFloat(state, -1.0f, 1.0f));
        if (glm::dot(vec, vec) <= 1.0f) {
            return vec;
        }
    }
}

glm::vec3 randomInUnitSphere() noexcept {
    while (true) {
        glm::vec3 vec(randomFloat(-1.0f, 1.0f),
                      randomFloat(-1.0f, 1.0f), 
                      randomFloat(-1.0f, 1.0f));
        if (glm::dot(vec, vec) <= 1.0f) {
            return vec;
        }
    }
}