#ifndef CLOUDPLANE_H
#define CLOUDPLANE_H

#include <drawable.h>
#include "texture.h"

class CloudPlane : public TransparentDrawable
{
public:
    CloudPlane(OpenGLContext *context);
    virtual void createVBOdata();

    void setPlayerPos(glm::vec3 playerPos);
    glm::vec3 getPlayerPos();

private:
    glm::vec3 m_playerPos;  // The orignal player position
};

#endif // CLOUDPLANE_H
