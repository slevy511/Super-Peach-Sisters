#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h" 


/*function that is called when a non-peach actor is supposed to die*/
void Actor:: kill() 
{
	isLiving = false;
	if (!takesUserInput())
		getWorld().deleteActor(this);
}

/*helper function for typical left/movement*/
void Actor::moveAppropriately(double x, double y, int dir)
{
	if (dir == 0) {

		if (getWorld().canMoveRightTo(getX() + x, getY() + y))
		{
			doSpecificDirectionalBehavior(0);
		}
		else {
			doSpecificAlternateBehavior(0);
		}
	}
	else {
		if (getWorld().canMoveTo(getX() - x, getY() + y)) {
			doSpecificDirectionalBehavior(180);
		}
		else {
			doSpecificAlternateBehavior(180);
		}
	}
}


void Peach::doSomething()
{
	//check if she is alive first
	if (!isAlive())
		return;

	//decrement appropriate properties if necessary

	if (isInvincible()){
		invinc--;
	}

	if (isTempInvincible()) {
		tempInvinc--;
	}

	if (isRecharging()) {
		recharge--;
	}

	//bonk overlapping actors

	if (getWorld().hasActor(getX(), getY())) {

		getWorld().getActor(getX(), getY()).bonk();

		//check if the overlapping actor is a flag or mario

		if (getWorld().getActor(getX(), getY()).endsLevel()) {

			if (getWorld().getActor(getX(), getY()).endsGame()) {
				encounterFlag(true);
			}
			else {
				encounterFlag(false);
			}

			getWorld().increaseScore(1000);
			getWorld().getActor(getX(), getY()).kill();

		}
	}

	//implement jumping

	if (remainingJumpDist > 0) {

		double tempY = getY() + 4;

		if (getWorld().hasImmovableActor(getX(), tempY)) {
				getWorld().getActor(getX(), tempY).bonk();
				remainingJumpDist = 0;
		}
		else {
			moveTo(getX(), tempY);
			remainingJumpDist--;
		}

		//implement falling

	}else if(!getWorld().hasImmovableActor(getX(), getY() - 1)
		&& !getWorld().hasImmovableActor(getX(), getY() - 2)
		&& !getWorld().hasImmovableActor(getX(), getY() - 3)
		&& !getWorld().hasImmovableActor(getX(), getY())){

		
		moveTo(getX(), getY() - 4);

	}

	//implement user input-based actions (pressing left, right, up, and space)

	int key;
	if (getWorld().getKey(key))
	{
		
		switch (key)
		{
		case KEY_PRESS_LEFT:

			setDirection(180);
			moveAppropriately(4, 0, 180);
			
			break;

		case KEY_PRESS_RIGHT:

			setDirection(0);
			
			moveAppropriately(4, 0, 0);

			break;

		case KEY_PRESS_UP:

			if (getWorld().hasImmovableActor(getX(), getY() - 1)) { 

					if (jumpPower == 0) {
						remainingJumpDist = 8;
					}
					else {
						remainingJumpDist = 12;
					}

					getWorld().playSound(SOUND_PLAYER_JUMP);

			}

			break;

		case KEY_PRESS_SPACE:

			if (shootPower > 0){

				if (!isRecharging()) {

					getWorld().playSound(SOUND_PLAYER_FIRE);

					recharge = 8;

					if (getDirection() == 0) {

						Shell* temp = new Shell(&getWorld(), IID_PEACH_FIRE, getX() + 4, getY(),0);
						getWorld().addActor(temp);

					}
					else {
						
						Shell* temp = new Shell(&getWorld(), IID_PEACH_FIRE, getX() - 4, getY(),180);
						getWorld().addActor(temp);
					}

				}
			}

			break;
		default:
			break;
		}
	}

}

/*function for when Peach gets bonked*/
void Peach::bonk()
{

	if (isInvincible() || isTempInvincible()) {
		return;
	}
	else {
		hitPoints--;
		tempInvinc = 10;
		shootPower = 0;
		jumpPower = 0;
		if (hitPoints >= 1) {
			getWorld().playSound(SOUND_PLAYER_HURT);
		}
		
	}

}

/*overriden doSomething implementation for projectiles*/
void Projectile::doSomething()
{
	if (damageCertainActor()) {
		kill();
		return;
	}
	else if (!getWorld().hasImmovableActor(getX(), getY() - 2)) {
		moveTo(getX(), getY() - 2);
	}

	if (getDirection() == 0) {
		moveAppropriately(2, 0, 0);
	}
	else {
		moveAppropriately(2, 0, 180);
	}
}

/*overriden doSomething implementation for goodies*/
void Goodie::doSomething()
{
	if (getWorld().hasPlayer(getX(), getY())) {
		gainPower();
		kill();
		getWorld().playSound(SOUND_PLAYER_POWERUP);
		return;
	}
	else if (!getWorld().hasImmovableActor(getX(), getY() - 2)) {
		moveTo(getX(), getY() - 2);
	}

	if (getDirection() == 0) {
		moveAppropriately(2, 0, 0);
	}
	else {
		moveAppropriately(2, 0, 180);
	}
}

/*overriden bonk implementation for blocks*/
void Block::bonk()
{
	if (!hasGoodie()) {
		getWorld().playSound(SOUND_PLAYER_BONK);
		return;
	}
	else {
		getWorld().playSound(SOUND_POWERUP_APPEARS);
		releaseGoodie();
		holdsGoodie = false;
	}
}

/*overriden bonk implementation for enemies*/
void Enemy::bonk() 
{
	if (!getWorld().hasPlayer(getX(), getY())) {
		return;
	}

	if (getWorld().getPlayer().isInvincible()) {
		getWorld().playSound(SOUND_PLAYER_KICK);
		damage();
	}
}

/*overriden doSomething implementation for movingenemies*/
void MovingEnemy::doSomething()
{
	if (getWorld().hasPlayer(getX(), getY())) {
		getWorld().getPlayer().bonk();
		return;
	}

	if (getDirection() == 0) {
		moveAppropriately(1, 0, 0);
	}
	else {
		moveAppropriately(1, 0, 180);
	}
}

/*overriden implementation for what a MovingEnemy does when it can move in the direction it wants to move in*/
void MovingEnemy::doSpecificDirectionalBehavior(int dir)
{
	if (dir == 0) {
		if (getWorld().canStablyMoveTo(getX() + SPRITE_WIDTH, getY())) {
			moveTo(getX() + 1, getY());
		}
		else {
			setDirection(180);
			if (getWorld().canStablyMoveTo(getX() - SPRITE_WIDTH, getY())
				&& getWorld().canMoveTo(getX() - 1, getY())) {
				moveTo(getX() - 1, getY());
			}
		}
	}
	else {
		if (getWorld().canStablyMoveTo(getX() - SPRITE_WIDTH, getY())) {
			moveTo(getX() - 1, getY());
		}
		else {
			setDirection(0);
			if (getWorld().canStablyMoveTo(getX() + SPRITE_WIDTH, getY())
				&& getWorld().canMoveRightTo(getX() + 1, getY())) {
				moveTo(getX() + 1, getY());
			}
		}
	}
}

/*overriden doSomething implementation for piranhas*/
void Piranha::doSomething()
{
	increaseAnimationNumber();

	if (getWorld().hasPlayer(getX(), getY())) {
		getWorld().getPlayer().bonk();
		return;
	}

	double dist = 1.5 * SPRITE_HEIGHT;

	if (!(getWorld().getPlayer().getY() <= getY() + dist &&
		getWorld().getPlayer().getY() >= getY() - dist)) {
		return;
	}

	if (getWorld().getPlayer().getX() < getX()) {
		setDirection(180);
	}

	if (getWorld().getPlayer().getX() > getX()) {
		setDirection(0);
	}

	if (firingDelay > 0) {
		firingDelay--;
		return;
	}

	double peachDist = abs(getWorld().getPlayer().getX() - getX());

	if (peachDist < 8 * SPRITE_WIDTH) {

		PiranhaFireball* temp = new PiranhaFireball(&getWorld(), IID_PIRANHA_FIRE, getX(), getY(), getDirection());
		getWorld().addActor(temp);

		getWorld().playSound(SOUND_PIRANHA_FIRE);
		firingDelay = 40;
	}
}
