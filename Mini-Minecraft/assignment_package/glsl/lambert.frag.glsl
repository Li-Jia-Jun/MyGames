#version 330
// ^ Change this to version 130 if you have compatibility issues


uniform float u_Time;
uniform int u_GameTimeMin;

uniform mat4 u_ModelInvTr;

uniform vec4 u_camEye;  // Camera world position

uniform vec4 u_fogColor;

uniform vec4 u_Color; // Sunlight color

uniform vec4 u_Weather;

uniform sampler2D u_Texture;
uniform sampler2D u_Normalmap;

in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec2 fs_UV;
in vec4 fs_OriginalPos;

in float fs_AnimFlag;

in vec4 fs_ViewDir;

in float fs_shade;

out vec4 out_Col;

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

float fractalPerlin2D(vec2 p, float persistence, int octaves, float freq, float amp)
{
    float total = 0;
    for(int i = 1; i <= octaves; i++) {
        total += perlin2D(p * freq) * amp;
        freq *= 2.f;
        amp *= persistence;
    }
    return total;
}

vec3 distortVert(vec3 vertPos)
{
    vec3 vert = vec3(vertPos.x, vertPos.y, vertPos.z);
    vert /= 3.f;

    float tmp = (fractalPerlin2D(vert.xz + 100.f + u_Time * 0.16f, 0.5f, 4, 1.f, 0.5f) * 0.5f + 0.5f);
    vert.x += (fractalPerlin2D(vert.xz + 200.f + u_Time * 0.16f, 0.5f, 4, 1.f, 0.5f) * 0.5f + 0.5f);
    vert.z += tmp;

    float noiseVal = fractalPerlin2D(vert.xz, 0.5f, 4, 1.f, 0.5f) * 0.5f + 0.2f;
    vertPos.y -= noiseVal;
    return vertPos;
}



float random1(vec3 p) {
    return fract(sin(dot(p,vec3(127.1, 311.7, 191.999)))
                 *43758.5453);
}

float mySmoothStep(float a, float b, float t) {
    t = smoothstep(0, 1, t);
    return mix(a, b, t);
}

float cubicTriMix(vec3 p) {
    vec3 pFract = fract(p);
    float llb = random1(floor(p) + vec3(0,0,0));
    float lrb = random1(floor(p) + vec3(1,0,0));
    float ulb = random1(floor(p) + vec3(0,1,0));
    float urb = random1(floor(p) + vec3(1,1,0));

    float llf = random1(floor(p) + vec3(0,0,1));
    float lrf = random1(floor(p) + vec3(1,0,1));
    float ulf = random1(floor(p) + vec3(0,1,1));
    float urf = random1(floor(p) + vec3(1,1,1));

    float mixLoBack = mySmoothStep(llb, lrb, pFract.x);
    float mixHiBack = mySmoothStep(ulb, urb, pFract.x);
    float mixLoFront = mySmoothStep(llf, lrf, pFract.x);
    float mixHiFront = mySmoothStep(ulf, urf, pFract.x);

    float mixLo = mySmoothStep(mixLoBack, mixLoFront, pFract.z);
    float mixHi = mySmoothStep(mixHiBack, mixHiFront, pFract.z);

    return mySmoothStep(mixLo, mixHi, pFract.y);
}

float fbm(vec3 p) {
    float amp = 0.5f;
    float freq = 4.0f;
    float sum = 0.0f;
    for(int i = 0; i < 8; i++) {
        sum += cubicTriMix(p * freq) * amp;
        amp *= 0.5;
        freq *= 2.0;
    }
    return sum;
}


float distanceFog(float distance)
{
    float maxDistance = 500;
    float minDistance = 200;

    // distance fog gets closer during rain
    if(u_Weather[0] == 1)
    {
        maxDistance = mix(maxDistance, 300, u_Weather[1]);
        minDistance = mix(minDistance, 0, u_Weather[1]);
    }

    if(distance >= maxDistance)
    {
        return 1;
    }
    else if(distance <= minDistance)
    {
        return 0;
    }
    else
    {
        // Linear blend
        return 1 - (maxDistance - distance) / (maxDistance - minDistance);
    }
}

float getShadeValue()
{
    float shade;

    float f1 = 0.7f;
    float f2 = 0.9f;
    int minInDay = u_GameTimeMin % 1440;
    if(minInDay < 300)
    {
        shade = f2;
    }
    else if(minInDay < 420)
    {
        shade = mix(f2, f1, (minInDay - 300.f) / 120.f);
    }
    else if(minInDay < 1020)
    {
        shade = f1;
    }
    else if(minInDay < 1080)
    {
        shade = mix(f1, f2, (minInDay - 1020.f) / 60.f);
    }
    else
    {
        shade = f2;
    }

    return shade;
}

void main()
{
    vec4 waterNor = vec4(0);

    // Animation offset
    vec2 uv = fs_UV;
    if(abs(fs_AnimFlag - 1.f) < 0.00001f || abs(fs_AnimFlag - 2.f) < 0.00001f)
    {
        float gap = 1.f / 16.f;
        int timeFactor = int(u_Time * 40) % 100;
        float offsetX = timeFactor * 0.01 * gap + fs_UV.x;
        if(offsetX > ceil(fs_UV.x * 16.f) / 16.f)
        {
            offsetX -= gap;
        }
        uv.x = offsetX;


        // compute normal of water
        vec3 tangent1 = vec3(0.f, 0.f, 1.f);
        vec3 tangent2 = vec3(1.f, 0.f, 0.f);

        vec3 pos1 = fs_OriginalPos.xyz + tangent1 * 0.02f;
        vec3 pos2 = fs_OriginalPos.xyz - tangent1 * 0.02f;
        vec3 pos3 = fs_OriginalPos.xyz + tangent2 * 0.02f;
        vec3 pos4 = fs_OriginalPos.xyz - tangent2 * 0.02f;

        pos1 = distortVert(pos1);
        pos2 = distortVert(pos2);
        pos3 = distortVert(pos3);
        pos4 = distortVert(pos4);

        waterNor.xyz = normalize(cross(pos1 - pos2, pos3 - pos4));
        mat3 invTranspose = mat3(u_ModelInvTr);
        waterNor.xyz = invTranspose * vec3(waterNor);
    }

    // Material base color (before shading)
    vec4 diffuseColor = texture(u_Texture, uv);

    if(diffuseColor.a == 0)
        discard;


//    diffuseColor.rgb = diffuseColor.rgb * (0.5f * fbm(fs_Pos.xyz / 32.0f) + 0.5f);

    // Diffuse
    vec4 dirNor = normalize(fs_Nor);
    vec4 dirSun = normalize(fs_LightVec);
    float diffuseTerm = dot(dirNor, dirSun) * 0.8f;
    float moonDiffuseTerm = dot(dirNor, -dirSun) * 0.2f;    // Moon is at the opposite direction of sun
    moonDiffuseTerm = clamp(moonDiffuseTerm, 0.f, 1.f);
    diffuseTerm = clamp(diffuseTerm, 0.f, 1.f);

    // Smooth diffuse effect of side faces
    if (abs(dirNor.y) < 0.00001f)
    {
        float h = dirNor.y - dirSun.y;
        if(h <= 0.f)
        {
        }
        else if(h > 0.f && h < 0.1f)
        {
            diffuseTerm *= ((0.1f - h) / 0.1f);
        }
        else
        {
            diffuseTerm = 0.f;
        }

        float h2 = dirNor.y + dirSun.y;
        if(h2 <= 0.f)
        {
        }
        else if(h2 > 0.f && h2 < 0.1f)
        {
            moonDiffuseTerm *= ((0.1f - h2) / 0.1f);
        }
        else
        {
            moonDiffuseTerm = 0.f;
        }
    }
    // Down faces don't diffuse
    else if(dirNor.y < -0.999f)
    {
        diffuseTerm = 0.f;
        moonDiffuseTerm = 0.f;
    }

    // Ambient
    float ambientTerm = 0.2f;

    float lightIntensity = diffuseTerm + moonDiffuseTerm + ambientTerm;   //Add a small float value to the color multiplier
                                                        //to simulate ambient lighting. This ensures that faces that are not
                                                        //lit by our point light are not completely black.   
    // Weather effect
    if(u_Weather[0] == 1)
    {
        lightIntensity *= (1 - u_Weather[1] * 0.4f);
    }

    // Shade under the tree
    if(fs_shade > 0)
    {
        lightIntensity *= getShadeValue();
    }

    // Fixed lightIntensity for lava surface
    if (abs(fs_AnimFlag - 2.f) < 0.00001f)
    {
        diffuseTerm = dot(dirNor, vec4(0.f, 1.f, 0.f, 0.f)) * 0.8f;
        lightIntensity = diffuseTerm + ambientTerm;
    }

    // Compute final shaded color
    vec4 outCol = vec4(diffuseColor.rgb * u_Color.rgb * lightIntensity, diffuseColor.a);

    // Compute Blinn Phong
    if (abs(fs_AnimFlag - 1.f) < 0.00001f && u_GameTimeMin % 1440 < 1140 && u_Weather[0] != 1)
    {
        vec4 H = (fs_ViewDir + dirSun) / 2;
        float specularTerm = max(pow(dot(waterNor, normalize(H)), 32), 0) * 0.7f;
        outCol.rgb += vec3(1, 1, 1) * specularTerm;
    }

    // Distance fog
    float d = distance(fs_Pos, u_camEye);
    float fogFactor = distanceFog(d);
    outCol = mix(outCol,u_fogColor, fogFactor);

    out_Col = outCol;
}
