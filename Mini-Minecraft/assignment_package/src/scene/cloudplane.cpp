#include "cloudplane.h"


CloudPlane::CloudPlane(OpenGLContext *context) : TransparentDrawable(context)
{}

void CloudPlane::createVBOdata()
{
    // Only create VBO once but update u_Offset in shaderprogram
    // to 'move' the cloud plane follwing player

    GLuint idx[6] {0, 1, 2, 0, 2, 3};

    // Position, (uv, 0, 0), normal
    float len = 1024.f;
    float height = 350.f;
    glm::vec4 transAttrs[12] {glm::vec4(m_playerPos[0] - len, height, m_playerPos[2] - len, 1.f),
                           glm::vec4(0.f, 0.f, 0.f, 0.f), glm::vec4(0.f, 1.f, 0.f, 1.f),

                           glm::vec4(m_playerPos[0] + len, height, m_playerPos[2] - len, 1.f),
                           glm::vec4(1.f, 0.f, 0.f, 0.f), glm::vec4(0.f, 1.f, 0.f, 1.f),

                           glm::vec4(m_playerPos[0] + len, height, m_playerPos[2] + len, 1.f),
                           glm::vec4(1.f, 1.f, 0.f, 0.f), glm::vec4(0.f, 1.f, 0.f, 1.f),

                           glm::vec4(m_playerPos[0] - len, height, m_playerPos[2] + len, 1.f),
                           glm::vec4(0.f, 1.f, 0.f, 0.f), glm::vec4(0.f, 1.f, 0.f, 1.f)};

    m_TransCount = 6;
    generateTransparentIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TransBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), idx, GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(glm::vec4), transAttrs, GL_STATIC_DRAW);
}

void CloudPlane::setPlayerPos(glm::vec3 playerPos)
{
    m_playerPos = playerPos;
}

glm::vec3 CloudPlane::getPlayerPos()
{
    return m_playerPos;
}
