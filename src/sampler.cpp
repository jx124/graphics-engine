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
