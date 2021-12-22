#version 150

uniform mat4 u_ViewProj;    // We're actually passing the inverse of the viewproj
                            // from our CPU, but it's named u_ViewProj so we don't
                            // have to bother rewriting our ShaderProgram class

uniform ivec2 u_Dimensions; // Screen dimensions

uniform vec4 u_camEye;  // Camera world position

uniform vec4 u_Offset;  // Use offset channel to store sun direction

uniform int u_GameTimeMin;

uniform vec4 u_Weather;

out vec4 outColor;

const float PI = 3.14159265359;
const float TWO_PI = 6.28318530718;

// Sunset palette
const vec4 daySkyColor[3] = vec4[](vec4(240, 240, 240, 255) / 255.0,
                               vec4(225, 225, 225, 255) / 255.0,
                               vec4(135, 206, 235, 255) / 255.0);

const vec4 nightSkyColor = vec4(0, 0, 21, 255) / 255.0;
const vec4 midSkyColor = vec4(0, 0, 31, 255) / 255.0;


const vec4 sunRiseColor = vec4(171, 66, 51, 255) / 255.0;
const vec4 sunRiseColor2 = vec4(253, 125, 1, 160) / 255.0;

const vec4 sunColor = vec4(255, 255, 240, 255) / 255.0;
const vec4 sunYellowColor = vec4(249, 219, 105, 255) / 255.0;   // Sun color during sunset or sunrise
const vec4 moonColor = vec4(255, 255, 255, 255) / 255.0;
const vec4 starColor = vec4(255, 255, 255, 255) / 255.0;


float random1(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}


float hash12 (vec2 p)
{
  vec3 p3 = fract (vec3 (p.x, p.y, p.x) * 0.1031);
  p3 += dot (p3, p3.yzx + 19.19);
  return fract ((p3.x + p3.y) * p3.z);
}


float snoise(vec2 p)
{
    vec2 cell = floor(p);

    return 2.0 * hash12(cell) - 1.0;
}


vec2 sphereToUV(vec3 p)
{
    float phi = atan(p.z, p.x);
    if(phi < 0)
    {
        phi += TWO_PI;
    }
    float theta = acos(p.y);
    return vec2(1 - phi / TWO_PI, 1 - theta / PI);
}

vec4 uvToDaySky(vec2 uv)
{
    if(uv.y < 0.35)
    {
        return daySkyColor[0];
    }
    else if(uv.y < 0.50)
    {
        return mix(daySkyColor[0], daySkyColor[1], (uv.y - 0.35) / 0.15f);
    }
    else if(uv.y < 0.56)
    {
        return mix(daySkyColor[1], daySkyColor[2], (uv.y - 0.50) / 0.06f);
    }
    return daySkyColor[2];
}

vec4 uvToNightSky(vec2 uv)
{
    if(uv.y < 0.3f)
    {
        return nightSkyColor;
    }

    uv += vec2(0, u_GameTimeMin * 0.0003f + 2.f);
    vec2 p = uv * 4096;

    // Star
    if(snoise(p) > 0.99965f)
    {
        int minInDay = u_GameTimeMin % 1440;
        if(minInDay >= 360 && minInDay < 450) // 6 AM - 7:30 AM, stars fades out
        {
            return mix(starColor, nightSkyColor, (minInDay - 360) / 90.f);
        }
        else if(minInDay >= 450 && minInDay < 1080)
        {
            return nightSkyColor;
        }
        else if(minInDay >= 1020 && minInDay < 1110) // 5 PM - 6:30 PM, stars fades in
        {
            return mix(nightSkyColor, starColor, (minInDay - 1020) / 80.f);
        }
        else
        {
            return starColor;
        }
    }
    else
    {
       return nightSkyColor;
    }
}

float uvToSunriseSunsetColorFactor(vec2 uv)
{
    if(uv.y < 0.35f)
    {
        return 0;
    }
    else if(uv.y <= 0.45f)
    {
        return ((uv.y - 0.35f) / 0.1f) * 0.9f;
    }
    else if(uv.y <= 0.55f) // When y goes from 0.45-0.55, factor stays 0.9
    {
        return 0.9f;
    }
    else if(uv.y <= 0.6f) // When y goes from 0.55-0.6, factor goes from 0.9-0.1
    {
        return 0.9f - ((uv.y - 0.55f) / 0.05f) * 0.8f;
    }
    else if(uv.y <= 0.75f) // When y goes from 0.6-0.75, factor goes from 0.1-0
    {
        return 0.1f - ((uv.y - 0.6f) / 0.15f) * 0.1f;
    }
    else
    {
        return 0;
    }
}

vec4 getSunRiseColor(int minInDay)
{
    if(minInDay < 370)
        return sunRiseColor;
    if(minInDay < 420)
        return mix(sunRiseColor, sunRiseColor2, (minInDay - 370) / 50.f);
    else
        return sunRiseColor2;
}

vec4 getSunSetColor(int minInDay)
{
    if(minInDay < 1020)
        return sunRiseColor2;
    else if(minInDay < 1100)
        return mix(sunRiseColor2, sunRiseColor, (minInDay - 1020) / 80.f);
    else
        return sunRiseColor;
}

vec4 getSkyColor(vec2 uv)
{
    // Get sky color of day or night
    vec4 dayColor = uvToDaySky(uv);
    vec4 nightcolor = uvToNightSky(uv);
    int minInDay = u_GameTimeMin % 1440;
    if(minInDay < 300) // 0 AM - 5 AM
    {
        return nightcolor;
    }
    else if(minInDay <= 440) // 5 Am - 7 20 AM (Sunrise)
    {
        vec4 baseColor = mix(nightcolor, dayColor, clamp((minInDay - 340) / 100.f, 0, 1));
        float factor = uvToSunriseSunsetColorFactor(uv);

        if(minInDay <= 360)
        {
            factor *= (minInDay - 300) / 60.f;
        }
        else if(minInDay <= 380)
        {
            factor *= 1.f;
        }
        else
        {
            factor *= (440 - minInDay) / 60.f;
        }

        return mix(baseColor, getSunRiseColor(minInDay), factor);
    }
    else if(minInDay < 1020) // 7:30 AM - 5 PM
    {
        return dayColor;
    }
    else if(minInDay <= 1140) // 5 PM - 7 PM (Sunset)
    {
        vec4 baseColor;
        if(minInDay < 1050)
        {
            baseColor = mix(dayColor, nightcolor, ((minInDay - 1020) / 30.f) * 0.4f);
        }
        else if(minInDay < 1120)
        {
            baseColor = mix(nightcolor, dayColor, 0.6f - ((minInDay - 1050) / 70.f) * 0.6f);
        }
        else
        {
            baseColor = nightcolor;
        }

        float factor = uvToSunriseSunsetColorFactor(uv);

        if(minInDay <= 1070)
        {
            factor *= ((minInDay - 1020) / 50.f);
        }
        else if(minInDay <= 1120)
        {
            factor *= ((1120 - minInDay) / 50.f);
        }
        else
        {
            factor = 0;
        }

        return mix(baseColor, getSunSetColor(minInDay), factor);
    }
    else    // 7 PM - 12 PM
    {
        return nightcolor;
    }
}

vec4 getSunColor()
{
    int minInDay = u_GameTimeMin % 1440;
    if(minInDay < 380)
    {
        return sunYellowColor;
    }
    else if(minInDay < 420)
    {
        return mix(sunYellowColor, sunColor, (minInDay - 380) / 40.f);
    }
    else if(minInDay < 1020)
    {
        return sunColor;
    }
    else if(minInDay < 1120)
    {
        return mix(sunColor, sunYellowColor, (minInDay - 1020) / 100.f);
    }
    else
    {
        return sunYellowColor;
    }
}

void main()
{
    vec2 ndc = (gl_FragCoord.xy / vec2(u_Dimensions)) * 2.0 - 1.0; // -1 to 1 NDC
    vec4 p = vec4(ndc.xy, 1, 1); // Pixel at the far clip plane
    p *= 1000.0; // Times far clip plane value
    p = /*Inverse of*/ u_ViewProj * p; // Convert from unhomogenized screen to world
    vec3 rayDir = normalize(p.xyz - u_camEye.xyz);
    vec2 uv = sphereToUV(rayDir);

    // Sky base color
    outColor = getSkyColor(uv);

    // Sun
    vec3 sunDir = u_Offset.xyz;
    float sunSize = 16.f;
    float sunInnerSize = 10.f;
    float sunAngle = acos(dot(rayDir, sunDir)) * 360.0 / PI;
    vec4 sunCol = getSunColor();
    if(sunAngle < sunSize)
    {
        if(sunAngle < sunInnerSize)
        {
            outColor = sunCol;
        }
        else
        {
            outColor = mix(sunCol, outColor, (sunAngle - sunInnerSize) / (sunSize - sunInnerSize));
        }
    }

    // Moon
    vec3 moonDir = -u_Offset.xyz;
    float moonSize = 12.f;
    float moonInnerSize = 10.f;
    float moonAngle = acos(dot(rayDir, moonDir)) * 360.0 / PI;
    if(moonAngle < moonSize)
    {
        if(moonAngle < moonInnerSize)
        {
            outColor = moonColor;
        }
        else
        {
            outColor = mix(moonColor, outColor, (moonAngle - moonInnerSize) / (moonSize - moonInnerSize));
        }
    }

    // Weather effect
    if(u_Weather[0] == 1)
    {
        outColor.rgb *= (1 - u_Weather[1] * 0.7f);
    }
}
