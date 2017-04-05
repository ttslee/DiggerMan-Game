#include "Actor.h"
#include "StudentWorld.h"

enum Direction;
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
double Actor::distanceFromActor(std::shared_ptr<Actor> a)
{
	return sqrt(pow(getX() - a->getX(), 2) + pow(getY() - a->getY(), 2));
}
bool Actor::isDirt()
{
	bool flag = false;
	int i = 0;
	//high_resolution_clock::time_point t1 = high_resolution_clock::now();
	
	/*for (auto & y : *(getWorld()->getDirt()))
	{
		if (y[i]->getY() - getY() >= 4)
			break;
		if (y[i]->getY() < getY())
		{
			i++;
			continue;
		}
		for (auto & x : y)
		{
			if (x->isDead())
				continue;
			if ((x->getX() - getX()) >= 0 && (x->getX() - getX()) <= 3 && (x->getY() - getY()) >= 0 && (x->getY() - getY()) <= 3)
			{
				x->setVisible(false);
				x->setDead(true);
				flag = true;
			}
		}
		i++;
	}*/
	
	if (getY() < 60)
	{
		std::vector<std::vector<std::unique_ptr<Dirt>>>* dirt = getWorld()->getDirt();
		for (int i = getY(); i < getY() + SPRITE_HEIGHT; i++)
		{
			if (i >= 60)
				break;
			for (int j = getX(); j < getX() + SPRITE_WIDTH; j++)
			{
				if (!(*dirt)[i][j]->isDead())
				{
					(*dirt)[i][j]->setDead(true);
					(*dirt)[i][j]->setVisible(false);
					flag = true;
				}
			}
		}
	}
	//high_resolution_clock::time_point t2 = high_resolution_clock::now();

	//auto duration = duration_cast<nanoseconds>(t2 - t1).count();

	//std::cout << duration;
	return flag;
}
bool Actor::isTypeActorInFront(Direction dir, ActorType type)
{
	switch (type)
	{
	case boulder:
		for (auto & a : getWorld()->getActors())
		{
			if (!dynamic_cast<Boulder*>(a.get()))
				break;
			if (!a->isDead() && (a->getY() - getY() <= 4 && a->getY() - getY() >= -4) && (a->getX() - getX() <= 4 && a->getX() - getX() >= -4))
			{
				switch (dir)
				{
				case up:
					return isAbove(a);
					break;
				case down:
					return isBelow(a);
					break;
				case left:
					return isLeft(a);
					break;
				case right:
					return isRight(a);
					break;
				}
			}
		}
		break;
	case digger:
		switch (dir)
		{
		case up:
			return isAbove(static_cast<std::shared_ptr<Actor>>(getWorld()->getDiggerMan()));
			break;
		case down:
			return isBelow(static_cast<std::shared_ptr<Actor>>(getWorld()->getDiggerMan()));
			break;
		case left:
			return isLeft(static_cast<std::shared_ptr<Actor>>(getWorld()->getDiggerMan()));
			break;
		case right:
			return isRight(static_cast<std::shared_ptr<Actor>>(getWorld()->getDiggerMan()));
			break;
		default:
			return false;
		}
		break;
	case protester:
		for (auto & p : getWorld()->getProtesters())
		{
			if (distanceFromActor(p) < 4)
			{
				p->decHealth();
				getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
				return true;
			}
		}
		/*for (auto & p : getWorld()->getProtesters())
		{
			switch (dir)
			{
			case up:
				if (isAbove(static_cast<std::shared_ptr<Actor>>(p)) && (p->getX() - getX() <= SPRITE_HEIGHT))
				{
					p->decHealth();
					getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
					return true;
				}
				break;
			case down:
				if (isBelow(static_cast<std::shared_ptr<Actor>>(p)) && (p->getX() - getX() <= SPRITE_HEIGHT))
				{
					p->decHealth();
					getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
					return true;
				}
				break;
			case left:
				if (isLeft(static_cast<std::shared_ptr<Actor>>(p)) && (p->getX() - getX() <= SPRITE_WIDTH))
				{
					p->decHealth();
					getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
					return true;
				}
				break;
			case right:
				if (isRight(static_cast<std::shared_ptr<Actor>>(p)) && (p->getX() - getX() <= SPRITE_WIDTH))
				{
					p->decHealth();
					getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
					return true;
				}
				break;
			default:
				return false;
			}
		}*/
	}
	return false;
}
bool Actor::isAbove(std::shared_ptr<Actor> &a)
{
	return (a->getY() - getY() == SPRITE_HEIGHT && ((getX() >= a->getX() && getX() - a->getX() < SPRITE_WIDTH) || (a->getX() >= getX() && a->getX() - getX() < SPRITE_WIDTH)));
}
bool Actor::isBelow(std::shared_ptr<Actor>&a)
{
	return (getY() - a->getY() == SPRITE_HEIGHT && ((getX() >= a->getX() && getX() - a->getX() < SPRITE_WIDTH) || (a->getX() >= getX() && a->getX() - getX() < SPRITE_WIDTH)));
}
bool Actor::isLeft(std::shared_ptr<Actor>&a)
{
	return (getX() - a->getX() == SPRITE_WIDTH && ((getY() >= a->getY() && getY() - a->getY() < SPRITE_HEIGHT) || (a->getY() >= getY() && a->getY() - getY() < SPRITE_HEIGHT)));
}
bool Actor::isRight(std::shared_ptr<Actor>&a)
{
	return (a->getX() - getX() == SPRITE_WIDTH && ((getY() >= a->getY() && getY() - a->getY() < SPRITE_HEIGHT) || (a->getY() >= getY() && a->getY() - getY() < SPRITE_HEIGHT)));
}
std::shared_ptr<DiggerMan> Actor::getDigger()
{
	return m_student_world->getDiggerMan();
}
void Squirt::doSomething()
{
	if (isDead())
		return;
	if (isDirt())
	{
		setVisible(false);
		setDead(true);
		return;
	}
	if (m_distance <= 10)
	{
		switch (getDirection())
		{
		case up:	
			moveTo(getX(), getY() + 1);
			break;
		case down:
			moveTo(getX(), getY() - 1);
			break;
		case left:
			moveTo(getX() - 1, getY());
			break;
		case right:
			moveTo(getX() + 1, getY());
			break;
		}
		if (!m_shotFlag){
			getWorld()->playSound(SOUND_PLAYER_SQUIRT);
			setShotFlag(true);
		}
		incDistance();
	}
	else
	{
		setVisible(false);
		setDead(true);
	}
	if (isTypeActorInFront(up, protester))
	{
		setVisible(false);
		setDead(true);
	}
}

//*************BOULDER******************
void Boulder::doSomething()
{
	if (isDead())
		return;
	if (!isDirtBelow())
	{
		if (delay())
		{
			fall();
			if (!m_soundCheck)
			{
				getWorld()->playSound(SOUND_FALLING_ROCK);
				m_soundCheck = true;
			}
		}
	}
	if (isFalling() && isDiggerBelow())
	{
		getWorld()->getDiggerMan()->setDead(true);
	}
}
bool Boulder::isDirtBelow()
{
	if (m_belowFlag)
	{
		/*int i = 0;
		int count = 0;
		for (auto & y : getWorld()->getDirt())
		{
			if (getY() - y[i]->getY() > 1 || getY() - y[i]->getY() < 1)
			{
				i++;
				continue;
			}
			for (auto & x : y)
			{
				
				if (getY() - x->getY() != 1)
					continue;
				if (x->isDead() && (x->getX() - getX() >= 0) && (x->getX() - getX() <= 3))
				{
					count++;
					if (count == 4)
					{
						setBelowFlag(false);
						break;
					}
				}
			}
			if (!m_belowFlag)
				break;
			i++;
		}*/
		auto count = 0;
		std::vector<std::vector<std::unique_ptr<Dirt>>>* dirt = getWorld()->getDirt();
		for (int x = getX(); x < getX() + SPRITE_WIDTH; x++)
		{
			if (count > 0)
				break;
			if (!(*dirt)[getY() - 1][x]->isDead())
				count++;
		}
		if (count == 0)
			setBelowFlag(false);
	}
	if (m_Falling)
	{
		/*int i = 0;
		int count = 0;
		for (auto & y : getWorld()->getDirt())
		{
			if (getY() - y[i]->getY() > 1)
			{
				i++;
				continue;
			}
			for (auto & x : y)
			{
				if (getY() - x->getY() != 1)
					continue;
				if (!x->isDead() && (x->getX() - getX() >= 0) && (x->getX() - getX() <= 3))
				{
					count++;
					if (count == 4)
					{
						setBelowFlag(true);
						break;
					}
				}
			}
			if (m_belowFlag)
				break;
			i++;
		}*/
		auto count = 0;
		std::vector<std::vector<std::unique_ptr<Dirt>>>* dirt = getWorld()->getDirt();
		for (int x = getX(); x < getX() + SPRITE_WIDTH; x++)
		{
			if ((*dirt)[getY() - 1][x]->isDead())
				count++;
		}
		if (count == 4)
			setBelowFlag(false);
		else if (count > 0)
			isDirt();
		else
			setBelowFlag(true);
	}
	return m_belowFlag;
}
bool Boulder::delay()
{
	if (m_delayTicks == 0)
		m_delayTicks = getWorld()->getTicks();
	if (getWorld()->getTicks() - m_delayTicks < 20)
		return false;
	else
	{
		m_Falling = true;
		return true;
	}
}
void Boulder::fall()
{
	if (getY() > 0)
		moveTo(getX(), getY() - 1);
	if (isDirtBelow() || getX() == 0)
	{
		setDead(true);
		setVisible(false);
	}
}
bool Boulder::isDiggerBelow()
{
	return ((getX() - getWorld()->getDiggerMan()->getX() <= 3 && getX() - getWorld()->getDiggerMan()->getX() >= -3) && (getY() - getWorld()->getDiggerMan()->getY() <= 4 && getY() - getWorld()->getDiggerMan()->getY() >= 0));
}

//**************DIGGERMAN****************
void DiggerMan::doSomething()
{
	if (isDead())
		return;
	if (getWorld()->getTicks() % 8 == 0) // Timer to control sounds played. without this the sound is cluttered
		m_digFlag = true;
	diggerAction();
	if (!isDead())
	{
		if (isDirt())
		{
			if (m_digFlag)
				getWorld()->playSound(SOUND_DIG);
			m_digFlag = false;
		}
	}
}
bool DiggerMan::isBoulder(Direction dir)
{
	return isTypeActorInFront(dir, boulder);
}
void DiggerMan::dropGold()
{
	switch (getDirection())
	{
	case up:
		getWorld()->addActor(std::make_shared<Nugget>(getWorld(), getX(), getY() + 4, true, false, true));
		break;
	case down:
		getWorld()->addActor(std::make_shared<Nugget>(getWorld(), getX(), getY() - 4, true, false, true));
		break;
	case left:
		getWorld()->addActor(std::make_shared<Nugget>(getWorld(), getX() - 4, getY(), true, false, true));
		break;
	case right:
		getWorld()->addActor(std::make_shared<Nugget>(getWorld(), getX() + 4, getY(), true, false, true));
		break;
	}
}
void DiggerMan::diggerAction()			
{
	int key;
	if (getWorld()->getKey(key) == true) 
	{
		switch (key)
		{
		case KEY_PRESS_UP:
			if (getDirection() != up)
				setDirection(up);
			if (!isBoulder(up))
			{
				if (getY() < 60)
					moveTo(getX(), getY() + 1);
			}
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down)
				setDirection(down);
			if (!isBoulder(down))
			{
				if (getY() > 0){
					moveTo(getX(), getY() - 1);
				}
			}
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() != left)
				setDirection(left);
			if (!isBoulder(left))
			{
				if (getX() > 0)
					moveTo(getX() - 1, getY());
			}
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right)
				setDirection(right);
			if (!isBoulder(right))
			{
				if (getX() < 60)
					moveTo(getX() + 1, getY());
			}
			break;
		case 'z':
		case 'Z':
			if (getGold() > 0)
			{
				dropGold();
				m_gold--;
			}
			break;
		case KEY_PRESS_SPACE:
			shoot();
			break;
		}
	}
}
void DiggerMan::shoot()			
{
	if (m_numSquirts > 0) 
	{
		switch (getDirection())
		{
		case up:
			m_gun = std::make_shared<Squirt>(getWorld(), getX(), getY() + 1, up);
			getWorld()->addActor(m_gun);
			break;
		case down:
			m_gun = std::make_shared<Squirt>(getWorld(), getX(), getY() - 1, down);
			getWorld()->addActor(m_gun);
			break;
		case left:
			m_gun = std::make_shared<Squirt>(getWorld(), getX() - 1, getY(), left);
			getWorld()->addActor(m_gun);
			break;
		case right:
			m_gun = std::make_shared<Squirt>(getWorld(), getX() + 1, getY(), right);
			getWorld()->addActor(m_gun);
			break;
		}
		m_gun->setVisible(true);
		decSquirts();
	}
}

//**************PROTESTERS****************
void Protester::doSomething()
{
	if (isDead())
		return;
	else if (!getWaitState())
	{
		if (getTickCount() % (getWaitTicks() + 1) == 0)
		{
			incTickCount();
			setWaitState(true);
			return;
		}
		if (getHealth() == 0)
		{
			setDead(true);
			setLeaveState(true);
			getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
			leave();
			return;
		}
		if (getSquaresWalked() % (getMaxSquares() + 1) == 0)
		{
			setSquaresWalked(1);
			protesterAction(findNewDirection());
		}
		else
			protesterAction(getDirection());
		incTickCount();
	}
	else
	{

		if (getTickCount() % getWaitTicks() == 0)
			setWaitState(false);
		incTickCount();
	}
}
//void RegularProtester::doSomething()
//{
//	if (isDead())
//		return;
//	else if (!getWaitState())
//	{
//		if (getTickCount() % (getWaitTicks() + 1) == 0)
//		{
//			incTickCount();
//			setWaitState(true);
//			return;
//		}
//		if (getHealth() == 0)
//		{
//			setDead(true);
//			setLeaveState(true);
//			getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
//			leave();	
//			return;
//		}
//		if (getSquaresWalked() % (getMaxSquares() + 1) == 0)
//		{
//			setSquaresWalked(1);
//			protesterAction(findNewDirection());
//		}
//		else
//			protesterAction(getDirection());
//		incTickCount();
//	}
//	else
//	{
//		
//		if (getTickCount() % getWaitTicks() == 0)
//			setWaitState(false);
//		incTickCount();
//	}
//}
//void HardcoreProtester::doSomething()
//{
//	if (isDead())
//		return;
//	else if (!getWaitState())
//	{
//		if (getTickCount() % (getWaitTicks() + 1) == 0)
//		{
//			incTickCount();
//			setWaitState(true);
//			return;
//		}
//		else if (getHealth() == 0)
//		{
//			setDead(true);
//			setLeaveState(true);
//			getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
//			leave();
//			return;
//		}
//		if (getSquaresWalked() % (getMaxSquares() + 1) == 0)
//		{
//			setSquaresWalked(1);
//			protesterAction(findNewDirection());
//		}
//		else
//			protesterAction(getDirection());
//		incTickCount();
//	}
//	else
//	{
//
//		if (getTickCount() % getWaitTicks() == 0)
//			setWaitState(false);
//		incTickCount();
//	}
//}
void Protester::protesterAction(Direction dir)
{
	switch (dir)
	{
	case up:
		break;
	case down:
		break;
	case left:
		if (getX() > 0)
		{
			moveTo(getX() - 1, getY());
			incSquaresWalked();
		}
		break;
	case right:
		break;
	}
	if (isTypeActorInFront(getDirection(), digger))
	{
		getWorld()->playSound(SOUND_PROTESTER_YELL);
		getWorld()->playSound(SOUND_PLAYER_ANNOYED);
		return;
	}
}
GraphObject::Direction Protester::findNewDirection()
{

	return m_current_dir;
}
auto Protester::type(ActorType ty)->bool
{
	switch (ty)
	{
	case regular:
		return dynamic_cast<RegularProtester*>(this);
	case hardcore:
		return dynamic_cast<HardcoreProtester*>(this);
	}
	return false;
}
auto Protester::wait()->void
{

}
auto Protester::initWaitTicks()->void
{
	m_wait_ticks = std::max(0, 3 - static_cast<int>(getWorld()->getLevel()) / 4);
}
auto Protester::setWalkDistance()->void
{
	m_num_squares = rand() % 53 + 8;
}
auto Protester::leave()->void
{

}


//***************GOODIES*****************
void Goodie::getPickedUp()
{
	if (getDiggerState())
		setDiggerState(false);
	else
		setProtesterState(false);
	setPickedUp(true);
	setVisible(false);
	setDead(true);

}
void Goodie::setPickedUp(bool flag)
{
	m_pickedUp = flag;
}

void Nugget::doSomething()
{
	if (isDead())
		return;
	if (getDiggerState())
	{
		if (distanceFromActor(getDigger()) > 4 && distanceFromActor(getDigger()) < 7)
		{
			setVisible(true);
			return;
		}
		else if (distanceFromActor(getDigger()) < 4)
			getPickedUp();
		if (isPickedUp())
		{
			getWorld()->getDiggerMan()->incGold();
			getWorld()->playSound(SOUND_GOT_GOODIE);
			return;
		}
	}
	if (getProtState())
	{
		for (auto p : getWorld()->getProtesters())
		{
			if (distanceFromActor(p) <= 3)
			{
				getPickedUp();
				getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
				p->incGold();
				p->wait();
				if (p->type(regular))
				{
					if (p->getGold() > 3)
						p->leave();
				}
			}
		}
	}
}
void Oil::doSomething() 
{
	if (distanceFromActor(getDigger()) > 4 && distanceFromActor(getDigger()) < 7)
		setVisible(true);
	else if (distanceFromActor(getDigger()) <= 3)
		getPickedUp();
	if (isPickedUp())
	{
		getWorld()->decOil();
		getWorld()->playSound(SOUND_FOUND_OIL);
	}
}
void Water::doSomething() 
{
	if (distanceFromActor(getDigger()) > 4 && distanceFromActor(getDigger()) < 7)
		setVisible(true);
	else if (distanceFromActor(getDigger()) <= 3)
		getPickedUp();
	if (isPickedUp())
	{
		getWorld()->getDiggerMan()->incWater();
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}
}

void Sonar::doSomething() 
{
	if (distanceFromActor(getDigger()) > 4 && distanceFromActor(getDigger()) < 7)
		setVisible(true);
	else if (distanceFromActor(getDigger()) <= 3)
		getPickedUp();
	if (isPickedUp())
	{
		getWorld()->getDiggerMan()->incSonar();
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}
}
auto Sonar::initLifeTicks()->void
{
	m_life_ticks = max(100,( 300 - 10 * static_cast<int>(getWorld()->getLevel())));
}