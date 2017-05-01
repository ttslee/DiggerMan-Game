#include "StudentWorld.h" // Actor.h is included in StudentWorld.h

double Actor::distanceFromActor(std::shared_ptr<Actor> a)
{
	return sqrt(pow(getX() - a->getX(), 2) + pow(getY() - a->getY(), 2));
}
bool Actor::clearDirt()
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
				continue;
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
			return isAbove(getDigger());
			break;
		case down:
			return isBelow(getDigger());
			break;
		case left:
			return isLeft(getDigger());
			break;
		case right:
			return isRight(getDigger());
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
				p->setStunned(true);
				getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
				return true;
			}
		}
		break;
	case dirt:
		switch (dir)
		{
		case up:
			if (getY() < 56)
			{
				for (int i = 0; i < 4; i++)
				{
					if (!(*getWorld()->getDirt())[getY() + 4][getX() + i]->isDead())
						return true;
				}
			}
			break;
		case down:
			if (getY() > 0)
			{
				for (int i = 0; i < 4; i++)
				{
					if (!(*getWorld()->getDirt())[getY() - 1][getX() + i]->isDead())
						return true;
				}
			}
			break;
		case left:
			if (getX() > 0 && getY() < 56)
			{
				for (int i = 0; i < 4; i++)
				{
					if (!(*getWorld()->getDirt())[getY() + i][getX() - 1]->isDead())
						return true;
				}
			}
			break;
		case right:
			if (getX() < 59 && getY() < 56)
				for (int i = 0; i < 4; i++)
				{
					if (!(*getWorld()->getDirt())[getY() + i][getX() + 4]->isDead())
						return true;
				}
			break;
		}
		return false;
		break;
	}
	return false;
}
template<typename T>
bool Actor::isAbove(T& a)
{
	return (a->getY() - getY() == SPRITE_HEIGHT && ((getX() >= a->getX() && getX() - a->getX() < SPRITE_WIDTH) || (a->getX() >= getX() && a->getX() - getX() < SPRITE_WIDTH)));
}
template<typename T>
bool Actor::isBelow(T& a)
{
	return (getY() - a->getY() == SPRITE_HEIGHT && ((getX() >= a->getX() && getX() - a->getX() < SPRITE_WIDTH) || (a->getX() >= getX() && a->getX() - getX() < SPRITE_WIDTH)));
	/*(getY() - a->getY() == SPRITE_HEIGHT && ((getX() >= a->getX() && getX() - a->getX() < SPRITE_WIDTH) || (a->getX() >= getX() && a->getX() - getX() < SPRITE_WIDTH)));*/
}
template<typename T>
bool Actor::isLeft(T& a)
{
	return (getX() - a->getX() == SPRITE_WIDTH && ((getY() >= a->getY() && getY() - a->getY() < SPRITE_HEIGHT) || (a->getY() >= getY() && a->getY() - getY() < SPRITE_HEIGHT)));
}
template<typename T>
bool Actor::isRight(T& a)
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
		getWorld()->getEmptySquares()->emplace_back(getX(), getY());
		return;
	}
	if (m_distance <= 10)
	{
		switch (getDirection())
		{
		case up:
			if (getY() == 60)
				moveTo(getX(), getY() + 1);
			break;
		case down:
			if (getY() > 0)
				moveTo(getX(), getY() - 1);
			break;
		case left:
			if (getX() > 0)
				moveTo(getX() - 1, getY());
			break;
		case right:
			if (getX() < 60)
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
	if (!isFalling() && !isDirtBelow())
	{
		setFalling(true);
	}
	if (isFalling())
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
		{
			setBelowFlag(false);
		}
	}
	if (m_Falling)
	{
		auto count = 0;
		std::vector<std::vector<std::unique_ptr<Dirt>>>* dirt = getWorld()->getDirt();
		for (int x = getX(); x < getX() + SPRITE_WIDTH; x++)
		{
			if (getY() - 1 == 0)
			{
				setDead(true);
				setVisible(false);
				return true;
			}
			if ((*dirt)[getY() - 1][x]->isDead())
				count++;
		}
		if (count == 4){}
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
		return true;
}
void Boulder::fall()
{
	if (isBelow(getDigger()))
		getDigger()->setDead(true);
	if (getY() > 0)
		moveTo(getX(), getY() - 1);
	if (isDirtBelow() || getX() == 0)
	{
		setDead(true);
		setVisible(false);
	}
	(*getWorld()->getRunawayGrid())[getY()][getX()]->setWalkFlag(true);
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
		if (clearDirt())
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
				if (!getWorld()->containsSquare(getX(), getY()))
					getWorld()->getEmptySquares()->emplace_back(getX(), getY());
				(*getWorld()->getRunawayGrid())[getY()][getX()]->setWalkFlag(true);
			}
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down)
				setDirection(down);
			if (!isBoulder(down))
			{
				if (getY() > 0)
					moveTo(getX(), getY() - 1);
				if (!getWorld()->containsSquare(getX(), getY()))
					getWorld()->getEmptySquares()->emplace_back(getX(), getY());
				(*getWorld()->getRunawayGrid())[getY()][getX()]->setWalkFlag(true);
			}
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() != left)
				setDirection(left);
			if (!isBoulder(left))
			{
				if (getX() > 0)
					moveTo(getX() - 1, getY());
				if (!getWorld()->containsSquare(getX(), getY()))
					getWorld()->getEmptySquares()->emplace_back(getX(), getY());
				(*getWorld()->getRunawayGrid())[getY()][getX()]->setWalkFlag(true);
			}
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right)
				setDirection(right);
			if (!isBoulder(right))
			{
				if (getX() < 60)
					moveTo(getX() + 1, getY());
				if (!getWorld()->containsSquare(getX(), getY()))
					getWorld()->getEmptySquares()->emplace_back(getX(), getY());
				(*getWorld()->getRunawayGrid())[getY()][getX()]->setWalkFlag(true);
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
	if (getLeaveState())
	{
		leave();
		return;
	}
	if (getHealth() == 0)
	{
		setLeaveState(true);
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
	}
	if (isStunned())
	{
		stunned();
		return;
	}
	if (getDigger()->getX() == getX())
	{
		if (getDigger()->getY() > getY())
		{
			if (checkIfClear(up))
				setDirection(up);
		}
		else
		{
			if (checkIfClear(down))
				setDirection(down);
		}
	}
	else if (getDigger()->getY() == getY())
	{
		if (getDigger()->getX() < getX())
		{
			if (checkIfClear(left))
				setDirection(left);
		}
		else
		{
			if (checkIfClear(right))
				setDirection(right);
		}
	}

	if (!isWaiting())
	{
		if (getTickCount() % (getWaitDuration() + 1) == 0)
		{
			incTickCount();
			setWaitState(true);
			return;
		}
		if (getSquaresWalked() % (getMaxSquares() + 1) == 0)
		{
			setSquaresWalked(1);
			protesterAction(getDirection());
		}
		else
			protesterAction(getDirection());
		incTickCount();
	}
	else
	{
		if (getTickCount() % getWaitDuration() == 0)
			setWaitState(false);
		incTickCount();
	}
}
void Protester::protesterAction(Direction dir)
{
	switch (dir)
	{
	case up:
		if (getY() < 60)
		{
			if (getDirection() != up)
				setDirection(up);
			if (!isTypeActorInFront(up, dirt) && !isTypeActorInFront(up, boulder))
			{
				moveTo(getX(), getY() + 1);
				incSquaresWalked();
			}
		}
		break;
	case down:
		if (getY() > 0)
		{
			if (getDirection() != down)
				setDirection(down);
			if (!isTypeActorInFront(down, dirt) && !isTypeActorInFront(down, boulder))
			{
				moveTo(getX(), getY() - 1);
				incSquaresWalked();
			}
		}
		break;
	case left:
		if (getX() > 0)
		{
			if (getDirection() != left)
				setDirection(left);
			if (!isTypeActorInFront(left, dirt) && !isTypeActorInFront(left, boulder))
			{
				moveTo(getX() - 1, getY());
				incSquaresWalked();
			}
		}
		break;
	case right:
		if (getX() < 60)
		{
			if (getDirection() != right)
				setDirection(right);
			if (!isTypeActorInFront(right, dirt) && !isTypeActorInFront(right, boulder))
			{
				moveTo(getX() + 1, getY());
				incSquaresWalked();
			}
		}
		break;
	}
	if (isTypeActorInFront(getDirection(), digger))
	{
		getWorld()->playSound(SOUND_PROTESTER_YELL);
		getDigger()->setHealth(20);
		setStunned(true);
		return;
	}
}
GraphObject::Direction Protester::findNewDirection()
{
	return m_current_dir;
}
auto Protester::checkIfClear(Direction dir)->bool
{
	int currentY = getY();
	int currentX = getX();
	auto actors = getWorld()->getActors();
	auto dirt = getWorld()->getDirt();
	bool flag = true;
	switch (dir)
	{
	case up:
		if (getY() < 56)
		{
			for (int k = 0; k < getWorld()->getBoulder(); k++)
			{
				if (!actors[k]->isDead() && actors[k]->getX() == getX() && actors[k]->getY() < getDigger()->getY() && actors[k]->getY() > getY())
					return false;
			}
			for (int d = getY() + 4; d < getDigger()->getY(); d++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (!(*dirt)[d][getX() + j]->isDead())
						flag = false;
				}
			}
		}
		break;
	case down:
		if (getY() > 0)
		{
			for (int k = 0; k < getWorld()->getBoulder(); k++)
			{
				if (!actors[k]->isDead() && actors[k]->getX() == getX() && actors[k]->getY() > getDigger()->getY() && actors[k]->getY() < getY())
					return false;
			}
			for (int d = getY() - 1; d > getDigger()->getY(); d--)
			{
				for (int j = 0; j < 4; j++)
				{
					if (!(*dirt)[d][getX() + j]->isDead())
						flag = false;
				}
			}
		}
		break;
	case left:
		if (getX() > 0 && getY() < 56)
		{
			for (int k = 0; k < getWorld()->getBoulder(); k++)
			{
				if (!actors[k]->isDead() && actors[k]->getY() == getY() && actors[k]->getX() > getDigger()->getX() && actors[k]->getX() < getX())
					return false;
			}
			for (int d = getX() - 1; d > getDigger()->getX(); d--)
			{
				for (int j = 0; j < 4; j++)
				{
					if (!(*dirt)[getY() + j][d]->isDead())
						flag = false;
				}
			}
		}
		break;
	case right:
		if (getX() < 59 && getY() < 56)
		{
			for (int k = 0; k < getWorld()->getBoulder(); k++)
			{
				if (!actors[k]->isDead() && actors[k]->getY() == getY() && actors[k]->getX() < getDigger()->getX() && actors[k]->getX() > getX())
					return false;
			}
			for (int d = getX() + 4; d < getDigger()->getX(); d++)
			{
				if (d > 57)
					break;
				for (int j = 0; j < 4; j++)
				{
					if (!(*dirt)[getY() + j][d]->isDead())
						flag = false;
				}
			}
		}
		break;
	}
	return flag;
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
auto Protester::initWalkDistance()->void
{
	m_max_squares = rand() % 61;
}
auto Protester::initWaitTicks()->void
{
	m_wait_duration = std::max(0, 3 - static_cast<int>(getWorld()->getLevel()) / 4);
}
auto Protester::initStunDuration()->void
{
	m_stun_duration = std::max(50, 100 - static_cast<int>(getWorld()->getLevel()) * 10);
}

auto Protester::leave()->void
{
	if (!updated())
	{
		getWorld()->updateRunGrid(getX(), getY());
		fillLeaveQueue(getX(), getY());
		m_updated = true;
	}
	if (updated())
	{
		moveOut();
	}
}
auto Protester::moveOut()->void
{
	if (getX() == 60 && getY() == 60)
	{
		setDead(true);
		setVisible(false);
		getWorld()->decProtCount();
		return;
	}
	if (!m_leaveQ.empty())
	{
		switch (m_leaveQ.front())
		{
		case up:
			if(getDirection() != up)
				setDirection(up);
			moveTo(getX(), getY() + 1);
			break;
		case down:
			if (getDirection() != down)
				setDirection(down);
			moveTo(getX(), getY() - 1);
			break;
		case left:
			if (getDirection() != left)
				setDirection(left);
			moveTo(getX() - 1, getY());
			break;
		case right:
			if (getDirection() != right)
				setDirection(right);
			moveTo(getX() + 1, getY());
			break;
		}
		m_leaveQ.pop();
	}
}
auto Protester::fillLeaveQueue(int x, int y)->void
{
	//!(x==60 && y + 1 == 60)
	std::shared_ptr<RunawayBox> currentBox = (*getWorld()->getRunawayGrid())[y][x];
	while (currentBox->getDistance() > 0)
	{
		currentBox = (*getWorld()->getRunawayGrid())[y][x];
		if (y < 60 && (*getWorld()->getRunawayGrid())[y + 1][x]->getDistance() == currentBox->getDistance() - 1)
		{
			m_leaveQ.push(up);
			y++;
			continue;
		}
		if (x > 0 && (*getWorld()->getRunawayGrid())[y][x - 1]->getDistance() == currentBox->getDistance() - 1)
		{
			m_leaveQ.push(left);
			x--;
			continue;
		}
		if (x < 60 && (*getWorld()->getRunawayGrid())[y][x + 1]->getDistance() == currentBox->getDistance() - 1)
		{
			m_leaveQ.push(right);
			x++;
			continue;
		}
		if (y > 0 && (*getWorld()->getRunawayGrid())[y - 1][x]->getDistance() == currentBox->getDistance() - 1)
		{
			m_leaveQ.push(down);
			y--;
			continue;
		}
		
	}
}

auto Protester::stunned()->void
{
	if (m_stun_ticks == 0)
		m_stun_ticks = getWorld()->getTicks();
	if (getWorld()->getTicks() - m_stun_ticks < m_stun_duration)
		return;
	else
	{
		setStunned(false);
		m_stun_ticks = 0;
	}
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
				p->setStunned(true);
				if (p->type(regular))
				{
					if (p->getGold() > 3)
						p->setLeaveState(true);
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


auto TemporaryGoodie::initLifeTicks()->void
{
	m_life_ticks = std::max(100, (300 - 10 * static_cast<int>(getWorld()->getLevel())));
}
void TemporaryGoodie::doSomething()
{
	if (isDead())
		return;
	if (distanceFromActor(getDigger()) <= 3)
		getPickedUp();
	if (isPickedUp())
	{
		switch (getImgID())
		{
		case IMID_WATER_POOL:
			getDigger()->incWater();
			break;
		case IMID_SONAR:
			getDigger()->incSonar();
			break;
		}
		getWorld()->playSound(getSoundID());
	}
	incTicks();
	if (getTicks() == getLifeTicks())
	{
		setDead(true);
		setVisible(false);
	}
}
