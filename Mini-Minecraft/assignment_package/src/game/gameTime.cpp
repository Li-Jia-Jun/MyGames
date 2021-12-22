#include "gameTime.h"

float GameTime::time = 0;

int GameTime::getGameTimeInMin()
{
    return int(60 * startGameTime + time * TIMESCALE / 60.f);
}

int GameTime::getTotalGameTimeInHour()
{
    return int(startGameTime + time * TIMESCALE / 3600.f);
}
