#include "modelloader.h"
#include <random>
#include<iostream>
ModelLoader::ModelLoader(OpenGLContext *context)
    :Drawable(context), vertPos(), faces()
{}
ModelLoader::ModelLoader(OpenGLContext *context,std::vector<glm::vec3> pos, std::vector<std::vector<int>> f)
    :Drawable(context), vertPos(pos), faces(f)
{}

void ModelLoader::setData(std::vector<glm::vec3> p, std::vector<std::vector<int>> f,
                          std::vector<glm::vec3> n, std::vector<std::vector<int>> fn,
                          std::vector<glm::vec2> t, std::vector<std::vector<int>> ft,
                          glm::vec3 pos, int oid, int type)
{
    this->vertPos = p;
    this->faces = f;
    this->normals = n;
    this->faceNormals = fn;
    this->textures = t;
    this->faceTextures = ft;
    this->m_pos = pos;
    this->id = oid;
    this->type = type;
}
void ModelLoader::createVBOdata()
{
    std::vector<GLuint> idx;
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> nor;
    std::vector<glm::vec4> col;
    /*
    Note: the 3rd element of col is used as the index for the object position
    */


    for (unsigned int i=0; i<faces.size();i++)
    {
        idx.push_back(i*3);
        idx.push_back(i*3+1);
        idx.push_back(i*3+2);
        pos.push_back( glm::vec4(vertPos[faces.at(i).at(0)-1], 1.f));
        pos.push_back( glm::vec4(vertPos[faces.at(i).at(1)-1], 1.f));
        pos.push_back( glm::vec4(vertPos[faces.at(i).at(2)-1], 1.f));
        nor.push_back( glm::vec4(normals[faceNormals.at(i).at(0)-1], 1.f));
        nor.push_back( glm::vec4(normals[faceNormals.at(i).at(1)-1], 1.f));
        nor.push_back( glm::vec4(normals[faceNormals.at(i).at(2)-1], 1.f));
        col.push_back(glm::vec4(textures[faceTextures.at(i).at(0)-1], id, type));
        col.push_back(glm::vec4(textures[faceTextures.at(i).at(1)-1], id, type));
        col.push_back(glm::vec4(textures[faceTextures.at(i).at(2)-1], id, type));
    }

    m_count = idx.size();
    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    // Tell OpenGL that we want to perform subsequent operations on the VBO referred to by bufIdx
    // and that it will be treated as an element array buffer (since it will contain triangle indices)
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    // Pass the data stored in cyl_idx into the bound buffer, reading a number of bytes equal to
    // SPH_IDX_COUNT multiplied by the size of a GLuint. This data is sent to the GPU to be read by shader programs.
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
    // array buffers rather than element array buffers, as they store vertex attributes like position.
    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}
