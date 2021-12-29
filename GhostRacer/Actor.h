#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;

static const double PI = 4 * atan(1.0);
const int MAX_SHIFT_PER_TICK = 4.0;
const double VERTICAL_SPEED = -4.0;

const int LEFT_LANE = 1;
const int MIDDLE_LANE = 2;
const int RIGHT_LANE = 3;

const int TOP = 1;
const int BOTTOM = 2;

const int LEFT_EDGE = ROAD_CENTER - (ROAD_WIDTH/2);
const int RIGHT_EDGE = ROAD_CENTER + (ROAD_WIDTH/2);
const int LEFT_WHITE_DIVIDER = LEFT_EDGE+(ROAD_WIDTH/3);
const int RIGHT_WHITE_DIVIDER = RIGHT_EDGE-(ROAD_WIDTH/3);

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor: public GraphObject {
public:
    Actor (StudentWorld* world, int imageID, double startX, double startY, int dir, double size, unsigned int depth, int health = 1, bool collisionAvoidance = true, double vSpeed = VERTICAL_SPEED, double hSpeed = 0, bool affectedByHolyWater = false)
    //figure out how to attach a value to m_studentWorld
    : GraphObject (imageID, startX, startY, dir, size, depth)
    {
        m_health = health;
        m_collisionAvoidanceWorthy = collisionAvoidance;
        m_isAlive = true;
        m_studentWorld = world;
        m_vSpeed = vSpeed;
        m_hSpeed = hSpeed;
        m_affectedByHolyWater = affectedByHolyWater;
    }
    
    virtual void doSomething() = 0;
    
    int getHealth() { return m_health; }
    void changeHealth (int amt) { m_health += amt; }
    
    double getVSpeed() { return m_vSpeed; }
    void changeVSpeed(double amt) { m_vSpeed += amt; }
    double getHSpeed() { return m_hSpeed; }
    void changeHSpeed(double amt) { m_hSpeed += amt; }
    
    bool getAlive() { return m_isAlive; }
    void setAlive(bool value) { m_isAlive = value;}
    
    bool isAffectedByHolyWater() { return m_affectedByHolyWater; }
    
    bool getCollisionAvoidance() { return m_collisionAvoidanceWorthy; }
    
    StudentWorld* getWorld() {return m_studentWorld;}
    
    bool hasGoneOffScreen (Actor* a); //checks if the actor has gone off screen
    void moveAlgorithm(Actor* a); //the move algorithm for most actors
    void checkHealth(Actor* a); //checks if the health is zero, and if so sets actor to not alive;
    
    virtual void hitByHolyWater() {};
            
private:
    int m_health;
    bool m_collisionAvoidanceWorthy;
    bool m_isAlive;
    StudentWorld* m_studentWorld;
    double m_vSpeed;
    double m_hSpeed;
    bool m_affectedByHolyWater;
};

class Pedestrian: public Actor {
public:
    Pedestrian (StudentWorld* world, int imageID, double startX, double startY, double size)
    : Actor (world, imageID, startX, startY, 0, size, 0, 2, true, VERTICAL_SPEED, 0, true)
    {
        m_movementPlanDist = 0;
    }
    
    virtual void doSomething() = 0;
    
    int getMovementPlanDist() { return m_movementPlanDist; }
    void changeMovementPlanDist(int amt) { m_movementPlanDist += amt; }
    
    void createNewMovementPlan(); //creates a new movement plan for pedestrians
    
private:
    int m_movementPlanDist;
};

class HumanPedestrian: public Pedestrian {
public:
    HumanPedestrian (StudentWorld* world, double startX, double startY)
    : Pedestrian (world, IID_HUMAN_PED, startX, startY, 2.0)
    {}
    
    virtual void doSomething();
    virtual void hitByHolyWater();
private:
    
};

class ZombiePedestrian: public Pedestrian {
public:
    ZombiePedestrian (StudentWorld* world, double startX, double startY)
    : Pedestrian (world, IID_ZOMBIE_PED, startX, startY, 3.0)
    {
        m_timeToNextGrunt = 20;
    }
    
    virtual void doSomething();
    virtual void hitByHolyWater();
private:
    int m_timeToNextGrunt;
};

class ZombieCab: public Actor {
public:
    ZombieCab (StudentWorld* world, double startX, double startY, double vSpeed)
    : Actor (world, IID_ZOMBIE_CAB, startX, startY, 90, 4.0, 0, 3, true, vSpeed, 0, true)
    {
        m_movementPlanDist = 0;
        m_hasDamagedRacer = false;
    }
    
    virtual void doSomething();
    virtual void hitByHolyWater();
private:
    int m_movementPlanDist;
    bool m_hasDamagedRacer;
    
};

class Goodie: public Actor {
public:
    Goodie (StudentWorld* world, int imageID, double startX, double startY, double size, bool affectedByHolyWater)
    : Actor (world, imageID, startX, startY, 0, size, 2, 1, false, VERTICAL_SPEED, 0, affectedByHolyWater)
    {}
    
    virtual void doSomething() = 0;    
    void gotGoodie (Actor* a, int scoreIncrease);
private:
};

class HealingGoodie: public Goodie {
public:
    HealingGoodie (StudentWorld* world, double startX, double startY)
    : Goodie(world, IID_HEAL_GOODIE, startX, startY, 1.0, true)
    {}
    
    virtual void doSomething();
    virtual void hitByHolyWater();
private:
};

class HolyWaterRefill: public Goodie {
public:
    HolyWaterRefill (StudentWorld* world, double startX, double startY)
    : Goodie(world, IID_HOLY_WATER_GOODIE, startX, startY, 2.0, true)
    {}
    
    virtual void doSomething();
    virtual void hitByHolyWater();
private:
};

class LostSouls: public Goodie {
public:
    LostSouls (StudentWorld* world, double startX, double startY)
    : Goodie(world, IID_SOUL_GOODIE, startX, startY, 4.0, false)
    {}
    
    virtual void doSomething();
private:
};

class HolyWaterProjectile: public Actor {
public:
    HolyWaterProjectile (StudentWorld* world, double startX, double startY, int dir)
    : Actor (world, IID_HOLY_WATER_PROJECTILE, startX, startY, dir, 1.0, 1, 1, false, 0, 0, false)
    {
        m_maxTravelDist = 160;
        m_travelDist = 0;
        m_isActive = false;
    }
    
    virtual void doSomething();
    void setIsActive(bool value) { m_isActive = value; }
        
private:
    int m_maxTravelDist;
    int m_travelDist;
    bool m_isActive;
};

class OilSlick: public Actor {
public:
    OilSlick (StudentWorld* world, double startX, double startY, double size)
    : Actor (world, IID_OIL_SLICK, startX, startY, 0, size, 1, 0, false, VERTICAL_SPEED, 0, false)
    {}
    
    virtual void doSomething();
private:
};

class BorderLines: public Actor {
public:
    BorderLines (StudentWorld* world, int imageID, double startX, double startY)
    : Actor (world, imageID, startX, startY, 0, 2.0, 1, 0, false, VERTICAL_SPEED, 0, false)
    {}
    
    virtual void doSomething();
private:
};

class Racer: public Actor { 
public:
    Racer(StudentWorld* world)
    : Actor (world, IID_GHOST_RACER, 128, 32, 90, 4.0, 0, 100, true, 0, 0, false)
    {
        m_holyWaterUnits = 10;
        m_leftBoundary = ROAD_CENTER - (ROAD_WIDTH/2);
        m_rightBoundary = ROAD_CENTER + (ROAD_WIDTH/2);
        m_savedSouls = 0;
    }
    
    virtual void doSomething();
    
    void incrementSavedSouls() { m_savedSouls++; }
    int getSavedSouls() { return m_savedSouls; }
    
    int getHolyWaterUnits() { return m_holyWaterUnits; }
    void changeHolyWaterUnits( int amt) { m_holyWaterUnits += amt; }
    
    void spin();
private:
    int m_holyWaterUnits;
    int m_leftBoundary;
    int m_rightBoundary;
    int m_savedSouls;
};
#endif // ACTOR_H_
