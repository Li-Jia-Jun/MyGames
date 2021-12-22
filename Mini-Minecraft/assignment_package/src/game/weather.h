#ifndef WEATHER_H
#define WEATHER_H

#include "shaderprogram.h"
#include "glm_includes.h"
#include "scene/particlequad.h"

#include <array>
#include<QtMultimedia/QSound>
#include <QDir>

enum WeatherState
{
    Sunny = 0,
    Rainy = 1,
    GoldenRain = 2
};

// Manage sun movement and weather states
class Weather
{
public:

    Weather(OpenGLContext* context);

    void initialize();

    void update();

    WeatherState getWeatherState();
    float getWeatherRatio();

    glm::vec4 getSunDir();
    glm::vec4 getSunLightColor();
    glm::vec4 getFogColor();

    // Rain functions
    bool startRain(bool isGolden = false);
    void drawRain(ShaderProgram &shader, glm::vec3 playerPos);

private:    
    WeatherState m_state;

    // Rain members
    int m_rainStartTimeMin;
    float m_rainRatio;

    ParticleQuad *m_particle;
    QSound* rainSound;
    OpenGLContext *mp_context;
};


#endif // WEATHER_H
