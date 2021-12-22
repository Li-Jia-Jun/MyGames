#include "noise.h"

float Noise::random1(float x)
{
    return fract(sin(x * 127.1) * 43758.5453);
}

float Noise::random1(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float Noise::random1(vec3 p)
{
    return fract(sin((dot(p, vec3(127.1, 311.7, 191.999)))) * 43758.5453);
}

vec2 Noise::random2(vec2 p)
{
    return fract(sin(vec2(dot(p, vec2(127.1f, 311.7f)),
                     dot(p, vec2(269.5f,183.3f))))
                     * 43758.5453f);
}

vec3 Noise::random3(vec3 p)
{
    return fract(sin(vec3(dot(p, vec3(127.1f, 311.7f, 191.999f)),
                     dot(p, vec3(269.5f,183.3f, 472.6f)),
                     dot(p, vec3(377.4f,451.1f, 159.2f)))
                     * 43758.5453f));
}

float Noise::fbm(float x, float persistence, int octaves, float freq, float amp)
{
    float total = 0;
    for(int i = 1; i <= octaves; i++) {
        total += interpNoise1D(x * freq) * amp;
        freq *= 2.f;
        amp *= persistence;
    }
    return total;

}

float Noise::interpNoise1D(float x)
{
    int intX = int(floor(x));
    float fractX = fract(x);

    float v1 = random1(intX);
    float v2 = random1(intX + 1);
    return mix(v1, v2, fractX);
}



float Noise::fbm(vec2 p, float persistence, int octaves, float freq, float amp, float (*basis)(vec2))
{
    float total = 0;
    for(int i = 1; i <= octaves; i++) {
        total += basis(p * freq) * amp;
        freq *= 2.f;
        amp *= persistence;
    }
    return total;

}

float Noise::worley(vec2 uv)
{
    uv *= 10.0; // Now the space is 10x10 instead of 1x1. Change this to any number you want.
    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);
    float minDist = 1.0; // Minimum distance initialized to max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            vec2 neighbor = vec2(float(x), float(y)); // Direction in which neighbor cell lies
            vec2 point = random2(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell
            vec2 diff = neighbor + point - uvFract; // Distance between fragment coord and neighbor’s Voronoi point
            float dist = length(diff);
            minDist = min(minDist, dist);
        }
    }
    return minDist;

}

float Noise::warpedWorley(vec2 uv, float distortion)
{
    vec2 distort;

    distort.x = fbm(uv);
    distort.y = fbm(uv + vec2(6.2f, 1.3f));

    uv = distortion * distort;
    return worley(uv);
}

float Noise::voronoiMap(vec2 uv)
{
    float numCell = 2.f;
    uv *= numCell;

    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);

    float minDist = 1.f;
    float attribute;

    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            // Direction in which neighbor cell lies
            vec2 neighbor = vec2(float(x), float(y));
            // Get the Voronoi centerpoint for the neighboring cell
            vec2 point = random2(uvInt + neighbor);
            // Distance between fragment coord and neighbor’s Voronoi point
            float dist = length(neighbor + point - uvFract);

            if (dist < minDist)
            {
                minDist = dist;
                // Calculate the position of center
                vec2 centerPoint = uvInt + neighbor + point;
                attribute = random1(centerPoint);
            }
        }
    }
    return attribute;
}

float Noise::perlin2D(vec2 uv)
{
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
        for(int dy = 0; dy <= 1; ++dy) {
            surfletSum += surflet(uv, floor(uv) + vec2(dx, dy));
        }
    }
    return surfletSum;

}

float Noise::perlin3D(vec3 p)
{
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
        for(int dy = 0; dy <= 1; ++dy) {
            for(int dz = 0; dz <= 1; ++dz) {
                surfletSum += surflet(p, floor(p) + vec3(dx, dy, dz));
            }
        }
    }
    return surfletSum;
}

float Noise::HybridMultiFractal(vec2 uv, float H, float freq, float octaves, float offset)
{
    float lacunarity = 2.f;
    float perlinVal = (perlin2D(uv / 1.1f) * 0.5f + 0.5f) * (1 - abs(perlin2D(uv))) + offset;
    float result = perlinVal * pow(freq, -H);
    freq *= lacunarity;

    float weight = result;

    uv *= lacunarity;

    for (int i = 1; i < octaves; i++)
    {
        if (weight > 1.0) weight = 1.0;

        float signal = perlinVal * pow(freq, -H);
        freq *= lacunarity;

        result += weight * signal;
        weight *= signal;

        uv *= lacunarity;
    }

    float remainder = fract(octaves);
    if (remainder > 0.0)
    {
        result += remainder * perlinVal * pow(freq, -H);
    }

    return result;
}

float Noise::interpNoise2D(vec2 p)
{
    int intX = int(floor(p.x));
    float fractX = fract(p.x);
    int intY = int(floor(p.y));
    float fractY = fract(p.y);

    float v1 = random1(vec2(intX, intY));
    float v2 = random1(vec2(intX + 1, intY));
    float v3 = random1(vec2(intX, intY + 1));
    float v4 = random1(vec2(intX + 1, intY + 1));

    float i1 = mix(v1, v2, fractX);
    float i2 = mix(v3, v4, fractX);
    return mix(i1, i2, fractY);

}

float Noise::falloff(vec2 p, vec2 corner)
{
    float t = distance(p, corner); // Linear
    t = t*t*t*(t*(t*6 + 15) - 10); // Quintic, 6t5 + 15t4 - 10t3
    return 1 - t; // Want more weight the closer we are
}

float Noise::surflet(vec2 p, vec2 gridPoint)
{
    // Compute the distance between p and the grid point along each axis, and warp it with a
    // quintic function so we can smooth our cells
    vec2 t2 = abs(p - gridPoint);
    //vec2 t = vec2(1.f) - 6.f * pow(t2, vec2(5.f)) + 15.f * pow(t2, vec2(4.f)) - 10.f * pow(t2, vec2(3.f));
    vec2 t;
    t.x = 1.f - 6.f*t2.x*t2.x*t2.x*t2.x*t2.x + 15.f*t2.x*t2.x*t2.x*t2.x - 10.f*t2.x*t2.x*t2.x;
    t.y = 1.f - 6.f*t2.y*t2.y*t2.y*t2.y*t2.y + 15.f*t2.y*t2.y*t2.y*t2.y - 10.f*t2.y*t2.y*t2.y;
    // Get the random vector for the grid point (assume we wrote a function random2
    // that returns a vec2 in the range [0, 1])
    vec2 gradient = random2(gridPoint) * 2.f - vec2(1.f,1.f);
    // Get the vector from the grid point to P
    vec2 diff = p - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * t.x * t.y;
}

float Noise::surflet(vec3 p, vec3 gridPoint)
{
    // Compute the distance between p and the grid point along each axis, and warp it with a
    // quintic function so we can smooth our cells
    vec3 t2 = abs(p - gridPoint);
    //vec3 t = vec3(1.f) - 6.f * pow(t2, vec3(5.f)) + 15.f * pow(t2, vec3(4.f)) - 10.f * pow(t2, vec3(3.f));
    vec3 t;
    t.x = 1.f - 6.f*t2.x*t2.x*t2.x*t2.x*t2.x + 15.f*t2.x*t2.x*t2.x*t2.x - 10.f*t2.x*t2.x*t2.x;
    t.y = 1.f - 6.f*t2.y*t2.y*t2.y*t2.y*t2.y + 15.f*t2.y*t2.y*t2.y*t2.y - 10.f*t2.y*t2.y*t2.y;
    t.z = 1.f - 6.f*t2.z*t2.z*t2.z*t2.z*t2.z + 15.f*t2.z*t2.z*t2.z*t2.z - 10.f*t2.z*t2.z*t2.z;
    // Get the random vector for the grid point (assume we wrote a function random2
    // that returns a vec2 in the range [0, 1])
    vec3 gradient = random3(gridPoint) * 2.f - vec3(1.f, 1.f, 1.f);
    // Get the vector from the grid point to P
    vec3 diff = p - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * t.x * t.y * t.z;
}
