#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H


#include "shaderprogram.h"
#include "player.h"
#include <vector>

class Particle : public TransparentDrawable
{
public:
    Particle(OpenGLContext *context);

    virtual void createVBOdata();

public:

    float lifeTime;

    bool isAlive;

    float m_startTime;
    glm::vec3 m_startPos;
    glm::vec3 m_startVel;
    glm::vec2 m_uv;
};


// Generate particle based on player's position and velocity
class ParticleSystem
{
public:
    ParticleSystem();

    void drawParticles(ShaderProgram& program);

    void init(OpenGLContext *context, Player* player);

    virtual void update();

    virtual bool checkShouldRespawn() = 0;

    virtual glm::vec3 getParticleStartPos() = 0;
    virtual glm::vec3 getParticleStartVel() = 0;

protected:
    int particleNum;
    int particleMaxNum;
    float particleLife;
    float timeConstrain;
    glm::vec2 particleUV;

    Player *player;

private:
    float lastSpawnTime;
    int currNum;
    std::vector<Particle*> m_particles;
};

// Dirt particles when player walks on ground
class DirtParticleSystem : public ParticleSystem
{
public:
    DirtParticleSystem();
    virtual glm::vec3 getParticleStartPos() override;
    virtual glm::vec3 getParticleStartVel() override;
    virtual bool checkShouldRespawn() override;
    virtual void update() override;
};

// Spark particles during player flight mode
class FlySparkParticleSystem : public ParticleSystem
{
public:
    FlySparkParticleSystem();
    virtual glm::vec3 getParticleStartPos() override;
    virtual glm::vec3 getParticleStartVel() override;
    virtual bool checkShouldRespawn() override;
    virtual void update() override;
};

#endif // PARTICLESYSTEM_H
