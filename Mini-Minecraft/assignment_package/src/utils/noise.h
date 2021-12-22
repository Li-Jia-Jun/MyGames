#pragma once

#include "glm_includes.h"

using namespace glm;

class Noise
{
private:
    // helper functions
    static float interpNoise1D(float x);
    static float interpNoise2D(vec2 p);

    // helper function for perlin noise
    static float falloff(vec2 p, vec2 corner);
    static float surflet(vec2 p, vec2 gridPoint);
    static float surflet(vec3 p, vec3 gridPoint);

public:
    static float random1(float x);
    static float random1(vec2 p);
    static float random1(vec3 p);

    static vec2 random2(vec2 p);
    static vec3 random3(vec3 p);

    static float fbm(float x, float persistence = 0.5f, int octaves = 8,
                     float freq = 2.f, float amp = 0.5f);
    static float fbm(vec2 p, float persistence = 0.5f, int octaves = 8,
                     float freq = 2.f, float amp = 0.5f, float (*basis)(vec2) = interpNoise2D);


    static float worley(vec2 uv);
    static float warpedWorley(vec2 uv, float distortion);
    static float voronoiMap(vec2 uv);

    static float perlin2D(vec2 uv);
    static float perlin3D(vec3 p);

    /// For mountain biome only
    static float HybridMultiFractal(vec2 uv, float H, float freq, float octaves, float offset);



};

