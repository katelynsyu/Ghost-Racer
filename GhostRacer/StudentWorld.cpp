#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <stdlib.h>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_bonus = 0;
    m_lastWhiteY = 0;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    // set m_bonus to 5000
    m_bonus = 5000;
    
    //create a ghost racer
    m_racer = new Racer (this);
    
    //create white and yellow border lines
    int n = VIEW_HEIGHT/SPRITE_HEIGHT;
    for (int i = 0; i < n; i++)
    {
        Actor* a = new BorderLines(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, i*SPRITE_HEIGHT);
        m_actors.push_back (a);
        Actor* b = new BorderLines(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i*SPRITE_HEIGHT);
        m_actors.push_back (b);
    }
    
    int m = VIEW_HEIGHT/(4*SPRITE_HEIGHT);
    for (int i = 0; i < m; i++)
    {
        Actor* a = new BorderLines(this, IID_WHITE_BORDER_LINE, LEFT_WHITE_DIVIDER, i*(4*SPRITE_HEIGHT));
        m_actors.push_back (a);
        Actor* b = new BorderLines(this, IID_WHITE_BORDER_LINE, RIGHT_WHITE_DIVIDER, i*(4*SPRITE_HEIGHT));
        m_actors.push_back (b);
        m_lastWhiteY = i*(4*SPRITE_HEIGHT);
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //playSound(SOUND_VEHICLE_CRASH);

    ///////////// Check if GhostRacer is alive
    if (!m_racer->getAlive())
    {
        playSound(SOUND_PLAYER_DIE);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }

    ///////////// Loop through vector and ask each object to doSomething()
    m_racer->doSomething();
    vector<Actor*>:: iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        if ((*it)->getAlive())
        {
            (*it)->doSomething();
            if (m_racer->getHealth() <= 0)

            {
                m_racer->setAlive(false);
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (m_racer->getSavedSouls() >= getLevel()*2+5)
            {
                increaseScore(m_bonus);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
        it++;
    }

    /////////////// Get rid of actors that are not alive
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        if (!((*it)->getAlive()))
        {
            delete *it;
            m_actors.erase(it--);
        }
        it++;
    }

    /////////////// Potentially add new actors
    
    //Border Lines
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    
    m_lastWhiteY += -4 + m_racer->getVSpeed();
    double delta_y = new_border_y - m_lastWhiteY;
    
    if (delta_y >= SPRITE_HEIGHT)
    {
        Actor* a = new BorderLines(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER-(ROAD_WIDTH/2), new_border_y);
        m_actors.push_back (a);
        Actor* b = new BorderLines(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER+(ROAD_WIDTH/2), new_border_y);
        m_actors.push_back (b);
    }
    
    if (delta_y >= 4*SPRITE_HEIGHT)
    {
        Actor* a = new BorderLines(this, IID_WHITE_BORDER_LINE, ROAD_CENTER-(ROAD_WIDTH/2)+(ROAD_WIDTH/3), new_border_y);
        m_actors.push_back (a);
        Actor* b = new BorderLines(this, IID_WHITE_BORDER_LINE, ROAD_CENTER+(ROAD_WIDTH/2)-(ROAD_WIDTH/3), new_border_y);
        m_actors.push_back (b);
        m_lastWhiteY = new_border_y;
    }
    
    //Zombie Cabs
    int ChanceVehicle = max(100 - getLevel() * 10, 20);
    if (randInt(0, ChanceVehicle-1) == 0)
    {
        int lanes[]  = {1, 2, 3};
        vector<int> lanesUnvisited;
        lanesUnvisited.assign(lanes, lanes+3);

        int cur_lane = 0;
        int startY = 0;
        double startSpeed = 0;
        vector<int>::iterator it;
        while (!lanesUnvisited.empty())
        {
            int randIndex = randInt(0, lanesUnvisited.size()-1);
            cur_lane = lanesUnvisited[randIndex];
            it = lanesUnvisited.begin() + randIndex;
            lanesUnvisited.erase(it);

            if (actorInLane(cur_lane, BOTTOM))
            {
                startY = SPRITE_HEIGHT/2;
                startSpeed = m_racer->getVSpeed() + randInt(2, 4);
                break;
            }
            else if (actorInLane(cur_lane, TOP))
            {
                startY = VIEW_HEIGHT - SPRITE_HEIGHT/2;
                startSpeed = m_racer->getVSpeed() - randInt(2, 4);
                break;
            }
            else
                cur_lane = -1;
        }

        int startX = 0;
        if (cur_lane == LEFT_LANE)
            startX = ROAD_CENTER - ROAD_WIDTH/3;
        if (cur_lane == MIDDLE_LANE)
            startX = ROAD_CENTER;
        if (cur_lane == RIGHT_LANE)
            startX = ROAD_CENTER + ROAD_WIDTH/3;

        if (cur_lane != -1)
        {
            Actor* newZombieCab = new ZombieCab(this, startX, startY, startSpeed);
            m_actors.push_back(newZombieCab);
        }
    }
    
    //Oil Slicks
    int ChanceOilSlick = max(150 - getLevel() * 10, 40);
    if (randInt(0, ChanceOilSlick-1) == 0)
    {
        int startX = randInt(LEFT_EDGE, RIGHT_EDGE);
        int size = randInt(2, 5);
        Actor* newOilSlick = new OilSlick(this, startX, VIEW_HEIGHT, size);
        m_actors.push_back (newOilSlick);
    }
    
    //Zombie Peds
    int ChanceZombiePed = max(100 - getLevel() * 10, 20);
    if (randInt(0, ChanceZombiePed-1) == 0)
    {
        int startX = randInt(0, VIEW_WIDTH);
        Actor* newZombiePed = new ZombiePedestrian (this, startX, VIEW_HEIGHT);
        m_actors.push_back (newZombiePed);
    }
    
    //Human Peds
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    if (randInt(0, ChanceHumanPed-1) == 0)
    {
        int startX = randInt(0, VIEW_WIDTH);
        Actor* newHumanPed = new HumanPedestrian (this, startX, VIEW_HEIGHT);
        m_actors.push_back (newHumanPed);
    }
    
    //Holy Water Refills
    int ChanceOfHolyWater = 100 + 10 * getLevel();
    if (randInt(0, ChanceOfHolyWater-1) == 0)
    {
        int startX = randInt(LEFT_EDGE, RIGHT_EDGE);
        Actor* newHolyWaterRefill = new HolyWaterRefill(this, startX, VIEW_HEIGHT);
        m_actors.push_back (newHolyWaterRefill);
    }
    
    //Lost Souls
    int ChanceOfLostSoul = 100;
    if (randInt(0, ChanceOfLostSoul-1) == 0)
    {
        int startX = randInt(LEFT_EDGE, RIGHT_EDGE);
        Actor* newLostSoul = new LostSouls(this, startX, VIEW_HEIGHT);
        m_actors.push_back (newLostSoul);
    }
    
    
    /////////////// Update display text
    m_bonus--;
    int souls2save = (getLevel()*2+5)-m_racer->getSavedSouls();
    
    ostringstream oss;
    oss << "Score: " << getScore() << "  Lvl: " << getLevel() << "  Souls2Save: " << souls2save << "  Lives: " << getLives() << "  Health: " << m_racer->getHealth() << "  Sprays: " << m_racer->getHolyWaterUnits() << "  Bonus: " << m_bonus;
    string s = oss.str();
    setGameStatText(s);
    
    //decLives();
    return GWSTATUS_CONTINUE_GAME;
    
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    //return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp()
//delete all objects in m_actors and the ghost racer
{
    delete m_racer;
    vector<Actor*>:: iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        delete (*it);
        it = m_actors.erase(it);
    }
}

void StudentWorld::createNewHolyWaterProjectile(double startX, double startY, double direction)
//create a new holy water projectile at startX, startY with direction and put the new object in m_actors
{
    Actor* newProjectile = new HolyWaterProjectile(this, startX, startY, direction);
    m_actors.push_back(newProjectile);
}

void StudentWorld::createNewHealingGoodie(double startX, double startY)
//create a new healing goodie at startX, startY and put the new object in m_actors
{
    Actor* newGoodie = new HealingGoodie(this, startX, startY);
    m_actors.push_back(newGoodie);
}

bool StudentWorld::determineOverlap (Actor* a, Actor* b)
// determine if actors a and b overlap
{
    double delta_x = abs(a->getX() - b->getX());
    double delta_y = abs(a->getY() - b->getY());
    double radius_sum = a->getRadius() + b->getRadius();
    if (delta_x < radius_sum*.25 && delta_y < radius_sum*.6)
        return true;
    return false;
}

bool StudentWorld::checkPlayerDead()
// if ghost racer's health <= 0, play a sound, set to not alive, return true. else return false
{
    if (m_racer->getHealth() <= 0)
    {
        playSound(SOUND_PLAYER_DIE);
        m_racer->setAlive(false);
        return true;
    }
    return false;
}

bool StudentWorld::isHolyWaterProjectileActive(HolyWaterProjectile *a)
// if the holy water projectile overlaps with an alive actor that is affected by holy water projectiles, call the hitByHolyWater function for that actor, set the projectile to not alive and active, return true. else return false
{
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->isAffectedByHolyWater() == true && m_actors[i]->getAlive() == true && determineOverlap(a, m_actors[i]))
        {
            m_actors[i]->hitByHolyWater();
            a->setAlive(false);
            a->setIsActive(true);
            return true;
        }
    }
    return false;
}

int StudentWorld::determineActorNearby(Actor *a, int lane)
// if there is an actor within 96 pixels of the zombie cab, return TOP or BOTTOM, depending on whether the actor is above the cab or below the cab. return -1 if there is no such actor
{
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (lane == LEFT_LANE && m_actors[i]->getAlive() && m_actors[i]->getX() >= LEFT_EDGE && m_actors[i]->getX() < LEFT_WHITE_DIVIDER && m_actors[i]->getCollisionAvoidance())
        {
            int delta_y = m_actors[i]->getY() - a->getY();
            if (delta_y < 96 && delta_y > 0)
            {
                return TOP;
                // return TOP because there is an actor in ZombieCab's lane that is less than 96 pixels in front of Zombie Cab
            }
            if (delta_y > -96 && delta_y < 0)
            {
                return BOTTOM;
                // return BOTTOM because there is an actor in ZombieCab's lane that is less than 96 pixels behind Zombie Cab
            }
        }
        else if (lane == MIDDLE_LANE && m_actors[i]->getAlive() && m_actors[i]->getX() >= LEFT_WHITE_DIVIDER && m_actors[i]->getX() < RIGHT_WHITE_DIVIDER && m_actors[i]->getCollisionAvoidance())
        {
            int delta_y = m_actors[i]->getY() - a->getY();
            if (delta_y < 96 && delta_y > 0)
            {
                return TOP;
                // return 1 because there is an actor in ZombieCab's lane that is less than 96 pixels in front of Zombie Cab
            }
            if (delta_y > -96 && delta_y < 0)
            {
                return BOTTOM;
                // return 2 because there is an actor in ZombieCab's lane that is less than 96 pixels behind Zombie Cab
            }
        }
        else if (lane == RIGHT_LANE && m_actors[i]->getAlive() && m_actors[i]->getX() >= RIGHT_WHITE_DIVIDER && m_actors[i]->getX() < RIGHT_EDGE && m_actors[i]->getCollisionAvoidance())
        {
            int delta_y = m_actors[i]->getY() - a->getY();
            if (delta_y < 96 && delta_y > 0)
            {
                return TOP;
                // return 1 because there is an actor in ZombieCab's lane that is less than 96 pixels in front of Zombie Cab
            }
            if (delta_y > -96 && delta_y < 0)
            {
                return BOTTOM;
                // return 2 because there is an actor in ZombieCab's lane that is less than 96 pixels behind Zombie Cab
            }
        }
    }
    return -1;
}

bool StudentWorld::actorInLane(int lane, int location)
{
    //if trying to create a zombie cab at the bottom of a lane that ghost racer is in, return false
    if (lane == LEFT_LANE && location == BOTTOM && m_racer->getX() >= LEFT_EDGE && m_racer->getX() < LEFT_WHITE_DIVIDER)
        return false;
    if (lane == MIDDLE_LANE && location == BOTTOM && m_racer->getX() >= LEFT_WHITE_DIVIDER && m_racer->getX() < RIGHT_WHITE_DIVIDER)
        return false;
    if (lane == RIGHT_LANE && location == BOTTOM && m_racer->getX() >= RIGHT_WHITE_DIVIDER && m_racer->getX() < RIGHT_EDGE)
        return false;
    
    for (int i = 0; i < m_actors.size(); i++)
    {
        //check if there is an actor in the prospective lane that zombie cab is looking to spawn into
        if (lane == LEFT_LANE && m_actors[i]->getAlive() && m_actors[i]->getX() >= LEFT_EDGE && m_actors[i]->getX() < LEFT_WHITE_DIVIDER && m_actors[i]->getCollisionAvoidance())
        {
            // zombie cab is looking to spawn into the bottom of the lane and  there is a collision avoidance worthy actor that is close to the bottom of the lane, return false
            if (location == BOTTOM && m_actors[i]->getY() < VIEW_HEIGHT/3)
                return false;
            // zombie cab is looking to spawn into the top of the lane and  there is a collision avoidance worthy actor that is close to the top of the lane, return false
            if (location == TOP && m_actors[i]->getY() > VIEW_HEIGHT*2/3)
                return false;
        }
        if (lane == MIDDLE_LANE && m_actors[i]->getAlive() && m_actors[i]->getX() >= LEFT_WHITE_DIVIDER && m_actors[i]->getX() < RIGHT_WHITE_DIVIDER && m_actors[i]->getCollisionAvoidance())
        {
            if (location == BOTTOM && m_actors[i]->getY() < VIEW_HEIGHT/3)
                return false;
            if (location == TOP && m_actors[i]->getY() > VIEW_HEIGHT*2/3)
                return false;
        }
        if (lane == RIGHT_LANE && m_actors[i]->getAlive() && m_actors[i]->getX() >= RIGHT_WHITE_DIVIDER && m_actors[i]->getX() < RIGHT_EDGE && m_actors[i]->getCollisionAvoidance())
        {
            if (location == BOTTOM && m_actors[i]->getY() < VIEW_HEIGHT/3)
                return false;
            if (location == TOP && m_actors[i]->getY() > VIEW_HEIGHT*2/3)
                return false;
        }
    }
    return true;
}
