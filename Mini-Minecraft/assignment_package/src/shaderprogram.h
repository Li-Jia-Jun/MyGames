#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <openglcontext.h>
#include <glm_includes.h>
#include <glm/glm.hpp>

#include "drawable.h"

enum Postprocess : unsigned char
{
    NOOPERATION, INWATER, INLAVA
};

class ShaderProgram
{
public:
    GLuint vertShader; // A handle for the vertex shader stored in this shader program
    GLuint fragShader; // A handle for the fragment shader stored in this shader program
    GLuint prog;       // A handle for the linked shader program stored in this class

    int attrPos; // A handle for the "in" vec4 representing vertex position in the vertex shader
    int attrNor; // A handle for the "in" vec4 representing vertex normal in the vertex shader
    int attrCol; // A handle for the "in" vec4 representing vertex color in the vertex shader
    int attrPosOffset; // A handle for a vec3 used only in the instanced rendering shader
    int attrUV; // A handle for the "in" vec2 representing uv position for a texture

    int unifModel; // A handle for the "uniform" mat4 representing model matrix in the vertex shader
    int unifModelInvTr; // A handle for the "uniform" mat4 representing inverse transpose of the model matrix in the vertex shader
    int unifViewProj; // A handle for the "uniform" mat4 representing combined projection and view matrices in the vertex shader
    int unifColor; // A handle for the "uniform" vec4 representing color of geometry in the vertex shader

    int unifTexture;
    int unifNormalMap;

    int unifRenderedTexture; // A handle for "uniform" sampler2D representing the rendered texture
    int unifDimensions; // A handle for the "uniform" vec2 representing the pixel size of the rendered texture

    int unifTime;
    int unifGameTimeMin;

    int unifCamEye;

    int unifFogCol;

    int unifOffset;

    int unifWeather;

    int unifObjPos;     // the position of the objects
    int unifObjRot;     // the rotation angles of the objects
    int unifJoints;     // the joints of the objects

public:
    /// Never use the default constructor. The default constructor
    /// is defined so that it can be used as the value of unordered_map
    ShaderProgram();
    ShaderProgram(OpenGLContext* context);
    // Sets up the requisite GL data and shaders from the given .glsl files
    void create(const char *vertfile, const char *fragfile);
    // Tells our OpenGL context to use this shader to draw things
    void useMe();
    // Pass the given model matrix to this shader on the GPU
    void setModelMatrix(const glm::mat4 &model);
    // Pass the given Projection * View matrix to this shader on the GPU
    void setViewProjMatrix(const glm::mat4 &vp);
    // Pass the given color to this shader on the GPU
    void setGeometryColor(glm::vec4 color);

    void setTexture(int texSlot);
    void setNormalMap(int texSlot);
    void setDimensions(glm::ivec2 dims);

    void setTime(float time);
    void setGameTimeMin(int min);

    void setCamEye(glm::vec4 eye);

    void setFogColor(glm::vec4 col);

    void setOffset(glm::vec4 offset);

    void setWeather(glm::vec4 weather);

    void setObjPos(glm::vec4 pos[]);
    void setObjRot(float rads[]);
    void setJoints(glm::vec4 joints[]);


    // Draw the given object to our screen using this ShaderProgram's shaders
    void draw(Drawable &d);
    // Draw the given object to our screen using this ShaderProgram's shaders
    void draw(Drawable &d, int textureSlot);
    // Draw the given object to our screen multiple times using instanced rendering
    void drawInstanced(InstancedDrawable &d);
    // Draw the given object interleaved VBO
    void drawInterleavedVBO(Drawable &d);
    // Draw the given transparent object
    void drawTransparentVBO(TransparentDrawable &d);

    // Draw the given object interleaved VBO
    void renderInterleavedVBO(Drawable &d, bool render);
    // Draw the given transparent object
    void renderTransparentVBO(TransparentDrawable &d, bool render);

    // Utility function used in create()
    char* textFileRead(const char*);
    // Utility function that prints any shader compilation errors to the console
    void printShaderInfoLog(int shader);
    // Utility function that prints any shader linking errors to the console
    void printLinkInfoLog(int prog);

    QString qTextFileRead(const char*);

private:
    OpenGLContext* context;   // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                            // we need to pass our OpenGL context to the Drawable in order to call GL functions
                            // from within this class.
};

#endif // SHADERPROGRAM_H
