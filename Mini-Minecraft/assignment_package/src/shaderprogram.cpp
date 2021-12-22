#include "shaderprogram.h"
#include <QFile>
#include <QStringBuilder>
#include <QTextStream>
#include <QDebug>
#include <stdexcept>

#include <iostream>

ShaderProgram::ShaderProgram()
{

}

ShaderProgram::ShaderProgram(OpenGLContext *context)
    : vertShader(), fragShader(), prog(),
      attrPos(-1), attrNor(-1), attrCol(-1), attrUV(-1),
      unifModel(-1), unifModelInvTr(-1), unifViewProj(-1), unifColor(-1),
      unifTexture(-1), unifNormalMap(-1), unifRenderedTexture(-1), unifDimensions(-1), unifTime(-1), unifGameTimeMin(-1),
      unifCamEye(-1), unifFogCol(-1), unifOffset(-1), unifWeather(-1),
      unifObjPos(-1), unifObjRot(-1), unifJoints(-1),
      context(context)
{}

void ShaderProgram::create(const char *vertfile, const char *fragfile)
{
    // Allocate space on our GPU for a vertex shader and a fragment shader and a shader program to manage the two
    vertShader = context->glCreateShader(GL_VERTEX_SHADER);
    fragShader = context->glCreateShader(GL_FRAGMENT_SHADER);
    prog = context->glCreateProgram();
    // Get the body of text stored in our two .glsl files
    QString qVertSource = qTextFileRead(vertfile);
    QString qFragSource = qTextFileRead(fragfile);

    char* vertSource = new char[qVertSource.size()+1];
    strcpy(vertSource, qVertSource.toStdString().c_str());
    char* fragSource = new char[qFragSource.size()+1];
    strcpy(fragSource, qFragSource.toStdString().c_str());


    // Send the shader text to OpenGL and store it in the shaders specified by the handles vertShader and fragShader
    context->glShaderSource(vertShader, 1, (const char**)&vertSource, 0);
    context->glShaderSource(fragShader, 1, (const char**)&fragSource, 0);
    // Tell OpenGL to compile the shader text stored above
    context->glCompileShader(vertShader);
    context->glCompileShader(fragShader);
    // Check if everything compiled OK
    GLint compiled;
    context->glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(vertShader);
    }
    context->glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(fragShader);
    }

    // Tell prog that it manages these particular vertex and fragment shaders
    context->glAttachShader(prog, vertShader);
    context->glAttachShader(prog, fragShader);
    context->glLinkProgram(prog);

    // Check for linking success
    GLint linked;
    context->glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        printLinkInfoLog(prog);
    }

    // Get the handles to the variables stored in our shaders
    // See shaderprogram.h for more information about these variables

    attrPos = context->glGetAttribLocation(prog, "vs_Pos");
    attrNor = context->glGetAttribLocation(prog, "vs_Nor");
    attrCol = context->glGetAttribLocation(prog, "vs_Col");
    if(attrCol == -1) attrCol = context->glGetAttribLocation(prog, "vs_ColInstanced");
    attrPosOffset = context->glGetAttribLocation(prog, "vs_OffsetInstanced");
    attrUV = context->glGetAttribLocation(prog, "vs_UV");

    unifModel      = context->glGetUniformLocation(prog, "u_Model");
    unifModelInvTr = context->glGetUniformLocation(prog, "u_ModelInvTr");
    unifViewProj   = context->glGetUniformLocation(prog, "u_ViewProj");
    unifColor      = context->glGetUniformLocation(prog, "u_Color");

    unifTexture    = context->glGetUniformLocation(prog, "u_Texture");
    unifNormalMap  = context->glGetUniformLocation(prog, "u_NormalMap");

    unifRenderedTexture = context->glGetUniformLocation(prog, "u_RenderedTexture");
    unifDimensions = context->glGetUniformLocation(prog, "u_Dimensions");

    unifTime       = context->glGetUniformLocation(prog, "u_Time");
    unifGameTimeMin= context->glGetUniformLocation(prog, "u_GameTimeMin");

    unifCamEye     = context->glGetUniformLocation(prog, "u_camEye");

    unifFogCol     = context->glGetUniformLocation(prog, "u_fogColor");

    unifOffset     = context->glGetUniformLocation(prog, "u_Offset");

    unifWeather    = context->glGetUniformLocation(prog, "u_Weather");

    unifObjPos = context->glGetUniformLocation(prog, "u_ObjPos");
    unifObjRot = context->glGetUniformLocation(prog, "u_ObjRot");
    unifJoints = context->glGetUniformLocation(prog, "u_Joints");
}

void ShaderProgram::useMe()
{
    context->glUseProgram(prog);
}

void ShaderProgram::setModelMatrix(const glm::mat4 &model)
{
    useMe();

    if (unifModel != -1) {
        // Pass a 4x4 matrix into a uniform variable in our shader
                        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModel,
                        // How many matrices to pass
                           1,
                        // Transpose the matrix? OpenGL uses column-major, so no.
                           GL_FALSE,
                        // Pointer to the first element of the matrix
                           &model[0][0]);
    }

    if (unifModelInvTr != -1) {
        glm::mat4 modelinvtr = glm::inverse(glm::transpose(model));
        // Pass a 4x4 matrix into a uniform variable in our shader
                        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModelInvTr,
                        // How many matrices to pass
                           1,
                        // Transpose the matrix? OpenGL uses column-major, so no.
                           GL_FALSE,
                        // Pointer to the first element of the matrix
                           &modelinvtr[0][0]);
    }
}

void ShaderProgram::setViewProjMatrix(const glm::mat4 &vp)
{
    // Tell OpenGL to use this shader program for subsequent function calls
    useMe();

    if(unifViewProj != -1) {
    // Pass a 4x4 matrix into a uniform variable in our shader
                    // Handle to the matrix variable on the GPU
    context->glUniformMatrix4fv(unifViewProj,
                    // How many matrices to pass
                       1,
                    // Transpose the matrix? OpenGL uses column-major, so no.
                       GL_FALSE,
                    // Pointer to the first element of the matrix
                       &vp[0][0]);
    }
}

void ShaderProgram::setGeometryColor(glm::vec4 color)
{
    useMe();

    if(unifColor != -1)
    {
        context->glUniform4fv(unifColor, 1, &color[0]);
    }
}

void ShaderProgram::setTexture(int texSlot)
{
    useMe();

    if(unifTexture != -1)
    {
        context->glUniform1i(unifTexture, texSlot);
    }
}
void ShaderProgram::setNormalMap(int texSlot)
{
    useMe();

    if(unifNormalMap != -1)
    {
        context->glUniform1i(unifNormalMap, texSlot);
    }
}

void ShaderProgram::setDimensions(glm::ivec2 dims)
{
    useMe();

    if(unifDimensions != -1)
    {
        context->glUniform2i(unifDimensions, dims.x, dims.y);
    }
}

void ShaderProgram::setTime(float time)
{
    useMe();

    if(unifTime != -1)
    {       
        context->glUniform1f(unifTime, time);
    }
}

void ShaderProgram::setGameTimeMin(int min)
{
    useMe();

    if(unifGameTimeMin != -1)
    {
        context->glUniform1i(unifGameTimeMin, min);
    }
}

void ShaderProgram::setCamEye(glm::vec4 eye)
{
    useMe();

    if(unifCamEye != -1)
    {
        context->glUniform4fv(unifCamEye, 1, &eye[0]);
    }
}

void ShaderProgram::setFogColor(glm::vec4 col)
{
    useMe();

    if(unifFogCol != -1)
    {
        context->glUniform4fv(unifFogCol, 1, &col[0]);
    }
}

void ShaderProgram::setOffset(glm::vec4 offset)
{
    useMe();

    if(unifOffset != -1)
    {
        context->glUniform4fv(unifOffset, 1, &offset[0]);
    }
}

void ShaderProgram::setWeather(glm::vec4 weather)
{
    useMe();

    if(unifWeather != -1)
    {
        context->glUniform4fv(unifWeather, 1, &weather[0]);
    }
}

void ShaderProgram::setObjPos(glm::vec4 pos[])
{
    useMe();
    if (unifObjPos != -1)
    {
        context->glUniform4fv(unifObjPos, 200, &pos[0][0]);
    }
}

void ShaderProgram::setObjRot(float rads[])
{
    useMe();
    if (unifObjRot != -1)
    {
        context->glUniform1fv(unifObjRot, 200, &rads[0]);
    }
}


void ShaderProgram::setJoints(glm::vec4 *joints)
{
    useMe();
    if (unifJoints != -1)
    {
        context->glUniform4fv(unifJoints, 200, &joints[0][0]);
    }
}

//This function, as its name implies, uses the passed in GL widget
void ShaderProgram::draw(Drawable &d)
{
    useMe();

    if(d.elemCount() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(d.elemCount()) + "!");
    }

    // Each of the following blocks checks that:
    //   * This shader has this attribute, and
    //   * This Drawable has a vertex buffer for this attribute.
    // If so, it binds the appropriate buffers to each attribute.

    // Remember, by calling bindPos(), we call
    // glBindBuffer on the Drawable's VBO for vertex position,
    // meaning that glVertexAttribPointer associates vs_Pos
    // (referred to by attrPos) with that VBO
    if (attrPos != -1 && d.bindPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrNor != -1 && d.bindNor()) {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrCol != -1 && d.bindCol()) {
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 0, NULL);
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindIdx();

    // Skip the next line
    context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);

    context->printGLErrorLog();
}

void ShaderProgram::draw(Drawable &d, int textureSlot)
{
    useMe();

    // Set our "renderedTexture" sampler to user Texture Unit 0
    context->glUniform1i(unifRenderedTexture, textureSlot);

    // Each of the following blocks checks that:
    //   * This shader has this attribute, and
    //   * This Drawable has a vertex buffer for this attribute.
    // If so, it binds the appropriate buffers to each attribute.

    if (attrPos != -1 && d.bindPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
    }
    if (attrUV != -1 && d.bindUV()) {
        context->glEnableVertexAttribArray(attrUV);
        context->glVertexAttribPointer(attrUV, 2, GL_FLOAT, false, 0, NULL);
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindIdx();
    context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrUV != -1) context->glDisableVertexAttribArray(attrUV);

    context->printGLErrorLog();
}

void ShaderProgram::drawInstanced(InstancedDrawable &d)
{
    useMe();

    if(d.elemCount() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(d.elemCount()) + "!");
    }

    // Each of the following blocks checks that:
    //   * This shader has this attribute, and
    //   * This Drawable has a vertex buffer for this attribute.
    // If so, it binds the appropriate buffers to each attribute.

    // Remember, by calling bindPos(), we call
    // glBindBuffer on the Drawable's VBO for vertex position,
    // meaning that glVertexAttribPointer associates vs_Pos
    // (referred to by attrPos) with that VBO
    if (attrPos != -1 && d.bindPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
        context->glVertexAttribDivisor(attrPos, 0);
    }

    if (attrNor != -1 && d.bindNor()) {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
        context->glVertexAttribDivisor(attrNor, 0);
    }

    if (attrCol != -1 && d.bindCol()) {
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol, 3, GL_FLOAT, false, 0, NULL);
        context->glVertexAttribDivisor(attrCol, 1);
    }

    if (attrPosOffset != -1 && d.bindOffsetBuf()) {
        context->glEnableVertexAttribArray(attrPosOffset);
        context->glVertexAttribPointer(attrPosOffset, 3, GL_FLOAT, false, 0, NULL);
        context->glVertexAttribDivisor(attrPosOffset, 1);
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindIdx();
    context->glDrawElementsInstanced(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0, d.instanceCount());
    context->printGLErrorLog();

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);
    if (attrPosOffset != -1) context->glDisableVertexAttribArray(attrPosOffset);

}

void ShaderProgram::drawInterleavedVBO(Drawable &d)
{
    useMe();

    if(d.elemCount() < 0)
    {
        //throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(d.elemCount()) + "!");
        return;
    }

    if(d.bindPos())
    {
        if (attrPos != -1)
        {
            context->glEnableVertexAttribArray(attrPos);
            context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 3 * sizeof(glm::vec4), (void*)0);
        }

        if (attrCol != -1)
        {
            context->glEnableVertexAttribArray(attrCol);
            context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 3 * sizeof(glm::vec4), (void*)sizeof(glm::vec4));
        }

        if (attrNor != -1)
        {
            context->glEnableVertexAttribArray(attrNor);
            context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 3 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
        }
    }


    d.bindIdx();
    context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);

    context->printGLErrorLog();
}

void ShaderProgram::drawTransparentVBO(TransparentDrawable &d)
{
    useMe();

    if(d.transparentElementCount() < 0)
    {
        //throw std::out_of_range("Attempting to draw a TransparentDrawable with m_count of " +
        //                        std::to_string(d.transparentElementCount()) + "!");
        return;
    }

    if(d.bindCol()) // Transparent data stores in color channel
    {
        if (attrPos != -1)
        {
            context->glEnableVertexAttribArray(attrPos);
            context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 3 * sizeof(glm::vec4), (void*)0);
        }

        if (attrCol != -1)
        {
            context->glEnableVertexAttribArray(attrCol);
            context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 3 * sizeof(glm::vec4), (void*)sizeof(glm::vec4));
        }

        if (attrNor != -1)
        {
            context->glEnableVertexAttribArray(attrNor);
            context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 3 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
        }
    }


    d.bindTransIdx();
    context->glDrawElements(d.drawMode(), d.transparentElementCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);

    context->printGLErrorLog();
}

void ShaderProgram::renderInterleavedVBO(Drawable &d, bool render)
{
    useMe();

    if(d.elemCount() < 0)
    {
        throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(d.elemCount()) + "!");
    }

    if(d.bindPos())
    {
        if (attrPos != -1)
        {
            context->glEnableVertexAttribArray(attrPos);
            context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 3 * sizeof(glm::vec4), (void*)0);
        }

        if (attrCol != -1)
        {
            context->glEnableVertexAttribArray(attrCol);
            context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 3 * sizeof(glm::vec4), (void*)sizeof(glm::vec4));
        }

        if (attrNor != -1)
        {
            context->glEnableVertexAttribArray(attrNor);
            context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 3 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
        }
    }


    d.bindIdx();
    if (render)
    {
        context->glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);
    }

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);

    context->printGLErrorLog();
}

void ShaderProgram::renderTransparentVBO(TransparentDrawable &d, bool render)
{
    useMe();

    if(d.transparentElementCount() < 0)
    {
        throw std::out_of_range("Attempting to draw a TransparentDrawable with m_count of " +
                                std::to_string(d.transparentElementCount()) + "!");
    }

    if(d.bindCol()) // Transparent data stores in color channel
    {
        if (attrPos != -1)
        {
            context->glEnableVertexAttribArray(attrPos);
            context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 3 * sizeof(glm::vec4), (void*)0);
        }

        if (attrCol != -1)
        {
            context->glEnableVertexAttribArray(attrCol);
            context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 3 * sizeof(glm::vec4), (void*)sizeof(glm::vec4));
        }

        if (attrNor != -1)
        {
            context->glEnableVertexAttribArray(attrNor);
            context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 3 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
        }
    }


    d.bindTransIdx();
    if (render)
    {
        context->glDrawElements(d.drawMode(), d.transparentElementCount(), GL_UNSIGNED_INT, 0);
    }

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);

    context->printGLErrorLog();
}

char* ShaderProgram::textFileRead(const char* fileName) {
    char* text;

    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");

        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);

            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';	//cap off the string with a terminal symbol, fixed by Cory
            }
            fclose(file);
        }
    }
    return text;
}

QString ShaderProgram::qTextFileRead(const char *fileName)
{
    QString text;
    QFile file(fileName);
    if(file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
        text = in.readAll();
        text.append('\0');
    }
    return text;
}

void ShaderProgram::printShaderInfoLog(int shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0)
    {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
        qDebug() << "ShaderInfoLog:" << "\n" << infoLog << "\n";
        delete [] infoLog;
    }

    // should additionally check for OpenGL errors here
}

void ShaderProgram::printLinkInfoLog(int prog)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        qDebug() << "LinkInfoLog:" << "\n" << infoLog << "\n";
        delete [] infoLog;
    }
}
