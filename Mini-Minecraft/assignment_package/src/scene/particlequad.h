#ifndef PARTICLEQUAD_H
#define PARTICLEQUAD_H

#include <drawable.h>
#include <vector>


// Particle for rain or snow drops
class ParticleQuad : public TransparentDrawable
{
public:
    ParticleQuad(OpenGLContext *context);

    virtual void createVBOdata();

    void createParticleVBOdata(int type, std::vector<glm::vec4> &offsets);
};

#endif // PARTICLEQUAD_H
