#version 150
// ^ Change this to version 130 if you have compatibility issues


uniform mat4 u_Model;

uniform mat4 u_ModelInvTr;

uniform mat4 u_ViewProj;

uniform vec4 u_Offset;
uniform float u_Time;

uniform vec4 u_ObjPos[100];
uniform float u_ObjRot[100];
uniform vec3 u_Joints[100];


in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec4 vs_Col;             // Use the first two channels for UV, the other two are not in used yet

out vec4 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
out vec2 fs_UV;


const vec4 lightDir = normalize(vec4(0.5, 1, 0.75, 0));  // The direction of our virtual light, which is used to compute the shading of

mat4 rotateX(float theta)
{
    return mat4(
                vec4(1.f, 0.f, 0.f, 0.f),
                vec4(0.f, cos(theta), sin(theta), 0.f),
                vec4(0.f, -sin(theta), cos(theta), 0.f),
                vec4(0.f, 0.f, 0.f, 1.f));
}
mat4 rotateY(float theta)
{
    return mat4(
                vec4(cos(theta), 0.f, -sin(theta), 0.f),
                vec4(0.f, 1.f, 0.f, 0.f),
                vec4(sin(theta), 0.f, cos(theta), 0.f),
                vec4(0.f, 0.f, 0.f, 1.f));
}
mat4 rotateZ(float theta)
{
    return mat4(
                vec4(cos(theta), -sin(theta), 0.f, 0.f),
                vec4(sin(theta), cos(theta), 0.f, 0.f),
                vec4(0.f, 0.f, 1.f, 0.f),
                vec4(0.f, 0.f, 0.f, 1.f));
}
void main()
{
    fs_Pos = vs_Pos;
    fs_UV = vs_Col.xy;
    // id is the index to the position array of the objects
    int id = int(vs_Col.z);
    // type is the object type
    int type = int(vs_Col.w);


    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);          // Pass the vertex normals to the fragment shader for interpolation.
                                                            // Transform the geometry's normals by the inverse transpose of the
                                                            // model matrix. This is necessary to ensure the normals remain
                                                            // perpendicular to the surface after the surface is transformed by
                                                            // the model matrix.

    vec4 modelposition = u_Model * vs_Pos;   // Temporarily store the transformed vertex positions for use below


    // Robots
    if (type == 3)
    {
       if (modelposition.y < u_Joints[id].y)
        {
           modelposition.y -= u_Joints[id].y;

           if (modelposition.x < u_Joints[id].x)
           {
            modelposition.x -= u_Joints[id].x;
            modelposition = rotateX(0.5*cos(5* u_Time)) * modelposition;
            modelposition.x += u_Joints[id].x;

           }
            else
           {
            modelposition.x -= u_Joints[id].x;
            modelposition = rotateX(-0.5*cos(5*u_Time)) * modelposition;
            modelposition.x += u_Joints[id].x;
           }
            modelposition.y += u_Joints[id].y;
        }
    }
    // Birds
    else if (type == 4)
    {
           if (modelposition.x < u_Joints[id].y)
           {
            modelposition.x -= u_Joints[id].x;
            modelposition = rotateZ(0.1*cos(5* u_Time)) * modelposition;
            modelposition.x += u_Joints[id].x;

           }
            else if(modelposition.x > u_Joints[id].z)
           {
            modelposition.x -= u_Joints[id].x;
            modelposition = rotateZ(-0.1*cos(5*u_Time)) * modelposition;
            modelposition.x += u_Joints[id].x;
           }
    }
    // T-REX
    else if (type == 5)
    {
            if (modelposition.x < u_Joints[id].y)
            {
             modelposition.x -= u_Joints[id].x;
             modelposition = rotateX(0.5*cos(5* u_Time)) * modelposition;
             modelposition.x += u_Joints[id].x;

            }
            else if(modelposition.x > u_Joints[id].z)
            {
             modelposition.x -= u_Joints[id].x;
             modelposition = rotateX(-0.5*cos(5*u_Time)) * modelposition;
             modelposition.x += u_Joints[id].x;
            }
    }


    float radians = u_ObjRot[id];
    // Render offset
     modelposition = rotateY(radians) * modelposition ;

    modelposition += u_ObjPos[id];

    fs_LightVec = (lightDir);  // Compute the direction in which the light source lies

    gl_Position = u_ViewProj * modelposition;// gl_Position is a built-in variable of OpenGL which is
                                             // used to render the final positions of the geometry's vertices
}
