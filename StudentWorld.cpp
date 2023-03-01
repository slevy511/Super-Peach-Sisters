#include "StudentWorld.h"
#include "Actor.h" //make sure this is necesary
#include "GameConstants.h"
#include <string>
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw
using namespace std;


GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

//constructor
StudentWorld::StudentWorld(string assetPath)
    : GameWorld(assetPath)
{}

//destructor
StudentWorld::~StudentWorld()
{
    cleanUp();
}

/*checks if there is an actor (that isn't peach at the specified location*/
bool StudentWorld::hasActor(double x, double y)
{
    for (size_t i = 0; i < m_actors.size(); i++)
    {
        if ((m_actors[i]->getX() - SPRITE_WIDTH+1 <= x && m_actors[i]->getX() + SPRITE_WIDTH-1 >= x)
            && (m_actors[i]->getY() - SPRITE_HEIGHT +1 <= y && m_actors[i]->getY() + SPRITE_HEIGHT -1 >= y)) {
        
            return true;
        }
    }
    return false;
}

/*returns the actor at the specified location if there is one (returns peach otherwise)*/
Actor& StudentWorld::getActor(double x, double y)
{
        for (size_t i = 0; i < m_actors.size(); i++)
        {
            if ((m_actors[i]->getX() - SPRITE_WIDTH +1 <= x && m_actors[i]->getX() + SPRITE_WIDTH -1>= x)
                && (m_actors[i]->getY() - SPRITE_HEIGHT +1 <= y && m_actors[i]->getY() + SPRITE_HEIGHT -1 >= y)) {

                return *m_actors[i];
            }
        }
    
    return *m_peach;
}

/*checks if there is a block/pipe at the specified location*/
bool StudentWorld::hasImmovableActor(double x, double y)
{
    if (!hasActor(x, y)) {
        return false;
    }

    for (size_t i = 0; i < m_actors.size(); i++)
    {
        if ((m_actors[i]->getX() - SPRITE_WIDTH + 1 <= x && m_actors[i]->getX() + SPRITE_WIDTH - 1 >= x)
            && (m_actors[i]->getY() - SPRITE_HEIGHT + 1 <= y && m_actors[i]->getY() + SPRITE_HEIGHT - 1 >= y)) {
            
            if (!m_actors[i]->canShare()) {
                return true;
            }
        }
    }
    
    return false;
}

/*checks if peach overlaps with the specified location */
bool StudentWorld::hasPlayer(double x, double y)
{
    if ((m_peach->getX() - SPRITE_WIDTH + 1 <= x &&
        m_peach->getX() + SPRITE_WIDTH - 1 >= x)
        && (m_peach->getY() - SPRITE_HEIGHT + 1 <= y &&
            m_peach->getY() + SPRITE_HEIGHT - 1 >= y)) {
        return true;
    }
    return false;
}

/*peach pointer getter*/
Peach& StudentWorld::getPlayer()
{
    return *m_peach;
}

/*adds the given actor to the actor container vector*/
void StudentWorld::addActor(Actor* a)
{
    Actor* temp = a;
    m_actors.push_back(temp);
}

/*removes the given actor from the actor container vector and puts it in the dead actor container vector*/
void StudentWorld::deleteActor(Actor* a)
{
    vector<Actor*>::iterator it = m_actors.begin();
    for (; it != m_actors.end(); ) {
        if (*it == a) {
            m_deadActors.push_back(*it);
            it = m_actors.erase(it);
            break;
        }
        else {
            it++;
        }
    }
}

/*clears the dead actors container vector*/
void StudentWorld::removeDeadActors()
{
    int size2 = m_deadActors.size();
    for (int i = 0; i < size2; i++)
    {
        delete m_deadActors[i];
    }
    m_deadActors.clear();
}

/*checks if the actor can move to the specific location*/
bool StudentWorld::canMoveTo(double x, double y)
{
    if (hasActor(x, y) && !getActor(x, y).canShare()) {
        return false;
    }
    return true;
}

/*checks if the actor can move to the specific location and have a block/pipe underneath it*/
bool StudentWorld::canStablyMoveTo(double x, double y)
{
    if (hasActor(x, y - SPRITE_HEIGHT) && !getActor(x, y - SPRITE_HEIGHT).canShare())
    {
        return true;
    }
    return false;
}

/*checks if the actor can move to the specific location if moving to the right*/
bool StudentWorld::canMoveRightTo(double x, double y)
{
    for (size_t i = 0; i < m_actors.size(); i++)
    {
        if ((m_actors[i]->getX() <= x+SPRITE_WIDTH-1 && m_actors[i]->getX() + SPRITE_WIDTH - 1 >= x+ SPRITE_WIDTH-1)
            && (m_actors[i]->getY() - SPRITE_HEIGHT + 1 <= y && m_actors[i]->getY() + SPRITE_HEIGHT - 1 >= y)) {
            
            if (!m_actors[i]->canShare()) {
                return false;
            } 
        }
    }
    return true;
}

/*takes in the levle file and creates and stores the appropriate types of actors in the given locations*/
int StudentWorld::init() 
{
    Actor* temp;

    ostringstream oss;
    int k = getLevel();

    if (k < 10) {
        oss << "level0" << k << ".txt";
    }
    else {
        oss << "level" << k << ".txt";
    }

    Level lev(assetPath());

    string levelFile = oss.str();

    cerr << "level file: " << levelFile << endl;

    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found) {
        cerr << "Could not find level01.txt data file" << endl;
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_fail_bad_format) {
        cerr << "level01.txt is improperly formatted" << endl;
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;

        int tempRow = 0;
        int tempCol = 0;

        for (tempRow = 0; tempRow < GRID_HEIGHT; tempRow++)
        {
            for (tempCol = 0; tempCol < GRID_WIDTH; tempCol++)
            {
                Level::GridEntry ge;

                ge = lev.getContentsOf(tempRow, tempCol);

                switch (ge)
                { 
                case Level::peach:
                    m_peach = new Peach(this, IID_PEACH, tempRow * SPRITE_WIDTH, tempCol * SPRITE_HEIGHT);
                    break;
                case Level::flag:
                    temp = new Flag(this, IID_FLAG, tempRow * SPRITE_WIDTH, tempCol * SPRITE_HEIGHT); 
                    m_actors.push_back(temp);
                    break;
                case Level::mario:
                    temp = new Mario(this, IID_MARIO, tempRow * SPRITE_WIDTH, tempCol * SPRITE_HEIGHT);
                    m_actors.push_back(temp);
                    break;
                case Level::block:
                    temp = new Block(this, IID_BLOCK, tempRow * SPRITE_WIDTH, tempCol * SPRITE_HEIGHT); 
                    m_actors.push_back(temp);
                    break;
                case Level::flower_goodie_block:
                    temp = new FlowerBlock(this, IID_BLOCK, tempRow * SPRITE_WIDTH, tempCol * SPRITE_HEIGHT); 
                    m_actors.push_back(temp);
                    break;
                case Level::mushroom_goodie_block:
                    temp = new MushroomBlock(this, IID_BLOCK, tempRow * SPRITE_WIDTH, tempCol * SPRITE_HEIGHT); 
                    m_actors.push_back(temp);
                    break;
                case Level::star_goodie_block:
                    temp = new StarBlock(this,IID_BLOCK, tempRow*SPRITE_WIDTH,tempCol*SPRITE_HEIGHT); 
                    m_actors.push_back(temp);
                    break;
                case Level::pipe:
                    temp = new Block(this, IID_PIPE, tempRow * SPRITE_WIDTH, tempCol * SPRITE_HEIGHT); 
                    m_actors.push_back(temp);
                    break;
                case Level::goomba:
                    temp = new MovingEnemy(this, IID_GOOMBA, tempRow * SPRITE_WIDTH, tempCol * SPRITE_HEIGHT); 
                    m_actors.push_back(temp);
                    break;
                case Level::koopa:
                    temp = new Koopa(this, IID_KOOPA, tempRow * SPRITE_WIDTH, tempCol * SPRITE_HEIGHT); 
                    m_actors.push_back(temp);
                    break;
                case Level::piranha:
                    temp = new Piranha(this, IID_PIRANHA, tempRow * SPRITE_WIDTH, tempCol * SPRITE_HEIGHT); 
                    m_actors.push_back(temp);
                    break;
                default:
                    break;
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

/*give all the actors a chance to do something and then check deal with deaths appropriately as well
as dealing with if the player finished the level. Also, update the stats on screen*/
int StudentWorld::move()
{
        m_peach->doSomething();

    for (size_t i = 0; i < m_actors.size(); i++)
    {
        if(m_actors[i]->isAlive())
            m_actors[i]->doSomething();
    }

    if (!m_peach->isAlive()) {
        decLives();
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
    }

    if (m_peach->reachedFlag() == 1) {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    if (m_peach->reachedFlag() == 2) {
        playSound(SOUND_GAME_OVER);
        return GWSTATUS_PLAYER_WON;
    }

    removeDeadActors();

    /*update display text*/

    ostringstream oss;
    oss << "Lives: ";
    oss << getLives() << setw(2);
    oss << "  Level: ";
    oss << getLevel() << setw(2);
    oss << "  Score: ";
    oss << getScore() << " ";
    if(m_peach->isInvincible()) {
        oss << "StarPower! ";
    }
    if (m_peach->hasShootPower()) {
        oss << "ShootPower! ";
    }
    if (m_peach->hasJumpPower()) {
        oss << "JumpPower!";
    }
    string s = oss.str();

    setGameStatText(s);

    return GWSTATUS_CONTINUE_GAME;
}

/*delete dynamically allocate items*/
void StudentWorld::cleanUp()
{
    delete m_peach;
    vector<Actor*>::iterator it;
   
    int size = m_actors.size();
    for (int i = 0; i < size; i++)
    {
        delete m_actors[i];
    }

    m_actors.clear();

    removeDeadActors();

}

