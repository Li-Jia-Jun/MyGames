#include "uicrosshair.h"

UICrosshair::UICrosshair(OpenGLContext *context) : Drawable(context)
{}

GLenum UICrosshair::drawMode()
{
    return GL_LINES;
}

void UICrosshair::createVBOdata()
{
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> col;
    std::vector<GLint> idx;

    // Draw a white crosshair at screen central
    pos.push_back(glm::vec4(0.02, 0.0, 0, 1));
    pos.push_back(glm::vec4(-0.02, 0.0, 0, 1));
    pos.push_back(glm::vec4(0, 0.03, 0, 1));
    pos.push_back(glm::vec4(0, -0.03, 0, 1));
    col.push_back(glm::vec4(1, 1, 1, 1));
    col.push_back(glm::vec4(1, 1, 1, 1));
    col.push_back(glm::vec4(1, 1, 1, 1));
    col.push_back(glm::vec4(1, 1, 1, 1));
    idx.push_back(0);
    idx.push_back(1);
    idx.push_back(2);
    idx.push_back(3);

    m_count = 4;

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufPos);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufCol);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}
