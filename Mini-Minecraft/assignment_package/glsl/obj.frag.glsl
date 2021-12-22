#version 150
// ^ Change this to version 130 if you have compatibility issues

uniform vec4 u_camEye;

uniform vec4 u_fogColor;

uniform vec4 u_Color; // Sunlight color

uniform vec4 u_Weather;

uniform sampler2D u_Texture;

in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec2 fs_UV;
in float fs_Type;

out vec4 out_Col;


const vec4 distanceFogColor = vec4(0.9, 0.9, 0.9, 0.0);
const vec4 dayColor = vec4(1, 1, 1, 0.8);
const vec4 nightColor = vec4(0.1, 0.1, 0.1, 0.05);


float distanceFog(float distance)
{
    float maxDistance = 500;
    float minDistance = 200;

    if(abs(fs_Type - 11) < 0.001f)
    {
        maxDistance = 1000;
        minDistance = 0;
    }

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

void main()
{
    vec2 uv = fs_UV;

    vec4 diffuseColor = texture(u_Texture, uv);

    // Diffuse
    vec4 dirNor = normalize(fs_Nor);
    vec4 dirSun = normalize(fs_LightVec);
    float diffuseTerm = dot(dirNor, dirSun) * 0.6f;
    float moonDiffuseTerm = dot(dirNor, -dirSun) * 0.1f;    // Moon is at the opposite direction of sun
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
    float ambientTerm = 0.3f;

    if(fs_Type == 0)
    {
        ambientTerm = 0.2f;
    }

    float lightIntensity = diffuseTerm + moonDiffuseTerm + ambientTerm;

    // Weather effect
    if(u_Weather[0] == 1)
    {
        lightIntensity *= (1 - u_Weather[1] * 0.4f);
    }

    // Compute final shaded color
    vec4 outCol = vec4(diffuseColor.rgb * u_Color.rgb * lightIntensity, diffuseColor.a);

    // Distance fog
    float d = distance(fs_Pos, u_camEye);
    float fogFactor = distanceFog(d);
    outCol = mix(outCol,u_fogColor, fogFactor);

    out_Col = outCol;
}
