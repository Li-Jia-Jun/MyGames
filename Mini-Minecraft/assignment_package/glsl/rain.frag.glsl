#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

uniform sampler2D u_Texture;

in vec2 fs_UV;
in float fs_distance;
in float fs_weatherState;

out vec4 out_Col;

void main()
{
    vec4 col = texture(u_Texture, fs_UV);

    if(col.w == 0.f)
        discard;


    if(abs(fs_weatherState - 1) <= 0.001f)
    {
        col.a *= 0.3f;
    }

    else if(abs(fs_weatherState - 2) <= 0.001f)
    {
        col.rgb *= 1.5f;
        col.a *= 0.5f;
    }

    // Alpha changed based on distance
    col *= ((1000.f - fs_distance) / 1000.f);

    out_Col = col;
}
