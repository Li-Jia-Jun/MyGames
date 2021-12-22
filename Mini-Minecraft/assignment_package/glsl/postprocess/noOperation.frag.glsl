#version 330
// noOperation.vert.glsl:
// A fragment shader used for post-processing that simply reads the
// image produced in the first render pass by the surface shader
// and outputs it to the frame buffer


in vec2 fs_UV;

out vec4 color;

uniform sampler2D u_RenderedTexture;

void main()
{
    color = texture(u_RenderedTexture, fs_UV).rgba;
}
