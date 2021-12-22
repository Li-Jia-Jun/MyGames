#include "particlesystem.h"
#include "game/gameTime.h"
#include "utils/noise.h"

#include <iostream>
using namespace std;


Particle::Particle(OpenGLContext *context) : TransparentDrawable(context), m_startTime(-100.f)
{}

void Particle::createVBOdata()
{
    std::vector<glm::vec4> transAttrs(12);

    transAttrs[0] = glm::vec4(m_startPos, m_startTime);
    transAttrs[1] = glm::vec4(m_startVel, lifeTime);
    transAttrs[2] = glm::vec4(-0.5f, -0.5f, m_uv.x/16.f, m_uv.y/16.f);

    transAttrs[3] = glm::vec4(m_startPos, m_startTime);
    transAttrs[4] = glm::vec4(m_startVel, lifeTime);
    transAttrs[5] = glm::vec4(0.5f, -0.5f, (m_uv.x+1.f)/16.f, m_uv.y/16.f);

    transAttrs[6] = glm::vec4(m_startPos, m_startTime);
    transAttrs[7] = glm::vec4(m_startVel, lifeTime);
    transAttrs[8] = glm::vec4(0.5f, 0.5f, (m_uv.x+1.f)/16.f, (m_uv.y+1)/16.f);

    transAttrs[9] = glm::vec4(m_startPos, m_startTime);
    transAttrs[10] = glm::vec4(m_startVel, lifeTime);
    transAttrs[11] = glm::vec4(-0.5f, 0.5f, m_uv.x/16.f, (m_uv.y+1)/16.f);

    GLuint idx[6] {0, 1, 2, 0, 2, 3};

    m_TransCount = 6;

    generateTransparentIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TransBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), idx, GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(glm::vec4), transAttrs.data(), GL_STATIC_DRAW);
}

//////////////////////////////////////////////////////////

ParticleSystem::ParticleSystem() : lastSpawnTime(-100.f), currNum(0)
{}

void ParticleSystem::init(OpenGLContext *context, Player* player)
{
    for(int i = 0; i < particleNum; i++)
    {
        m_particles.push_back(new Particle(context));
    }

    this->player = player;
}

void ParticleSystem::update()
{
    for(auto particle : m_particles)
    {
        // Respawn particle
        if(!particle->isAlive)
        {
            // Self constrain
            if(!checkShouldRespawn())
                continue;

            // Count constrain
            if(currNum >= particleMaxNum)
                continue;

            // Time constrain
            if(GameTime::time - lastSpawnTime < timeConstrain)
                continue;

            lastSpawnTime = GameTime::time;
            currNum++;

            particle->isAlive = true;
            particle->m_startPos = getParticleStartPos();
            particle->m_startVel = getParticleStartVel();
            particle->m_startTime = GameTime::time;
            particle->lifeTime = particleLife;
            particle->m_uv = particleUV;

            particle->createVBOdata();
        }
        // Kill particle
        else if(GameTime::time - particle->m_startTime > particle->lifeTime)
        {
            currNum--;

            particle->isAlive = false;
            particle->destroyVBOdata();
        }
    }
}

void ParticleSystem::drawParticles(ShaderProgram& program)
{
    for(auto particle : m_particles)
    {
        if(particle->isAlive)
        {
            program.drawTransparentVBO(*particle);
        }
    }
}

///////////////////////////////////////////////////////////

DirtParticleSystem::DirtParticleSystem()
{
    particleNum = 3;
    particleLife = 0.2f;/*0.1f;*/
    timeConstrain = 0.04f;
    particleUV = glm::vec2(14, 4);
}

glm::vec3 DirtParticleSystem::getParticleStartPos()
{
    glm::vec3 playerPos = player->mcr_position;
    //playerPos -= glm::vec3(0, 0.1f, 0);

    //vec3 noise = Noise::random3(playerPos + vec3(GameTime::time)) * 0.6f;
    //noise.y *= 0.2f;
    //playerPos += noise;

    return playerPos;
}

glm::vec3 DirtParticleSystem::getParticleStartVel()
{
    glm::vec3 vel = player->getVelocity();
    vel[1] = 0.f;
    return vel;
}

bool DirtParticleSystem::checkShouldRespawn()
{
    // On ground constarin
    if(!player->isOnGround())
        return false;

    // Velocity constrain
    glm::vec3 vel = player->getVelocity();
    vel[1] = 0;
    if(glm::length(vel) < 5.f)
        return false;

    return true;
}

void DirtParticleSystem::update()
{
    particleMaxNum = int(particleNum * glm::clamp(glm::length(player->getVelocity()) / 8.f, 0.f, 1.f));
    ParticleSystem::update();
}

/////////////////////////////////////////////////////////////////

FlySparkParticleSystem::FlySparkParticleSystem()
{
    particleNum = 50;
    particleLife = 0.2f;
    timeConstrain = 0.01f;
    particleUV = glm::vec2(15, 4);
}

glm::vec3 FlySparkParticleSystem::getParticleStartPos()
{
    glm::vec3 playerPos = player->mcr_position;
    playerPos += glm::vec3(0, 0.8f, 0);
    playerPos += Noise::random3(playerPos) * 1.2f;
    return playerPos;
}

glm::vec3 FlySparkParticleSystem::getParticleStartVel()
{
    glm::vec3 vel = player->getVelocity();
    vel += Noise::random3(vel) * 0.4f;
    return vel;
}

bool FlySparkParticleSystem::checkShouldRespawn()
{
    if(!player->flightModeOn)
        return false;

    if(player->firstPerson)
        return false;

    glm::vec3 vel = player->getVelocity();
    if(glm::length(vel) < 3.f)
        return false;

    return true;
}

void FlySparkParticleSystem::update()
{
    particleMaxNum = int(particleNum * glm::clamp(glm::length(player->getVelocity()) / 40.f, 0.f, 1.f));
    ParticleSystem::update();
}


