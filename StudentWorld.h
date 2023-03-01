#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>

class Actor;
class Peach;


class StudentWorld : public GameWorld
{
public:

	//constructor and destructor
	StudentWorld(std::string assetPath);
	virtual ~StudentWorld();

	//main functions for implementing gameplay (we don't directly call these)
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	//helper functions for finding and getting actors of certain types at a specific location
	virtual bool hasActor(double x, double y);
	virtual Actor& getActor(double x, double y);
	virtual bool hasImmovableActor(double x, double y);
	virtual bool hasPlayer(double x, double y);
	virtual Peach& getPlayer();

	//helper functions for adding/removing actors and managing storage appropriately
	virtual void addActor(Actor* a);
	virtual void deleteActor(Actor* a);
	virtual void removeDeadActors();

	//helper functions for determining if movement to a certain location is possible under certain conditions
	virtual bool canMoveTo(double x, double y);
	virtual bool canStablyMoveTo(double x, double y);
	virtual bool canMoveRightTo(double x, double y);
	


private:

	std::vector<Actor*> m_actors; //vector container for living actors

	std::vector<Actor*> m_deadActors; //vector container for dead actors

	Peach* m_peach; //peach pointer

};

#endif // STUDENTWORLD_H_

