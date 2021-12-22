#version 330
// noOperation.vert.glsl:
// A fragment shader used for post-processing that simply reads the
// image produced in the first render pass by the surface shader
// and outputs it to the frame buffer


in vec2 fs_UV;

out vec4 color;

uniform sampler2D u_RenderedTexture;

uniform float u_Time;


vec2 random2(vec2 p)
{
    return fract(sin(vec2(dot(p, vec2(127.1f, 311.7f)), dot(p, vec2(269.5f,183.3f)))) * 43758.5453f);
}

float surflet(vec2 p, vec2 gridPoint)
{
    // Compute the distance between p and the grid point along each axis, and warp it with a
    // quintic function so we can smooth our cells
    vec2 t2 = abs(p - gridPoint);
//    vec2 t = vec2(1.f) - 6.f * pow(t2, vec2(5.f)) + 15.f * pow(t2, vec2(4.f)) - 10.f * pow(t2, vec2(3.f));
    vec2 t;
    t.x = 1.0f - 6.0f*t2.x*t2.x*t2.x*t2.x*t2.x + 15.0f*t2.x*t2.x*t2.x*t2.x - 10.0f*t2.x*t2.x*t2.x;
    t.y = 1.0f - 6.0f*t2.y*t2.y*t2.y*t2.y*t2.y + 15.0f*t2.y*t2.y*t2.y*t2.y - 10.0f*t2.y*t2.y*t2.y;
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

float perlin2D(vec2 uv)
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

float worleyNoise(vec2 uv) {
    float numCell = 20.f;

    uv.x += perlin2D(uv * 5.f + u_Time * 0.5f) * 0.2f;
    uv.y += perlin2D(uv * 5.f + 100.f + u_Time * 0.5f) * 0.2f;
    uv.x = clamp(uv.x, 0.0f, 1.0f);
    uv.y = clamp(uv.y, 0.0f, 1.0f);

    uv *= numCell;

    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);

    float minDist = 1.0;

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

            minDist = min(minDist, dist);
        }
    }
    return minDist;
}

float scalar_field(vec2 uv){
    // return range [0, 1]
    return distance(uv ,vec2(0.5f, 0.5f)) * sqrt(2.0f);
}




vec3 random3(vec3 p)
{
    return fract(sin(vec3(dot(p, vec3(127.1f, 311.7f, 191.999f)),
                     dot(p, vec3(269.5f,183.3f, 472.6f)),
                     dot(p, vec3(377.4f,451.1f, 159.2f)))
                     * 43758.5453f));
}

float surflet(vec3 p, vec3 gridPoint)
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

float perlin3D(vec3 p)
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

void main()
{
    vec2 uv;
    uv.x = fs_UV.x + perlin3D(vec3(fs_UV * 2.0f, u_Time * 0.12f) + u_Time * 0.2f) * 0.015f;
    uv.y = fs_UV.y + perlin3D(vec3(fs_UV * 2.0f + 100.0f, u_Time * 0.22f) + 100.0f + u_Time * 0.15f) * 0.015f;

    color = texture(u_RenderedTexture, uv).rgba;

    float intensity = smoothstep(0.25f, 0.85f, scalar_field(fs_UV));
    float noiseVal = smoothstep(0.3f, 0.7f, worleyNoise(fs_UV / 1.1f) * worleyNoise(fs_UV));
    float colorIntensity = noiseVal * intensity * 0.4f + 0.6f; // map to [0.6, 1]
    vec3 mixColor = vec3(10.f/255.f,170.f/255.f,230.f/255.f) * colorIntensity;
    color.rgb = mix(color.rgb, mixColor, 0.45f);
}
