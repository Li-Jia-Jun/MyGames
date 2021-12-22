#version 150
// ^ Change this to version 130 if you have compatibility issues

uniform mat4 u_Model;       // To pass view matrix for billboard effect
uniform mat4 u_ViewProj;

uniform float u_Time;

uniform vec4 u_Weather;    // To pass weather info

in vec4 vs_Pos;
in vec4 vs_Col;
in vec4 vs_Nor;             // xy stores UV local offset, zw stores player position in X-Z plane

out vec2 fs_UV;
out float fs_distance;
out float fs_weatherState;


float myRandom(vec2 p)
{
    //return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 12345.6789);
}

void main()
{
    fs_UV = vs_Col.xy;
    fs_distance = abs(vs_Nor.x) + abs(vs_Nor.y);
    fs_weatherState = u_Weather.x;

    float weatherFlag = u_Weather.x;
    float weatherRatio = u_Weather.y;

    //vec4 modelposition = u_Model * vs_Pos;
    vec4 modelposition = vs_Pos;

    float n1 = myRandom(vs_Nor.xy + vs_Nor.zw);
    float n2 = myRandom(vs_Nor.yx + vs_Nor.wz);


    // Initial offset
    modelposition += vec4(vs_Nor.x + vs_Nor.z, 0, vs_Nor.y + vs_Nor.w, 0);

    // Rain fall
    if(abs(weatherFlag - 1) < 0.001f || abs(weatherFlag - 2) < 0.001f)
    {
        float uBound = 345.f;
        float lBound = 130.f;
        float speed = (0.8f + n2 * 0.5f) * 50.f;

        float offsetY = n1 * 20.f + uBound;
        float move = -(int(u_Time * speed) % 100) * 0.01f;

        offsetY += move * (uBound - lBound);

        modelposition += vec4(0, offsetY, 0, 0);
    }

    // Billboard(in world space)
    vec4 camUp = u_Model * vec4(0, 1, 0, 0);
    vec4 camRight = u_Model * vec4(1, 0, 0, 0);
    float biasX = vs_Pos.x * 2.f; // vs_Pos spans from -0.5 to 0.5
    float biasY = vs_Pos.y * 2.f;
    modelposition += camRight * biasX + camUp * biasY;

    //built-in things to pass down the pipeline
    gl_Position = u_ViewProj * modelposition;
}
