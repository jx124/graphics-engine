#pragma once

#include <cstdint>
#include <limits>
#include <glm/glm.hpp>
#include <random>

struct xorshift64_state {
    uint64_t a;
};

uint64_t xorshift64(struct xorshift64_state *state) noexcept;
float randomFloat(struct xorshift64_state *state) noexcept;
float randomFloat(struct xorshift64_state *state, float min, float max) noexcept;
float randomFloat(float min, float max) noexcept;
glm::vec3 randomInUnitSphere(struct xorshift64_state *state) noexcept;
glm::vec3 randomInUnitSphere() noexcept;