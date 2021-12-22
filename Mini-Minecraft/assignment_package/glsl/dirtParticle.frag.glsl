#version 150
// ^ Change this to version 130 if you have compatibility issues

uniform sampler2D u_Texture;

in vec2 fs_UV;

out vec4 out_Col;

void main()
{
    vec4 col = texture(u_Texture, fs_UV);

    if(col.w == 0)
        discard;

    out_Col = col;
}
