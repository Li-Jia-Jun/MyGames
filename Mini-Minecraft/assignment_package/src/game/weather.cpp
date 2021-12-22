#include "weather.h"
#include "gameTime.h"
#include "utils/noise.h"
#include <iostream>

Weather::Weather(OpenGLContext *context) :
    m_state(WeatherState::Sunny), m_rainStartTimeMin(0), m_rainRatio(0),
    rainSound(new QSound(QDir::currentPath().append(QString(" ../../assignment_package/sound_files/rain.wav")))),
  mp_context(context)
{}

WeatherState Weather::getWeatherState()
{
    return m_state;
}

float Weather::getWeatherRatio()
{
    if(m_state == WeatherState::Rainy)
    {
        return m_rainRatio;
    }
    else if(m_state == WeatherState::GoldenRain)
    {
        return m_rainRatio;
    }
    else if(m_state == WeatherState::Sunny)
    {
        return 0;
    }
    else
    {
        return 0;
    }
}

void Weather::initialize()
{
    m_particle = new ParticleQuad(mp_context);
}

void Weather::update()
{  
    int min = GameTime::getGameTimeInMin();
    if(m_state == Rainy || m_state == GoldenRain)
    {
        // A rain cicle:
        // 0 - 60 min rain builds up
        // 60 - 180 min full rain
        // 180 - 210 min rain fades out
        int rainMin = min - m_rainStartTimeMin;
        if(rainMin <= 60)
        {
            m_rainRatio = rainMin / 60.f;
        }
        else if(rainMin <= 180)
        {
            m_rainRatio = 1.f;
        }
        else if(rainMin <= 210)
        {
            m_rainRatio = (210 - rainMin) / 30.f;
        }
        else
        {
            m_state = WeatherState::Sunny;
            m_rainRatio = -1.f;
            this->rainSound->stop();
        }
    }
}

glm::vec4 Weather::getSunDir()
{
    float minPerAngle = 0.25f;                      // 1440 minutes equals 360 degree, so 1 min equals 360/1440=0.25 degree)
    glm::vec4 startDir = glm::vec4(0, -1, 0, 0);    // Sun position in 12 PM(0 AM)

    int minInDay = GameTime::getGameTimeInMin() % 1440;

    glm::mat4 rot = glm::mat4(1.f);
    rot = glm::rotate(rot, glm::radians(minPerAngle * minInDay), glm::vec3(0, 0, 1));

    return glm::normalize(rot * startDir + glm::vec4(0, 0, 0.2f, 0));
}

glm::vec4 Weather::getSunLightColor()
{
    static glm::vec4 orangeColor = glm::vec4(249, 219, 105, 255) / 255.f;
    static glm::vec4 normalColor = glm::vec4(1, 1, 1, 1);

    int minInDay = GameTime::getGameTimeInMin() % 1440;
    if(minInDay < 300)
    {
        return normalColor;
    }
    else if(minInDay <= 360)
    {
        return glm::mix(normalColor, orangeColor, ((minInDay - 300) / 60.f) * 0.8f);
    }
    else if(minInDay <= 400)
    {
        return orangeColor;
    }
    else if(minInDay <= 450)
    {
        return glm::mix(normalColor, orangeColor, ((450 - minInDay) / 50.f) * 0.8f);
    }
    else if(minInDay <= 1020)
    {
        return normalColor;
    }
    else if(minInDay <= 1080)
    {
        return glm::mix(normalColor, orangeColor, ((minInDay - 1020) / 60.f) * 0.8f);
    }
    else if(minInDay <= 1140)
    {
        return glm::mix(orangeColor, normalColor, ((minInDay - 1080) / 60.f) * 0.8f);
    }
    else
    {
        return normalColor;
    }
}

glm::vec4 Weather::getFogColor()
{
    static glm::vec4 fogDayColor = vec4(225, 225, 225, 255) / 255.f;
    static glm::vec4 fogNightColor = vec4(0, 0, 21, 255) / 255.f;

    glm::vec4 baseColor;

    int minInDay = GameTime::getGameTimeInMin() % 1440;
    if(minInDay < 330.f)
    {
        baseColor = fogNightColor;
    }
    else if(minInDay < 450.f)
    {
        float factor = (minInDay - 330.f) / 120.f;
        vec4 baseColor = mix(fogNightColor, fogDayColor, factor);
        if(minInDay < 390.f)
        {
            baseColor = mix(baseColor, getSunLightColor(), factor);
        }
        else
        {
            baseColor = mix(baseColor, getSunLightColor(), 1 - factor);
        }
    }
    else if(minInDay < 1020)
    {
        baseColor = fogDayColor;
    }
    else if(minInDay < 1140)
    {
        float factor = (minInDay - 1020.f) / 120.f;
        vec4 baseColor = mix(fogDayColor, fogNightColor, factor);
        if(minInDay < 1080.f)
        {
            baseColor = mix(baseColor, getSunLightColor(), factor);
        }
        else
        {
            baseColor = mix(baseColor, getSunLightColor(), 1 - factor);
        }
    }
    else
    {
        baseColor = fogNightColor;
    }

    // Rain effect
    if(m_state == WeatherState::Rainy)
    {
        baseColor *= (1 - m_rainRatio * 0.6f);
    }

    return baseColor;
}


void Weather::drawRain(ShaderProgram &shader, glm::vec3 playerPos)
{
    playerPos.x = int(playerPos.x);
    playerPos.z = int(playerPos.z);

    int radius = 256;  // About 5000 quads
    float dense[3] = {2.f, 5.f, 8.f};
    float dist[3] = {20.f, 60.f, float(radius + 1)};

    std::vector<glm::vec4> offsets;
    for(float x = -radius; x < radius;)
    {
        for(float z = -radius; z < radius;)
        {
            if(Noise::random1(vec2(x, z)) < m_rainRatio - 0.3f)
            {
                offsets.push_back(glm::vec4(x, z, playerPos.x, playerPos.z));
            }                  

            for(int i = 0; i < 3; i++)
            {
                if(abs(z) < dist[i])
                {
                    z += dense[i];
                    break;
                }
            }
        }

        for(int i = 0; i < 3; i++)
        {
            if(abs(x) < dist[i])
            {
                x += dense[i];
                break;
            }
        }
    }

    bool isGolden = m_state == WeatherState::GoldenRain;
    m_particle->createParticleVBOdata(isGolden ? WeatherState::GoldenRain : WeatherState::Rainy, offsets);
    shader.drawTransparentVBO(*m_particle);
}

bool Weather::startRain(bool isGolden)
{
    if(m_state != WeatherState::Sunny)
        return false;

    if(isGolden)
    {
         m_state = WeatherState::GoldenRain;
    }
    else
    {
         m_state = WeatherState::Rainy;
    }

    m_rainStartTimeMin = GameTime::getGameTimeInMin();
    this->rainSound->play();
    return true;
}
