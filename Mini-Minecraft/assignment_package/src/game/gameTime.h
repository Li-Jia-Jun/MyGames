#ifndef GAMETIME_H
#define GAMETIME_H


class GameTime
{
public:
    const static int TIMESCALE = 1440 / 10;     // Gametime =  m_dt * TIMESCALE
                                                // For example, if TIMESCALE = 60, then 1 sec in real world equals to 1 min in game
                                                // When we set it to 1440 = 60*24, then 1 min in real world equals to 24 hours in game

    const static int startGameTime = 15;         // Game time of the first day in hour when the program starts


public:

    static float time;                          // Read time since the start of the program

    static int getGameTimeInMin();

    static int getTotalGameTimeInHour();
};

#endif 
