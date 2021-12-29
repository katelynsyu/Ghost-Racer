#include "Actor.h"
#include "StudentWorld.h"
#include "GameWorld.h"
#include "StudentWorld.h"
#include <stdlib.h>
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//////////////  Actor Methods ///////////
bool Actor::hasGoneOffScreen(Actor *a)
// if the actor has gone off screen, then set it's alive status to false and return true. else return false
{
    int y = a->getY();
    int x = a->getX();
    if (x < 0 || y < 0 || x > VIEW_WIDTH || y > VIEW_HEIGHT)
    {
        a->setAlive(false);
        return true;
    }
    return false;
}

void Actor::moveAlgorithm(Actor* a)
// move algorithm as states in spec
{
    double vert_speed = a->getVSpeed() - getWorld()->getRacer()->getVSpeed();
    double horiz_speed = a->getHSpeed();
    double new_y = a->getY() + vert_speed;
    double new_x = a->getX() + horiz_speed;
    a->moveTo(new_x, new_y);
}

void Actor::checkHealth(Actor* a)
// if the actor's health is less than 0, set it's alive status to false
{
    if (a->getHealth() <= 0)
        a->setAlive(false);
}

//////////////  Pedestrian Methods ///////////
void Pedestrian::createNewMovementPlan()
{
    //set horizontal speed to a random int from -3 to 3 inclusive, excluding 0
    int a = 0;
    while (a == 0)
    {
        a = randInt(-3, 3);
    }
    changeHSpeed((getHSpeed() * -1) + a);
    
    //set length of the movement plan to a random int between 4 and 32 inclusive
    int b = randInt(4, 32);
    changeMovementPlanDist((getMovementPlanDist() * -1) + b);
    
    //if new horizontal speed < 0, set pedestrian's direction to 180 degrees. if new horizontal speed is > 0, set pedestrian's direction to 0 degrees.
    if (getHSpeed() < 0)
        setDirection(180);
    else
        setDirection(0);
}

//////////////  Human Pedestrian Methods ///////////
void HumanPedestrian::doSomething()
{
    //check if health is <= zero
    checkHealth(this);
    
    //If pedestrian is not alive, then return
    if (!getAlive())
        return;
    
    //If pedestrian overlaps with GhostRacer, set GhostRacer to not alive and return
    if (getWorld()->determineOverlap(this, getWorld()->getRacer()))
    {
        
        getWorld()->getRacer()->setAlive(false);
        return;
    }
    
    //The pedestrian must move
    moveAlgorithm(this);
    
    //If pedestrian has gone off screen, set status to not-alive
    if (hasGoneOffScreen(this))
        return;
    
    //Decrement pedestrian's movement plan distance. If the distance > 0, immediately return.
    changeMovementPlanDist(-1);
    if (getMovementPlanDist() > 0)
        return;
    else
    {
        //create a new movement plan
        createNewMovementPlan();
    }
}

void HumanPedestrian::hitByHolyWater()
// if hit by holy water, change reverse it's horizontal speed, reverse it's direction, and play a sound.
{
    changeHSpeed(-2 * getHSpeed());
    setDirection(-1 * getDirection());
    getWorld()->playSound(SOUND_PED_HURT);
}

//////////////  Zombie Pedestrian Methods ///////////
void ZombiePedestrian::doSomething()
{
    //check if health is <= zero
    checkHealth(this);
    
    //If pedestrian is not alive, return
    if (!getAlive())
        return;
    
    //If pedestrian overlaps with Ghost Racer, Ghost Racer's health decrements by 5 points and pedestrian's health decrements by 2 points. Return
    if (getWorld()->determineOverlap(this, getWorld()->getRacer()))
    {
        getWorld()->getRacer()->changeHealth(-5);
        changeHealth(-2);
        if (getWorld()->checkPlayerDead())
            return;
        if (getHealth() <= 0)
        {
            getWorld()->playSound(SOUND_PED_DIE);
            
            if (randInt(1, 5) == 1)
                getWorld()->createNewHealingGoodie(getX(), getY());
            setAlive(false);
            getWorld()->increaseScore(150);
        }
        else
            getWorld()->playSound(SOUND_PED_HURT);
        return;
    }
    
    //If pedestrian is within 30 pixels of Ghost Racer's x coord and is in front of Ghost Racer, then pedestrian walks towards Ghost Racer, grunting periodically
    int delta_x = getX() - getWorld()->getRacer()->getX();
    if (delta_x <= 30 && delta_x >= -30 && getY() > getWorld()->getRacer()->getY())
    {
        setDirection(270);
        m_timeToNextGrunt--;
        
        if (delta_x < 0)
            changeHSpeed((getHSpeed() * -1) + 1);
        else if (delta_x > 0)
            changeHSpeed((getHSpeed() * -1) - 1);
        else
            changeHSpeed(getHSpeed() * -1);
        
        if (m_timeToNextGrunt <= 0)
        {
            getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
            m_timeToNextGrunt = 20;
        }
    }
    
    //Pedestrian moves
    moveAlgorithm(this);
    
    //If pedestrian has gone off-screen, change status to not alive
    if (hasGoneOffScreen(this))
        return;
    
    //If pedestrian's movement plan distance > 0, decrement it, return.
    if (getMovementPlanDist() > 0)
    {
        changeMovementPlanDist(-1);
        return;
    }
    else
    {
        //create a new movement plan
        createNewMovementPlan();
    }
}

void ZombiePedestrian::hitByHolyWater()
// if hit by holy water, decrease health by one, if it's health is <= 0, set alive status to false, play a sound. if not overlapping with the racer, there is a 1/5 chance that a new healing goodie is created in it's place, increase score by 150
{
    changeHealth(-1);
    if (getHealth() <= 0)
    {
        setAlive(false);
        getWorld()->playSound(SOUND_PED_DIE);
        if (!getWorld()->determineOverlap(this, getWorld()->getRacer()))
        {
            if (randInt(1, 5) == 1)
                getWorld()->createNewHealingGoodie(getX(), getY());
        }
        getWorld()->increaseScore(150);
    }
    else
        getWorld()->playSound(SOUND_PED_HURT);
}

//////////////  Zombie Cab Methods ///////////
void ZombieCab::doSomething()
{
    //check if health is <= zero. if cab not alive, return
    checkHealth(this);
    if (!getAlive())
        return;
    
    //if cab overlaps with ghost racer
    if (getWorld()->determineOverlap(this, getWorld()->getRacer()))
    {
        if (m_hasDamagedRacer)
        {
            //skip to end of if statement
        }
        else
        {
            //otherwise, play a sound, do 20 points of damage to racer, check if racer has died from the damage (if so return)
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
            getWorld()->getRacer()->changeHealth(-20);
            if (getWorld()->checkPlayerDead())
                return;
            
            //if the cab is to the left of the racer or has the same x coordinate as the racer, then set the cab's horizontal speed to -5 and set it's direction to 60 - a random int from 0 - 19 inclusive
            //otherwise, set the cab's horizontal speed to 5 and set it's direction to 120 + a random int from 0 - 19 inclusive
            if (getX() <= getWorld()->getRacer()->getX())
            {
                changeHSpeed(-5 + getHSpeed() * -1);
                setDirection(120 - randInt(0, 19));
            }
            if (getX() > getWorld()->getRacer()->getX())
            {
                changeHSpeed(5 + getHSpeed() * -1);
                setDirection(60 + randInt(0, 19));
            }
                
            //set m_hasDamagedRacer to true
            m_hasDamagedRacer = true;
        }
    }
    
    //if cab has already damaged Ghost Racer, use movement algorithm
    moveAlgorithm(this);
    if (hasGoneOffScreen(this))
        return;
    
    //Get ZombieCab's lane
    int xPos = getX();
    int lane = -1;
    
    if (xPos >= LEFT_EDGE && xPos < LEFT_WHITE_DIVIDER)
        lane = LEFT_LANE;
    if (xPos >= LEFT_WHITE_DIVIDER && xPos < RIGHT_WHITE_DIVIDER)
        lane = MIDDLE_LANE;
    if (xPos >= RIGHT_WHITE_DIVIDER && xPos < RIGHT_EDGE)
        lane = RIGHT_LANE;
    
    //If cab's vertical speed > Racer's vertical speed And there is an actor in front of the cab, decrease speed by .5 and return
    double cabSpeed = getVSpeed();
    double racerSpeed = getWorld()->getRacer()->getVSpeed();
    if (cabSpeed > racerSpeed && getWorld()->determineActorNearby(this, lane) == TOP)
    {
        changeVSpeed(-0.5);
        return;
    }
    else if (cabSpeed <= racerSpeed && getWorld()->determineActorNearby(this, lane) == BOTTOM)
    {
        changeVSpeed(0.5);
        return;
    }
    
    //Decrement zombie cab's movement plan distance by one
    m_movementPlanDist--;
    
    //If cab's movement plan distance is greater than zero, then return
    if (m_movementPlanDist > 0)
        return;
    else //otherwise set the length of the cab's movement plan to a random integer between 4 and 32 inclusive and change the cab's speed to its current speed plus a random integer between -2 and 2 inclusive
    {
        m_movementPlanDist = randInt(4, 32);
        changeVSpeed(randInt(-2, 2));
    }
}

void ZombieCab::hitByHolyWater()
// if hit by holy water, decrement health by one, set alive status to not alive, play a sound, if there is no overlap between the cab and racer, there is a 1/5 chance a new healing goodie will be created in its place. increase score by 200
{
    changeHealth(-1);
    if (getHealth() <= 0)
    {
        setAlive(false);
        getWorld()->playSound(SOUND_PED_DIE);
        if (!getWorld()->determineOverlap(this, getWorld()->getRacer()))
        {
            if (randInt(1, 5) == 1)
                getWorld()->createNewHealingGoodie(getX(), getY());
        }
        getWorld()->increaseScore(200);
    }
    else
        getWorld()->playSound(SOUND_VEHICLE_HURT);
}

//////////////  Goodie Methods ///////////
void Goodie::gotGoodie(Actor* a, int scoreIncrease)
{
    //if goodie overlaps with racer, racer heals itself by 10 points, sets status to not alive, plays a sound, increases player's score by scoreIncrease points
    int health = getWorld()->getRacer()->getHealth();
    //ensure that health only increases up to 100
    if (health + 10 > 100)
        getWorld()->getRacer()->changeHealth((-1 * health) + 100);
    else
        getWorld()->getRacer()->changeHealth(10);
    a->setAlive(false);
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->increaseScore(scoreIncrease);
}

//////////////  Healing Goodie Methods ///////////
void HealingGoodie::doSomething()
{
    //check if health is <= zero
    checkHealth(this);
    if (!getAlive())
        return;
    
    //move
    moveAlgorithm(this);
    
    //if off-screen, set to not alive and return
    if (hasGoneOffScreen(this))
        return;
    
    //if goodie overlaps with racer, racer heals itself by 10 points, sets status to not alive, plays a sound, increases player's score by 250 points
    if (getWorld()->determineOverlap(this, getWorld()->getRacer()))
    {
        gotGoodie(this, 250);
    }
}

void HealingGoodie::hitByHolyWater()
//if hit by holy water, disappear
{
    setAlive(false); 
}


//////////////  HolyWater Refill Methods ///////////
void HolyWaterRefill::doSomething()
{
    //check if health is z<= ero
    checkHealth(this);
    
    //move
    moveAlgorithm(this);
    
    //if off-screen, set to not alive and return
    if (hasGoneOffScreen(this))
        return;
    
    //if goodie overlaps with racer, racer heals itself by 10 points, sets status to not alive, plays a sound, increases player's score by 50 points
    if (getWorld()->determineOverlap(this, getWorld()->getRacer()))
    {
        gotGoodie(this, 50);
        getWorld()->getRacer()->changeHolyWaterUnits(1);
    }
}

void HolyWaterRefill::hitByHolyWater()
//if hit by holy water, disappear
{
    setAlive(false);
}

//////////////  Lost Souls Methods ///////////
void LostSouls::doSomething()
{
    //move
    moveAlgorithm(this);
    
    //if off-screen, set to not alive and return
    if (hasGoneOffScreen(this))
        return;
    
    //if lost soul overlaps with racer, increment the number of saved souls, set the object to not alive, play a sound, increase the score by 100 points
    if (getWorld()->determineOverlap(this, getWorld()->getRacer()))
    {
        getWorld()->getRacer()->incrementSavedSouls();
        setAlive(false);
        getWorld()->playSound(SOUND_GOT_SOUL);
        getWorld()->increaseScore(100);
    }
    
    setDirection(getDirection()-10);
}

//////////////  Holy Water Projectile Methods ///////////
void HolyWaterProjectile::doSomething()
{
    //if not alive, return
    if (!getAlive())
        return;
    
    //see if projectile is activated. ie if it overlaps an object affected by the projectile if so, then damage the other object with 1 hit point of damage, set its status to not alive
    getWorld()->isHolyWaterProjectileActive(this);
    
    if (m_isActive)
        return;
    
    //projectile will move forward in its current dirrection by SPRITE_HEIGHT pixels. Add SPRITE_HEIGHT to the distance travelled for the spray
    moveForward(SPRITE_HEIGHT);
    m_travelDist += SPRITE_HEIGHT;
    
    //if projectile has gone off-screen, set ot not alive and return
    if (hasGoneOffScreen(this))
        return;
    
    //if projectile has travlled maxTravelDist (160 pixels), set to not_alive
    if (m_travelDist >= m_maxTravelDist)
        setAlive(false);
    return;
}

//////////////  Oil Slick Methods ///////////
void OilSlick::doSomething()
{
    //move
    moveAlgorithm(this);
    
    //set to not alive if goes off-screen
    if (hasGoneOffScreen(this))
        return;
    
    //if overlaps with racer, play sound, make racer spin
    if (getWorld()->determineOverlap(this, getWorld()->getRacer()))
    {
        getWorld()->playSound(SOUND_OIL_SLICK);
        getWorld()->getRacer()->spin();
    }
}


//////////////  Border Line Methods ///////////
void BorderLines::doSomething()
{
    //move
    moveAlgorithm(this);
    
    //set to not alive if the line goes off-screen
    if (hasGoneOffScreen(this))
    {
        return;
    }
}

//////////////  Ghost Racer Methods ///////////
void Racer::doSomething()
{
    //if racer is not alive, return
    if (getWorld()->checkPlayerDead())
        return;

    //Check if player has pressed a key
    int keyPressed;
    if (getWorld()->getKey(keyPressed))
    {
        switch(keyPressed)
        {
            case KEY_PRESS_SPACE:
                //add a new holy water sprey object, play a sound, decrease spray count by 1
                if (m_holyWaterUnits >= 1)
                {
                    double direction = getDirection();
                    double delta_y = SPRITE_HEIGHT * sin(direction * PI/180);
                    double delta_x = SPRITE_HEIGHT * cos(direction * PI/180);
                    getWorld()->createNewHolyWaterProjectile(delta_x + getX(), delta_y + getY(), direction);
                    getWorld()->playSound(SOUND_PLAYER_SPRAY);
                    m_holyWaterUnits--;
                }
                break;
            case KEY_PRESS_LEFT:
                //check if the angle is < 114 degrees in which case racer can add 8 degrees to its current angle
                if (getDirection() < 114)
                    setDirection(getDirection()+8);
                break;
            case KEY_PRESS_RIGHT:
                //check if the angle is > 66 degrees in which case racer can subtract 8 degrees to its current angle
                if (getDirection() > 66)
                    setDirection(getDirection()-8);
                break;
            case KEY_PRESS_UP:
                //If racer's current speed < 5, increment vertical speed
                if (getVSpeed() < 5)
                    changeVSpeed(1);
                break;
            case KEY_PRESS_DOWN:
                //If racer's current speed > -1, decrement vertical speed
                if (getVSpeed() > -1)
                    changeVSpeed(-1);
                break;
        }
    }
    
    // check if racer is on the road
    //if the racer's x coordinate <= left road boundary and its direction is > 90, get 10 hit point damage, change direction to 82 degrees, play a sound
    if (getX() <= m_leftBoundary)
    {
        if (getDirection() > 90)
        {
            changeHealth(-10);
            setDirection(82);
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
        }
    }
    //if the racer's x coordinate >= right road boundary and its direction is < 90, get 10 hit point damage, change direction to 98 degrees, play a sound
    if (getX() >= m_rightBoundary)
    {
        if (getDirection() < 90)
        {
            changeHealth(-10);
            setDirection(98);
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
        }
    }
    
    //movement algorithm
    double direction = getDirection() * PI/180;
    double delta_x = cos(direction) * MAX_SHIFT_PER_TICK;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}

void Racer::spin()
{
    //if an oil slick tries to spin racer, adjust direction by a random integer from 5 to 20 inclusive clockwise or counterclockwise from its current direction. Angle must never go below 60 degrees or above 120 degrees
    int cur_direction = getDirection();
    int amt_turn = randInt(5, 20);
    int spinDirection = randInt(1, 10);
    if (spinDirection <= 5)
        spinDirection = -1;
    else
        spinDirection = 1;
    
    int new_direction = cur_direction + (spinDirection * amt_turn);
    
    if (new_direction > 120 || new_direction < 60)
        new_direction = cur_direction - (spinDirection * amt_turn);

    setDirection(new_direction);
}
