#include "Actor.h"
#include "StudentWorld.h"
double Actor::distanceFromActor(std::shared_ptr<Actor> a)
{
	return sqrt(pow(getX() - a->getX(), 2) + pow(getY() - a->getY(), 2));
}
bool Actor::isDirt()
{
	bool flag = false;
	int i = 0;
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
			return isAbove(static_cast<std::shared_ptr<Actor>>(getDigger()));
			break;
		case down:
			return isBelow(static_cast<std::shared_ptr<Actor>>(getDigger()));
			break;
		case left:
			return isLeft(static_cast<std::shared_ptr<Actor>>(getDigger()));
			break;
		case right:
			return isRight(static_cast<std::shared_ptr<Actor>>(getDigger()));
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
		getDigger()->setDead(true);
	}
}
bool Boulder::isDirtBelow()
{
	if (m_belowFlag)
	{
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
	return ((getX() - getDigger()->getX() <= 3 && getX() - getDigger()->getX() >= -3) && (getY() - getDigger()->getY() <= 4 && getY() - getDigger()->getY() >= 0));
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
void DiggerMan::setHealth(int damage)
{
	m_health -= damage;
	if (m_health <= 0)
		setDead(true);
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
		case KEY_PRESS_TAB:
			if (getGold() > 0)
			{
				dropGold();
				m_gold--;
			}
			break;
		case 'Z':
		case 'z':
			if (getSonarCount() > 0)
			{
				setSonarFlag(true);
				decSonar();
			}
			break;
		case KEY_PRESS_SPACE:
			shoot();
			break;
		case KEY_PRESS_ESCAPE:
			setDead(true);
			break;
		}
	}
}
void DiggerMan::initSonarDuration()
{

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
		if (getTickCount() % (getWaitDuration() + 1) == 0)
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
	else if (getLeaveState())
	{

	}
	else
	{
		if (getTickCount() % getWaitDuration() == 0)
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
		if (getY() < 60)
		{
			moveTo(getX() - 1, getY());
			incSquaresWalked();
		}
		break;
	case down:
		if (getY() > 0)
		{
			moveTo(getX() - 1, getY());
			incSquaresWalked();
		}
		break;
	case left:
		if (getX() > 0)
		{
			moveTo(getX() - 1, getY());
			incSquaresWalked();
		}
		break;
	case right:
		if (getX() < 60)
		{
			moveTo(getX() - 1, getY());
			incSquaresWalked();
		}
		break;
	}
	if (isTypeActorInFront(getDirection(), digger))
	{
		getWorld()->playSound(SOUND_PROTESTER_YELL);
		getDigger()->setHealth(20);
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
auto Protester::initWalkDistance()->void
{
	m_max_squares = rand() % 53 + 8;
}
auto Protester::initWaitTicks()->void
{
	m_wait_duration = std::max(0, 3 - static_cast<int>(getWorld()->getLevel()) / 4);
}
auto Protester::initStunDuration()->void
{
	m_stun_duration = std::max(50, 100 - static_cast<int>(getWorld()->getLevel()) * 10);
}
auto Protester::setWaitDuration(int dur)->void
{
	m_wait_duration = dur;
}
auto Protester::leave()->void
{
	setLeaveState(true);
	getWorld()->getShortestPathOut(getX(), getY());
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
	else if (getDiggerState())
	{
		if (getDigger()->getSonarFlag())
		{
			if (distanceFromActor(getDigger()) < 12)
			{
				setVisible(true);
			}
		}
		else if (!isPickedUp())
		{
			if (distanceFromActor(getDigger()) < 7)
			{
				setVisible(true);
			}
		}
		if (distanceFromActor(getDigger()) <= 3)
			getPickedUp();
		if (isPickedUp())
		{
			getDigger()->incGold();
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
	if (isDead())
		return;
	if (getDiggerState())
	{
		if (getDigger()->getSonarFlag())
		{
			if (distanceFromActor(getDigger()) < 12)
				setVisible(true);
		}
		else
		{
			if (distanceFromActor(getDigger()) < 7)
				setVisible(true);
		}
		if (distanceFromActor(getDigger()) <= 3)
			getPickedUp();
		if (isPickedUp())
		{
			getWorld()->decOil();
			getWorld()->playSound(SOUND_FOUND_OIL);
		}
	}
}
void Water::doSomething()
{
	if (isDead())
		return;
	if (distanceFromActor(getDigger()) <= 3)
		getPickedUp();
	if (isPickedUp())
	{
		getDigger()->incWater();
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}
}

void Sonar::doSomething()
{
	if (isDead())
		return;
	else if (!isPickedUp())
	{
		if (distanceFromActor(getDigger()) <= 3)
		{
			getPickedUp();
			getDigger()->incSonar();
		}
		if (isPickedUp())
		{
			getWorld()->playSound(SOUND_GOT_GOODIE);
		}
	}
	m_ticks++;
	if (m_ticks == m_life_ticks)
	{
		setDead(true);
		setVisible(false);
	}
}
auto Sonar::initLifeTicks()->void
{
	m_life_ticks = std::max(100, (300 - 10 * static_cast<int>(getWorld()->getLevel())));
}