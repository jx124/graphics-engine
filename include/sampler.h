#pragma once

#include <cstdint>
#include <limits>

struct xorshift64_state {
    uint64_t a;
};

uint64_t xorshift64(struct xorshift64_state *state) noexcept;
float randomFloat(struct xorshift64_state *state) noexcept;