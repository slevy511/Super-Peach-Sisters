#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include <cmath>

/*Actor class which inherits from GraphObject and is the basis for all actors*/
class Actor : public GraphObject
{
public:

	/*Actor constructor*/
	Actor(StudentWorld* sw, int imageID, int x, int y, int dir = 0, int depth = 0, double size = 1.0)
		:GraphObject(imageID, x, y), m_world(sw), isLiving(true)
	{}
	
	/*pure virtual doSomething class to make sure you can't create an object just of type Actor*/
	virtual void doSomething() = 0;

	/*empty bonk and damage implementations by default, but will often be overridden*/
	virtual void bonk() {}
	virtual void damage() {}
	
	/*getter function for alive/dead status of each actor*/
	virtual bool isAlive(){ return isLiving; }

	/*function for killing non-peach actors*/
	virtual void kill();

	/*bools to check certain properties that often apply to multiple types of actors*/
	virtual bool canShare() { return true; }
	virtual bool isDamageable(){ return true; }
	virtual bool hurtsPlayer(){ return false; }
	virtual bool takesUserInput(){ return false; }
	virtual bool endsLevel(){ return false; }
	virtual bool endsGame(){ return false; }

	/*getter for StudentWorld pointer*/
	virtual StudentWorld& getWorld(){ return *m_world; }

	/*helper function for objects that move left/right unless something is in their way*/
	virtual void moveAppropriately(double x, double y, int dir);

private:

	StudentWorld* m_world; //StudentWorld pointer
	
	bool isLiving; //tracks living status


	/*helper functions for moveAppropriately function - specify what to do if it CAN (1st function) or 
	CANNOT (2nd function) move in the current direction the distance that the actor wants to move*/
	virtual void doSpecificDirectionalBehavior(int dir) {}

	virtual void doSpecificAlternateBehavior(int dir) {
		if (dir == 0) { setDirection(180); }
		else { setDirection(0); }
	}
	
};


/*class for Peach, the player that the user controls. Inherits directly from Actor*/
class Peach : public Actor
{
public:
	//Peach Constructor
	Peach(StudentWorld* sw, int imageID, int x, int y)
		: Actor(sw, imageID, x, y), invinc(0), tempInvinc(0), recharge(0),
		reachedTheFlag(0), remainingJumpDist(0), jumpPower(0), shootPower(0), hitPoints(1) 
	{}

	/*functions to check various player properties*/
	virtual bool isAlive(){ return hitPoints > 0; }
	bool isInvincible(){ return invinc > 0; }
	bool hasShootPower(){ return shootPower > 0; }
	bool hasJumpPower(){ return jumpPower > 0; }
	bool isTempInvincible(){ return tempInvinc > 0; }
	bool isRecharging(){ return recharge > 0; }
	int reachedFlag(){ return reachedTheFlag; } //0 for not reached, 1 for flag, 2 for mario
	virtual bool takesUserInput() { return true; }

	/*functions to implement certain player actions*/
	void encounterFlag(bool isMario) { if (isMario) { reachedTheFlag = 2; } else { reachedTheFlag = 1; } }
	void gainShootPower() { shootPower = 1; hitPoints = 2; }
	void gainJumpPower() { jumpPower = 1; hitPoints = 2; }
	void gainStarPower() { invinc = 150; }
	
	virtual void doSomething();

	virtual void bonk();
	virtual void damage() { bonk(); } //damaging does the same thing as bonking for peach

private:
	
	/*member variables to track certain player properties*/
	int invinc;
	int tempInvinc;
	int recharge;
	int shootPower;
	int jumpPower;
	int hitPoints;
	int reachedTheFlag;
	int remainingJumpDist;
	
	/*helper function for when peach can move in the direction she wants to move in*/
	virtual void doSpecificDirectionalBehavior(int dir) {
		if (dir == 0) { moveTo(getX() + 4, getY()); }
		else { moveTo(getX() - 4, getY()); }
	}

	/*helper function for when peach cannot move in the direction she wants to move in*/
	virtual void doSpecificAlternateBehavior(int dir) {
		if (dir == 0) { getWorld().getActor(getX() + 4, getY()).bonk(); }
		else { getWorld().getActor(getX() - 4, getY()).bonk(); }
	}

};

/*base class for blocks/pipes, flags, and mario*/
class StaticObject : public Actor
{
public: 

	StaticObject(StudentWorld* sw, int imageID, int x, int y, int depth = 2)
		: Actor(sw, imageID, x, y)
	{}

	//does nothing each tick
	virtual void doSomething(){}

	//overrides base class because it can't be damaged
	virtual bool isDamageable(){ return false; }

private:
	
};

/*base class for goodies and projectiles*/
class InanimateObject : public Actor
{
public:

	InanimateObject(StudentWorld* sw, int imageID, int x, int y, int depth = 1)
		: Actor(sw, imageID, x, y)
	{}

	//overrides base class because it can't be damaged
	virtual bool isDamageable(){ return false; }

private:

};

/*base class for fireballs (peach and piranha) and shells*/
class Projectile : public InanimateObject
{
public:
	Projectile(StudentWorld* sw, int imageID, int x, int y)
		: InanimateObject(sw, imageID, x, y)
	{}

	/*this implementation will be overriden in subclasses to actually try and damage certain
	actors and return true/false if it did/didn't do so successfully */
	virtual bool damageCertainActor() { return false; }

	virtual void doSomething();

private:

	/*helper function for when projectile can move in the direction she wants to move in*/
	virtual void doSpecificDirectionalBehavior(int dir){
		if (dir == 0) { moveTo(getX() + 2, getY()); }
		else { moveTo(getX() - 2, getY()); }
	}

	/*helper function for when projectile cannot move in the direction it wants to move in*/
	virtual void doSpecificAlternateBehavior(int dir){ kill(); }

};

class PiranhaFireball : public Projectile
{
public:
		PiranhaFireball(StudentWorld* sw, int imageID, int x, int y, int dir)
			: Projectile(sw, imageID, x, y)
		{ setDirection(dir); } //sets direction to the direction piranha is pointing in

		/*attemps to damage peach, returns true/false if it did/didn't*/
		virtual bool damageCertainActor(){ 
			if (getWorld().hasPlayer(getX(), getY())) { 
				getWorld().getPlayer().damage();
				return true; } return false;
		}
};


class Shell : public Projectile
{
public:
	Shell(StudentWorld* sw, int imageID, int x, int y, int dir)
		: Projectile(sw, imageID, x, y)
	{ setDirection(dir); } //sets direction to the direction creator is pointing in

	/*attemps to damage an enemy, returns true/false if it did/didn't*/
	virtual bool damageCertainActor(){
		if (getWorld().hasActor(getX(), getY()) && getWorld().getActor(getX(), getY()).hurtsPlayer()) {
			getWorld().getActor(getX(), getY()).damage();
			return true; } return false;
	}
};


class Goodie : public InanimateObject
{
public:

	Goodie(StudentWorld* sw, int imageID, int x, int y)
		: InanimateObject(sw, imageID, x, y)
	{}

	virtual void doSomething();


private:

	//pure virtual function to be overridden in subclasses
	virtual void gainPower() = 0;

	/*overriden implementation for what a goodie does when it can move in the direction it wants to move in*/
	virtual void doSpecificDirectionalBehavior(int dir) {
		if (dir == 0) { moveTo(getX() + 2, getY()); }
		else { moveTo(getX() - 2, getY()); }
	}

};

class Flower : public Goodie
{
public:

	Flower(StudentWorld* sw, int imageID, int x, int y)
		: Goodie(sw, imageID, x, y)
	{}

	
private:

	/*gives player the specified power and increases score accordingly*/
	virtual void gainPower() { getWorld().getPlayer().gainShootPower(); 
								getWorld().increaseScore(50); }

};

class Mushroom : public Goodie
{
public:

	Mushroom(StudentWorld* sw, int imageID, int x, int y)
		: Goodie(sw, imageID, x, y)
	{}

private:

	/*gives player the specified power and increases score accordingly*/
	virtual void gainPower() { getWorld().getPlayer().gainJumpPower(); 
								getWorld().increaseScore(75); }


};

class Star : public Goodie
{
public:

	Star(StudentWorld* sw, int imageID, int x, int y)
		: Goodie(sw, imageID, x, y)
	{}

private:

	/*gives player the specified power and increases score accordingly*/
	virtual void gainPower() { getWorld().getPlayer().gainStarPower(); 
								getWorld().increaseScore(100); }

};


class Block : public StaticObject
{
public:
	Block(StudentWorld* sw, int imageID, int x, int y)
		:StaticObject(sw, imageID, x, y), holdsGoodie(false)
	{}

	//overriden bool property of actor function
	virtual bool canShare(){ return false; }

	virtual void bonk();

	//returns whether or not the block holds a goodie
	bool hasGoodie() { return holdsGoodie; }

	/*functions for manipulating block's goodie-holding status*/
	void setContainsGoodieStatus(){ holdsGoodie = true; }
	void setRemovedGoodieStatus(){ holdsGoodie = false; }
	
private:
	bool holdsGoodie;

	virtual void releaseGoodie() {}
};

class FlowerBlock : public Block
{
public:

	FlowerBlock(StudentWorld* sw, int imageID, int x, int y)
		:Block(sw, imageID, x, y) 
	{ setContainsGoodieStatus(); } //block starts out holding a goodie


private:

	/*helper function for releasing a goodie of the specified type and adding it to actors vector*/
	virtual void releaseGoodie() { Actor* temp = new Flower(&getWorld(), IID_FLOWER, 
		getX(), getY() + SPRITE_HEIGHT); getWorld().addActor(temp); setRemovedGoodieStatus(); }


};

class MushroomBlock : public Block
{
public:
	MushroomBlock(StudentWorld* sw, int imageID, int x, int y)
		:Block(sw, imageID, x, y)
	{ setContainsGoodieStatus(); } //block starts out holding a goodie

private:

	/*helper function for releasing a goodie of the specified type and adding it to actors vector*/
	virtual void releaseGoodie() { Actor* temp = new Mushroom(&getWorld(), IID_MUSHROOM, 
		getX(), getY() + SPRITE_HEIGHT); getWorld().addActor(temp); }

};

class StarBlock : public Block
{
public:
	StarBlock(StudentWorld* sw, int imageID, int x, int y)
		:Block(sw, imageID, x, y)
	{ setContainsGoodieStatus(); } //block starts out holding a goodie

private:

	/*helper function for releasing a goodie of the specified type and adding it to actors vector*/
	virtual void releaseGoodie() { Actor* temp = new Star(&getWorld(), IID_STAR, 
		getX(), getY() + SPRITE_HEIGHT); getWorld().addActor(temp); }

};


class Flag : public StaticObject
{
public:
	Flag(StudentWorld* sw, int imageID, int x, int y, int depth = 1)
		:StaticObject(sw, imageID, x, y), levelComplete(false)
	{}

	//overriden bool property of actor function
	virtual bool endsLevel(){ return true; }

private:
	bool levelComplete;

};

class Mario : public Flag
{
public:
	Mario(StudentWorld* sw, int imageID, int x, int y)
		:Flag(sw, imageID, x, y)
	{}

	//overriden bool property of actor function
	virtual bool endsGame() { return true; }

private:

};

class Enemy : public Actor
{
public:
	Enemy(StudentWorld* sw, int imageID, int x, int y, int dir = (randInt(0, 1)) * 180)
		:Actor(sw, imageID, x, y, dir = (randInt(0, 1)) * 180)
	{}

	virtual void bonk();

	/*implements enemy's getting damaged behavior (different from bonk)*/
	virtual void damage() { getWorld().increaseScore(100); kill(); doSpecificBehavior(); }

	//overriden bool property of actor function
	virtual bool hurtsPlayer(){ return true; }

private:

	/*virtual helper function for doing added behaviors when specific types of enemies are damaged*/
	virtual void doSpecificBehavior() {}

};


class MovingEnemy : public Enemy
{
public:
	MovingEnemy(StudentWorld* sw, int imageID, int x, int y)
		:Enemy(sw, imageID, x, y)
	{}

	virtual void doSomething();

private:

	/*overriden implementation for what a MovingEnemy does when it can move in the direction it wants to move in*/
	virtual void doSpecificDirectionalBehavior(int dir);

};


class Koopa : public MovingEnemy
{
public:
	Koopa(StudentWorld* sw, int imageID, int x, int y)
		:MovingEnemy(sw, imageID, x, y)
	{}

private:
	
	/*virtual helper function for doing added behaviors when specific types of enemies are damaged*/
	virtual void doSpecificBehavior(){ Shell* temp = new Shell(&getWorld(), IID_SHELL, getX(), getY(), getDirection()); 
										getWorld().addActor(temp); }
};

class Piranha : public Enemy
{
public:
	Piranha(StudentWorld* sw, int imageID, int x, int y)
		:Enemy(sw, imageID, x, y), firingDelay(0)
	{}

	virtual void doSomething();

private:
	int firingDelay; //keeps track of 40 tick delays between fireballs shots
};

#endif // ACTOR_H_




