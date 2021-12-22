#version 150
// ^ Change this to version 130 if you have compatibility issues

//This is a vertex shader. While it is called a "shader" due to outdated conventions, this file
//is used to apply matrix transformations to the arrays of vertex data passed to it.
//Since this code is run on your GPU, each vertex is transformed simultaneously.
//If it were run on your CPU, each vertex would have to be processed in a FOR loop, one at a time.
//This simultaneous transformation allows your program to run much faster, especially when rendering
//geometry with millions of vertices.

uniform vec4 u_camEye;  // Camera world position

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself
uniform float u_Time;

in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec4 vs_Col;             // Use the first two channels for UV, the third channel is used for fs_AnimFlag

uniform vec4 u_Offset;      // Use this channel to store sun direction

out vec4 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
out vec2 fs_UV;

out vec4 fs_OriginalPos;

out float fs_AnimFlag;

out float fs_shade;

out vec4 fs_ViewDir;


//const vec4 lightDir = normalize(vec4(0.5, 1, 0.75, 0));  // The direction of our virtual light, which is used to compute the shading of
                                        // the geometry in the fragment shader.

float random1(vec3 p) {
    return fract(sin(dot(p,vec3(127.1, 311.7, 191.999)))
                 *43758.5453);
}

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

vec4 distortVert(vec4 vertPos)
{
    vec3 vert = vec3(vertPos.x / 3.f, vertPos.y / 3.f, vertPos.z / 3.f);
    float tmp = (fractalPerlin2D(vert.xz + 100.f + u_Time * 0.16f, 0.5f, 4, 1.f, 0.5f) * 0.5f + 0.5f);
    vert.x += (fractalPerlin2D(vert.xz + 200.f + u_Time * 0.16f, 0.5f, 4, 1.f, 0.5f) * 0.5f + 0.5f);
    vert.z += tmp;

    float noiseVal = fractalPerlin2D(vert.xz, 0.5f, 4, 1.f, 0.5f) * 0.5f + 0.2f;
    vertPos.y -= noiseVal;
    return vertPos;
}

void main()
{
    fs_OriginalPos = vs_Pos;
    fs_Pos = vs_Pos;
    fs_UV = vs_Col.xy;
    fs_AnimFlag = vs_Col.z;
    fs_shade = vs_Col.w;

    vec3 currNor = vec3(vs_Nor);


    // Water wave
    if(abs(fs_AnimFlag - 1.f) < 0.00001f)
    {
        fs_Pos = distortVert(vs_Pos);
    }

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);          // Pass the vertex normals to the fragment shader for interpolation.
                                                            // Transform the geometry's normals by the inverse transpose of the
                                                            // model matrix. This is necessary to ensure the normals remain
                                                            // perpendicular to the surface after the surface is transformed by
                                                            // the model matrix.

    vec4 modelposition = u_Model * fs_Pos;   // Temporarily store the transformed vertex positions for use below


    fs_ViewDir = normalize(u_camEye - modelposition);

    fs_LightVec = u_Offset;

    gl_Position = u_ViewProj * modelposition;// gl_Position is a built-in variable of OpenGL which is
                                             // used to render the final positions of the geometry's vertices
}
