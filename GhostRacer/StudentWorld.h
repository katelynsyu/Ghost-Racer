#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <iostream>
#include <string>
#include <vector>

class Racer;
class Actor;
class HolyWaterProjectile;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    virtual ~StudentWorld();
    Racer* getRacer() { return m_racer; }
    
    bool determineOverlap (Actor* a, Actor* b);
    bool checkPlayerDead();
    void createNewHolyWaterProjectile (double startX, double startY, double direction);
    void createNewHealingGoodie (double startX, double startY);
    
    bool isHolyWaterProjectileActive (HolyWaterProjectile* a);
    
    int determineActorNearby (Actor* a, int lane);
    
    bool actorInLane(int lane, int location);
    
private:
    Racer* m_racer;
    std::vector<Actor*> m_actors;
    double m_lastWhiteY;
    int m_bonus;
};

#endif // STUDENTWORLD_H_
