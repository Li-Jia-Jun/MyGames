#ifndef MODELLOADER_H
#define MODELLOADER_H
#include<vector>
#include "drawable.h"
#include "glm_includes.h"


class ModelLoader : public Drawable
{
    std::vector<glm::vec3> vertPos;
    std::vector<std::vector<int>> faces;
    std::vector<glm::vec3> normals;
    std::vector<std::vector<int>> faceNormals;
    std::vector<glm::vec2> textures;
    std::vector<std::vector<int>> faceTextures;
    glm::vec3 m_pos;    // position of the model
    int id; // The id of the object
    int type;   // The type of the object

public:
    ModelLoader(OpenGLContext *context);

    ModelLoader(OpenGLContext *context, std::vector<glm::vec3> pos, std::vector<std::vector<int>> f);
    void setData(std::vector<glm::vec3> p, std::vector<std::vector<int>> f,
                 std::vector<glm::vec3> n, std::vector<std::vector<int>> fn,
                 std::vector<glm::vec2> t, std::vector<std::vector<int>> ft,
                 glm::vec3 m_pos, int oid, int type);
    void createVBOdata() override;
};

#endif // MODELLOADER_H
