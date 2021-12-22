#version 150
// ^ Change this to version 130 if you have compatibility issues

uniform mat4 u_Model;       // To pass view matrix
uniform mat4 u_ViewProj;    // To pass projection martix

uniform float u_Time;


in vec4 vs_Pos;         // player start position, start time
in vec4 vs_Col;         // player start velocity, life time
in vec4 vs_Nor;         // initial postion x y, UV

out vec2 fs_UV;

out vec2 fs_alpha;


float myRandom(vec3 p)
{
   return fract(sin((dot(p, vec3(127.1, 311.7, 191.999)))) * 43758.5453);
}

void main()
{
    fs_UV = vs_Nor.zw;
    fs_alpha.x = 1.f - (u_Time - vs_Pos.w) / vs_Col.w;

    float scale = 0.4f;

    mat4 projMat = u_ViewProj;
    mat4 viewMat = u_Model;
    mat4 modelMat = mat4(scale, 0.0, 0.0, 0.0,    // 1. column
                      0.0, scale, 0.0, 0.0,       // 2. column
                      0.0, 0.0, scale, 0.0,       // 3. column
                      vs_Pos.x, vs_Pos.y, vs_Pos.z, 1.0); // 4. column

    // Initial position
    vec4 modelposition = vec4(vs_Nor.xy, 0, 1.f);

    // Rotation
    float r = u_Time * 15.f + myRandom(vs_Pos.zyx) * 10.f;
    float cosr = cos(r), sinr = sin(r);
    mat4 rotMat = mat4(cosr, sinr, 0, 0,
                       -sinr, cosr, 0, 0,
                       0, 0, 1, 0,
                       0, 0, 0, 1);
    modelposition = rotMat * modelposition;


    // Offset to player position
   // modelposition += vec4(vs_Pos.xyz, 0.f);
    modelposition = modelMat * modelposition;

    // Moves in opposite player moving direction
    vec3 moveDir = -normalize(vs_Col.xyz);
    float moveSpeed = 2.5f + 1.2f * myRandom(vs_Pos.xyz);
    modelposition += vec4(moveDir * moveSpeed * (u_Time - vs_Pos.w), 0.f);

    // Bounce
    //modelposition += vec4(0.f, 0.25 * abs(sin(u_Time * 35.f)), 0.f, 0.f);

    // Billboard
    vec4 world_origin = modelMat * vec4(0, 0, 0, 1.f);
    vec4 view_origin = viewMat * modelMat * vec4(0, 0, 0, 1.f);
    vec4 view_pos = modelposition - world_origin + view_origin;

    gl_Position = projMat * view_pos;
}




