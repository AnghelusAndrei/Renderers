#pragma once
#include "../utils/PerlinNoise.hpp"

struct config_{
    int SCREEN_HEIGHT;
    int SCREEN_WIDTH;
    int res_y;
    int res_x;
    int FOV;

    int density;
    int octaves;
    siv::PerlinNoise::seed_type seed;
    int height;
    
    int depth;
};
