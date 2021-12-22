#version 150
// ^ Change this to version 130 if you have compatibility issues

uniform vec4 u_camEye;

uniform int u_GameTimeMin;

uniform vec4 u_Weather;

uniform sampler2D u_Texture;

in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec2 fs_UV;


out vec4 out_Col;


const vec4 distanceFogColor = vec4(0.9, 0.9, 0.9, 0.0);
const vec4 distanceFogNightColor = vec4(0.1, 0.1, 0.1, 0.0);

const vec4 dayColor = vec4(1, 1, 1, 0.8);
const vec4 nightColor = vec4(0.15, 0.15, 0.15, 0.2);


float distanceFog(float distance)
{
    const float maxDistance = 1000;
    const float minDistance = 700;

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

vec4 getDistanceFogColor(int minInDay)
{
    if(minInDay < 300)
    {
        return distanceFogNightColor;
    }
    else if(minInDay < 420)
    {
        return mix(distanceFogNightColor, distanceFogColor, (minInDay - 300) / 120.f);
    }
    else if(minInDay < 1020)
    {
        return distanceFogColor;
    }
    else if(minInDay < 1140)
    {
        return mix(distanceFogColor, distanceFogNightColor, (minInDay - 1020) / 120.f);
    }
    else
    {
        return distanceFogNightColor;
    }
}

void main()
{
    vec2 uv = fs_UV;

    // texture offset animation
    float timeFactor = (u_GameTimeMin % 2000) * 0.0005f;
    float offsetX = timeFactor + fs_UV.x;
    if(offsetX > 1)
    {
        offsetX -= 1;
    }
    uv.x = offsetX;

    vec4 outCol = texture(u_Texture, uv);

    if(outCol.w == 0)
        discard;

    // Color change between day and night
    int minInDay = u_GameTimeMin % 1440;
    if(minInDay < 300)
    {
        outCol *= nightColor;
    }
    else if(minInDay >= 300 && minInDay < 420)
    {
        outCol *= mix(nightColor, dayColor, (minInDay - 300) / 120.f);
    }
    else if(minInDay >= 420 && minInDay < 1020)
    {
        outCol *= dayColor;
    }
    else if(minInDay >= 1020 && minInDay < 1140) // 5 PM - 7PM
    {
        outCol *= mix(dayColor, nightColor, (minInDay - 1020) / 120.f);
    }
    else
    {
        outCol *= nightColor;
    }

    // Distance fog
    float d = distance(fs_Pos, u_camEye);
    float fogFactor = distanceFog(d);
    outCol = mix(outCol, getDistanceFogColor(minInDay), fogFactor);

    if(u_Weather[0] == 1)
    {
        outCol.rgb *= 1 - u_Weather[1] * 0.6f;
    }

    out_Col = outCol;
}
