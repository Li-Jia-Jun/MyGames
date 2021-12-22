#version 150
// ^ Change this to version 130 if you have compatibility issues


uniform mat4 u_Model;

uniform mat4 u_ModelInvTr;

uniform mat4 u_ViewProj;

// Offset[0]: whether the player is moving， Offset[1]: whether flight mode is on
uniform vec4 u_Offset;
uniform float u_Time;

uniform vec4 u_ObjPos[100];
uniform float u_ObjRot[100];
uniform vec4 u_Joints[100];


in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

// Use the first two channels for UV, Col[3]: id of the object,Col[4]: type of the object
in vec4 vs_Col;

out vec4 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
out vec2 fs_UV;
out float fs_Type;


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
    fs_UV = vs_Col.xy;
    // id is the index to the position array of the objects
    int id = int(vs_Col.z);
    // type is the object type
    int type = int(vs_Col.w);

    fs_Type = type;


    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);          // Pass the vertex normals to the fragment shader for interpolation.
                                                            // Transform the geometry's normals by the inverse transpose of the
                                                            // model matrix. This is necessary to ensure the normals remain
                                                            // perpendicular to the surface after the surface is transformed by
                                                            // the model matrix.

    vec4 modelposition = u_Model * vs_Pos;   // Temporarily store the transformed vertex positions for use below

    // Me
    if (type == 1)
    {
        if (u_Offset.x == 1.f && u_Offset.y == 1.f)
        {
           float midLine = (u_Joints[id].x + u_Joints[id].y) / 2.f;
           float legLine = (u_Joints[id].z - u_Joints[id].w) * 0.16;
           float armLine = (u_Joints[id].z - u_Joints[id].w) * 0.45;
           float shoulderLine = (u_Joints[id].z - u_Joints[id].w) * 0.5;

           if (modelposition.y < legLine)
            {
               modelposition.y -= legLine;

               if (modelposition.x < midLine)
               {
                modelposition.x -= midLine;
                modelposition = rotateX(0.5*cos(7* u_Time)) * modelposition;
                modelposition.x += midLine;

               }
                else
               {
                modelposition.x -= midLine;
                modelposition = rotateX(-0.5*cos(7*u_Time)) * modelposition;
                modelposition.x += midLine;
               }
                modelposition.y += legLine;
            }

            if (modelposition.y < shoulderLine)
            {
               if(modelposition.x > (u_Joints[id].x + midLine) * 0.75)
               {
                   modelposition.y -= armLine;
                   modelposition = rotateX(0.7*cos(7* u_Time)) * modelposition;
                   modelposition.y += armLine;

               }
               else if(modelposition.x < (u_Joints[id].y + midLine) * 0.75)
                {
                    modelposition.y -= armLine;
                    modelposition = rotateX(-0.7*cos(7* u_Time)) * modelposition;
                    modelposition.y += armLine;
                }
            }
        }
        else if (u_Offset.x == 1.f && u_Offset.y == 0.f)
        {
            float midLine = (u_Joints[id].z + u_Joints[id].w) / 2.f;
            modelposition.y -= midLine;
            modelposition = rotateX(-0.4) * modelposition;
            modelposition.y += midLine;
        }

    }


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
    // Humans
    else if (type == 5)
    {
           float midLine = (u_Joints[id].x + u_Joints[id].y) / 2.f;
           float legLine = (u_Joints[id].z - u_Joints[id].w) * 0.16;
           float armLine = (u_Joints[id].z - u_Joints[id].w) * 0.45;
           float shoulderLine = (u_Joints[id].z - u_Joints[id].w) * 0.5;

           if (modelposition.y < legLine)
            {
               modelposition.y -= legLine;

               if (modelposition.x < midLine)
               {
                modelposition.x -= midLine;
                modelposition = rotateX(0.7*cos(5* u_Time)) * modelposition;
                modelposition.x += midLine;

               }
                else
               {
                modelposition.x -= midLine;
                modelposition = rotateX(-0.7*cos(5*u_Time)) * modelposition;
                modelposition.x += midLine;
               }
                modelposition.y += legLine;
            }

        if (modelposition.y < shoulderLine)
        {
           if(modelposition.x > (u_Joints[id].x + midLine) * 0.75)
           {
               modelposition.y -= armLine;
               modelposition = rotateX(0.5*cos(5* u_Time)) * modelposition;
               modelposition.y += armLine;

           }
           else if(modelposition.x < (u_Joints[id].y + midLine) * 0.75)
            {
                modelposition.y -= armLine;
                modelposition = rotateX(-0.5*cos(5* u_Time)) * modelposition;
                modelposition.y += armLine;
            }
        }
    }
    // Eagle
    else if (type == 6)
    {
        float leftWing = (u_Joints[id].y - u_Joints[id].x) * 1.3 / 4.f + u_Joints[id].x;
        float rightWing = (u_Joints[id].y - u_Joints[id].x) * 2.7f / 4.f + u_Joints[id].x;
        float midLine = (u_Joints[id].x + u_Joints[id].y) / 2.f;
        float axis = (u_Joints[id].z + u_Joints[id].w) / 2.f;
        if (modelposition.x < leftWing)
        {
         modelposition.x -= midLine;
         modelposition.y -= axis;
         modelposition = rotateZ(0.4*cos(5* u_Time)) * modelposition;
         modelposition.x += midLine;
        modelposition.y += axis;
        }
         else if(modelposition.x > rightWing)
        {
         modelposition.x -= midLine;
        modelposition.y -= axis;
         modelposition = rotateZ(-0.4*cos(5*u_Time)) * modelposition;
         modelposition.x += midLine;
         modelposition.y += axis;

        }
    }
    // Deer
    else if (type == 7)
    {
        float legLine = u_Joints[id].w + (u_Joints[id].z - u_Joints[id].w) / 5.f;
        // Z axis
        float midLine =  (u_Joints[id].y + u_Joints[id].x) / 2.f;
        // Y axis
        float midLine2 = (u_Joints[id].w + u_Joints[id].z) / 2.f;
        if (modelposition.y < legLine)
        {
            if (modelposition.z > midLine)
            {
                if (modelposition.x > 0.1f)
                {
                   modelposition.y += 0.1f* (cos(6.5f * u_Time));
                }
                else
                {
                   modelposition.y -= 0.1f* (cos(6.5f * u_Time));
                }

                 modelposition.y -= legLine;
                 modelposition = rotateX(-0.1f*cos(13.f*u_Time)) * modelposition;
                 modelposition.y += legLine;
            }
            else
            {
                if (modelposition.x < 0.1f)
                {
                   modelposition.y += 0.1f* (cos(6.5f * u_Time));
                }
                else
                {
                   modelposition.y -= 0.1f*(cos(6.5f * u_Time));
                }
                modelposition.y -= legLine;
                modelposition = rotateX(0.1f*cos(13.f*u_Time)) * modelposition;
                modelposition.y += legLine;
            }
        }
        else
        {}
        modelposition.z -= midLine;
        modelposition.y -= midLine2;
        modelposition = rotateX(0.01f*(cos(13.f*u_Time)+0.01f)) * modelposition;
        modelposition.z += midLine;
        modelposition.y += midLine2;
        modelposition.y += 0.08f;
    }
    // Rabbit
    else if (type == 8)
    {
        modelposition = rotateX(-0.3) * modelposition;
    }
    else if (type == 9)
    {
        float radius = 1.2f;
        modelposition -= vec4(u_Joints[id].x, u_Joints[id].y,u_Joints[id].z, 0.f);
        modelposition =  vec4(mix(modelposition.xyz, normalize(modelposition.xyz) * radius, 0.5f * (cos(2.f * u_Time)+1.f)), 1.f);
        modelposition += vec4(u_Joints[id].x, u_Joints[id].y,u_Joints[id].z, 0.f);
    }
    // Me on deer
    else if (type == 10)
    {
        if (u_Offset.x == 1.f && u_Offset.y == 1.f)
        {
            float legLine = u_Joints[id].w + (u_Joints[id].z - u_Joints[id].w) / 5.f;
            // Z axis
            float midLine =  (u_Joints[id].y + u_Joints[id].x) / 2.f;
            // Y axis
            float midLine2 = (u_Joints[id].w + u_Joints[id].z) / 2.f;
            if (modelposition.y < legLine)
            {
                if (modelposition.z > midLine)
                {
                    if (modelposition.x > 0.1f)
                    {
                       modelposition.y += 0.1f* (cos(6.5f * u_Time));
                    }
                    else
                    {
                       modelposition.y -= 0.1f* (cos(6.5f * u_Time));
                    }

                     modelposition.y -= legLine;
                     modelposition = rotateX(-0.1f*cos(13.f*u_Time)) * modelposition;
                     modelposition.y += legLine;
                }
                else
                {
                    if (modelposition.x < 0.1f)
                    {
                       modelposition.y += 0.1f* (cos(6.5f * u_Time));
                    }
                    else
                    {
                       modelposition.y -= 0.1f*(cos(6.5f * u_Time));
                    }
                    modelposition.y -= legLine;
                    modelposition = rotateX(0.1f*cos(13.f*u_Time)) * modelposition;
                    modelposition.y += legLine;
                }
            }
            else
            {}
            modelposition.z -= midLine;
            modelposition.y -= midLine2;
            modelposition = rotateX(0.01f*(cos(13.f*u_Time)+0.01f)) * modelposition;
            modelposition.z += midLine;
            modelposition.y += midLine2;
            modelposition.y += 0.08f;
        }
    }
    else if (type == 11)
    {
        if (u_Time < 45.f && u_Time > 10.f)
       {
        float scale = u_Time - 10.f;
         modelposition.x *= scale;
         modelposition.y *= scale;
         modelposition.z *= scale;

        }
        else if (u_Time >= 45)
        {
        modelposition.x *= 35.f;
        modelposition.y *= 35.f;
        modelposition.z *= 35.f;
        }
    }
    else if(type == 12)
    {
        float head=(u_Joints[id].y - u_Joints[id].x) * 0.2f + u_Joints[id].x;
        if (modelposition.z > head)
        {
            modelposition.z -= head;
            modelposition.z *= 0.9f + 0.2f*(cos(3*u_Time) + 1.f);
            modelposition.z += head;
        }

    }

    float radians = u_ObjRot[id];
    // Render offset
     modelposition = rotateY(radians) * modelposition ;

    modelposition += u_ObjPos[id];

    fs_Pos = modelposition;

    fs_LightVec = (lightDir);  // Compute the direction in which the light source lies

    gl_Position = u_ViewProj * modelposition;// gl_Position is a built-in variable of OpenGL which is
                                             // used to render the final positions of the geometry's vertices
}
