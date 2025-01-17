#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

#define MULTIPLIER 1664525
#define INCREMENT 1013904223
#define MODULUS ((uint32_t)1 << 7)

static uint32_t state = 10; // Initial seed

void simple_srand(uint32_t seed)
{
    state = seed;
}

uint32_t simple_rand()
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;

    return state;
}

#endif