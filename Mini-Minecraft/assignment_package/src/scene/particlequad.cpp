#include "particlequad.h"
#include "game/gameTime.h"
#include <iostream>

ParticleQuad::ParticleQuad(OpenGLContext *context) : TransparentDrawable(context)
{}

void ParticleQuad::createVBOdata()
{}

void ParticleQuad::createParticleVBOdata(int type, std::vector<glm::vec4> &offsets)
{
    // Create 1x1 quads at (0, 0, 0)
    // Then apply offset in shader

    // Rain particle UV: (4, 6)
    glm::vec2 uv(0, 0);
    if(type == 1)
    {
        uv = glm::vec2(4, 6);
    }
    else if(type == 2)
    {
        uv = glm::vec2(12, 3);
    }


    int c1 = 0;
    int c2 = 0;
    int quadCount = offsets.size();
    std::vector<GLuint> idx(6 * quadCount);
    std::vector<glm::vec4> transAttrs(12 * quadCount);
    for(int i = 0; i < quadCount; i++)
    {
        transAttrs[c1] = (glm::vec4(-0.5f, -0.5f, 0, 1.f));
        transAttrs[c1 + 1] = (glm::vec4(uv.x/16.f, uv.y/16.f, 0.f, 0.f));
        transAttrs[c1 + 2] = (offsets[i]);

        transAttrs[c1 + 3] = (glm::vec4(0.5f, -0.5f, 0, 1.f));
        transAttrs[c1 + 4] = (glm::vec4((uv.x+1.f)/16.f, uv.y/16.f, 0.f, 0.f));
        transAttrs[c1 + 5] = (offsets[i]);

        transAttrs[c1 + 6] = (glm::vec4(0.5f, 0.5f, 0, 1.f));
        transAttrs[c1 + 7] = (glm::vec4((uv.x+1.f)/16.f, (uv.y+1)/16.f, 0.f, 0.f));
        transAttrs[c1 + 8] = (offsets[i]);

        transAttrs[c1 + 9] = (glm::vec4(-0.5f, 0.5f, 0, 1.f));
        transAttrs[c1 + 10] = (glm::vec4(uv.x/16.f, (uv.y+1)/16.f, 0.f, 0.f));
        transAttrs[c1 + 11] = (offsets[i]);

        idx[c2] = 4 * i;
        idx[c2+1] = 4 * i + 1;
        idx[c2+2] = 4 * i + 2;
        idx[c2+3] = 4 * i;
        idx[c2+4] = 4 * i + 2;
        idx[c2+5] = 4 * i + 3;

        c1 += 12;
        c2 += 6;
    }

    m_TransCount = idx.size();
    generateTransparentIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TransBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, transAttrs.size() * sizeof(glm::vec4), transAttrs.data(), GL_STATIC_DRAW);
}
