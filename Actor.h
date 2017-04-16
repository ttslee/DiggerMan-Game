#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"
#include <memory>
#include <vector>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <string>
#include <cassert>
using namespace std::chrono;
enum ActorType {
	digger, boulder, water, gold, oil, sonar, squirt, hardcore, regular, protester
};
class StudentWorld;
class DiggerMan;
class Actor : public GraphObject
{
public:
	Actor(StudentWorld* sw, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0) :
		GraphObject(imageID, startX, startY, dir, size, depth),
		m_isDead(false),
		m_student_world(sw)
	{
	}
	virtual void doSomething() = 0;
	virtual bool clearDirt();
	void setDead(bool state)
	{
		m_isDead = state;
	}
	const bool isDead()
	{
		return m_isDead;
	}
	StudentWorld* getWorld()
	{
		return m_student_world;
	}
	std::shared_ptr<DiggerMan> getDigger();
	double distanceFromActor(std::shared_ptr<Actor>);
	bool isTypeActorInFront(Direction dir, ActorType type);
	auto isAbove(std::shared_ptr<Actor>&)->bool;
	auto isBelow(std::shared_ptr<Actor>&)->bool;
	auto isLeft(std::shared_ptr<Actor>&)->bool;
	auto isRight(std::shared_ptr<Actor>&)->bool;

private:
	bool m_isDead;
	StudentWorld* m_student_world;
};


//***********BOULDER, DIRT, SQUIRT, DIGGERMAN*************
class Boulder : public Actor
{
public:
	Boulder(StudentWorld* sw, int startX, int startY) :
		Actor(sw, IMID_BOULDER, startX, startY),
		m_initial_dirt_flag(true),
		m_belowFlag(true),
		m_Falling(false),
		m_delayTicks(0),
		m_soundCheck(false)
	{
		setVisible(true);
		clearDirt();
		std::cout << "Boulder Created" << std::endl;
	}
	virtual void doSomething();

	auto setFalling(bool flag)->void
	{
		m_Falling = flag;
	}
	auto isFalling()->bool
	{
		return m_Falling;
	}
	auto setBelowFlag(bool flag)->void
	{
		m_belowFlag = flag;
	}
private:
	auto isDirtBelow()->bool;
	auto isDiggerBelow()->bool;
	auto delay()->bool;
	auto fall()->void;

	int m_delayTicks;
	bool m_initial_dirt_flag;
	bool m_belowFlag;
	bool m_Falling;
	bool m_soundCheck;
};

class Dirt : public Actor
{
public:
	Dirt(StudentWorld* sw, int startX, int startY, bool visible = true, bool dead = false) :
		Actor(sw, IMID_DIRT, startX, startY, right, 0.25, 3)
	{
		setVisible(visible);
		setDead(dead);
	}
	virtual void doSomething() {}
};

class Squirt :public Actor
{
public:
	Squirt(StudentWorld* sw, int startX, int startY, Direction dir) :
		Actor(sw, IMID_WATER_SPURT, startX, startY, dir),
		m_shotFlag(false),
		m_distance(0)
	{
	}
	virtual void doSomething();
	void incDistance()
	{
		m_distance++;
	}
	void setShotFlag(bool flag)
	{
		m_shotFlag = flag;
	}

private:
	bool m_shotFlag;
	int m_distance;
};

class DiggerMan : public Actor
{
public:
	DiggerMan(StudentWorld* sw) :
		Actor(sw, IMID_PLAYER, 30, 60),
		m_gun(),
		m_gunFlag(false),
		m_sonarFlag(false),
		m_health(100),
		m_numSquirts(5),
		m_digFlag(true),
		m_gold(0),
		m_sonarCount(0)
	{
		setVisible(true);
	}
	virtual void doSomething();

	void decSonar()
	{
		m_sonarCount--;
	}
	void decSquirts()
	{
		m_numSquirts--;
	}
	void decGold()
	{
		m_gold--;
	}
	void incGold()
	{
		m_gold++;
	}
	void incWater()
	{
		m_numSquirts += 5;
	}
	void incSonar()
	{
		m_sonarCount++;
	}
	const int getHealth()
	{
		return m_health;
	}
	const int getNumSquirts()
	{
		return m_numSquirts;
	}
	const int getGold()
	{
		return m_gold;
	}
	const int getSonarCount()
	{
		return m_sonarCount;
	}
	const int getSonarFlag()
	{
		return m_sonarFlag;
	}
	void setSonarFlag(bool flag)
	{
		m_sonarFlag = flag;
	}
	void setHealth(int damage);
	void setGunFlag(bool flag)
	{
		m_gunFlag = flag;
	}
	bool isBoulder(Direction dir);
	void dropGold();
protected:
	void diggerAction();
	void shoot();
private:
	std::shared_ptr<Squirt> m_gun;
	bool m_gunFlag;
	bool m_digFlag;
	bool m_sonarFlag;
	int  m_sonarCount;
	int m_sonarDuration;
	int	 m_health;
	int	 m_numSquirts;
	int  m_gold;

};

//*********************PROTESTERS**************************
class Protester : public Actor
{
public:
	Protester(StudentWorld* sw, int imagID) :
		Actor(sw, imagID, 60, 60, left, 1.0, 0),
		m_goldCount(0),
		m_health(5),
		m_tick_count(1),
		m_current_dir(left),
		m_squares_walked(1),
		m_leave_state(false),
		m_wait_state(false),
		m_stunned(false)
	{
		setVisible(true);
		initWaitTicks();
		initWalkDistance();
		initStunDuration();
	}
	virtual void doSomething();

	auto incTickCount()->void
	{
		m_tick_count++;
	}
	void incGold()
	{
		m_goldCount++;
	}
	void incSquaresWalked()
	{
		m_squares_walked++;
	}
	void decHealth()
	{
		m_health--;
	}

	auto getSquaresWalked()->int
	{
		return m_squares_walked;
	}
	auto getMaxSquares()->int
	{
		return m_max_squares;
	}
	auto getTickCount()->int
	{
		return m_tick_count;
	}
	auto getGold()->int
	{
		return m_goldCount;
	}
	auto getHealth()->int
	{
		return m_health;
	}
	auto getWaitState()->bool
	{
		return m_wait_state;
	}
	auto getWaitDuration()->int
	{
		return m_wait_duration;
	}
	auto getLeaveState()->int
	{
		return m_leave_state;
	}

	auto setLeaveState(bool flag)->void
	{
		m_leave_state = flag;
	}
	auto setSquaresWalked(int sqrs)->void
	{
		m_squares_walked = sqrs;
	}
	auto setWaitState(bool flag)->void
	{
		m_wait_state = flag;
	}
	auto setWaitDuration(int)->void;

	auto initWalkDistance()->void;
	auto initWaitTicks()->void;
	auto initStunDuration()->void;
	auto type(ActorType)->bool;
	auto stunned()->void;
	auto leave()->void;
	Direction findNewDirection();
private:
	void protesterAction(Direction dir);

	int m_goldCount;
	int m_health;
	int m_tick_count;
	int m_max_squares;
	int m_squares_walked;
	int m_wait_duration;
	int m_stun_duration;
	Direction m_current_dir;

	bool m_leave_state;
	bool m_wait_state;
	bool m_stunned;
};


class RegularProtester : public Protester
{
public:
	RegularProtester(StudentWorld* sw) :
		Protester(sw, IMID_PROTESTER)
	{
	}
	//virtual void doSomething();
};

class HardcoreProtester : public Protester
{
public:
	HardcoreProtester(StudentWorld* sw) :
		Protester(sw, IMID_HARD_CORE_PROTESTER)
	{
	}
	//virtual void doSomething();
};

//***********************GOODIES***************************
class Goodie : public Actor
{
public:
	Goodie(StudentWorld* sw, int imigID, int startX, int startY, unsigned int depth = 2, bool digSt = true, bool protSt = false, bool pickUpSt = false) :
		Actor(sw, imigID, startX, startY, right, 1.0, depth),
		m_digger_state(digSt),
		m_protester_state(protSt),
		m_pickedUp(pickUpSt)
	{
	}
	virtual void doSomething(){}
	void setDiggerState(bool state)
	{
		m_digger_state = state;
	}
	void setProtesterState(bool state)
	{
		m_protester_state = state;
	}

	const bool getDiggerState()
	{
		return m_digger_state;
	}
	const bool getProtState()
	{
		return m_protester_state;
	}
	void getPickedUp();
	void setPickedUp(bool flag);
	bool isPickedUp()
	{
		return m_pickedUp;
	}

private:
	bool m_digger_state;
	bool m_protester_state;
	bool m_pickedUp;
};


class Nugget : public Goodie
{
public:
	Nugget(StudentWorld* sw, int startX, int startY, bool setV = false, bool digSt = true, bool protSt = false) :
		Goodie(sw, IMID_GOLD, startX, startY, 2, digSt, protSt)
	{
		setVisible(setV);
	}
	virtual void doSomething();
};

class Oil : public Goodie
{
public:
	Oil(StudentWorld* sw, int startX, int startY) :
		Goodie(sw, IMID_BARREL, startX, startY)
	{
		setVisible(false);
	}
	virtual void doSomething();
};

class TemporaryGoodie : public Goodie
{
public:
	TemporaryGoodie(StudentWorld* sw, int imID, int startX, int startY, int sndID) :
		Goodie(sw, imID, startX, startY),
		m_ticks(0),
		soundID(sndID),
		imgID(imID)
	{
		setVisible(true);
		initLifeTicks();
	}
	virtual void doSomething();
	auto initLifeTicks()->void;
	auto incTicks()->void
	{
		m_ticks++;
	}
	auto getTicks()->int
	{
		return m_ticks;
	}
	auto getSoundID()->int
	{
		return soundID;
	}
	auto getImgID()->int
	{
		return imgID;
	}
	auto getLifeTicks()->int
	{
		return m_life_ticks;
	}
private:
	int soundID;
	int imgID;
	int m_life_ticks;
	int m_ticks;
};
class Water : public TemporaryGoodie
{
public:
	Water(StudentWorld* sw, int startX, int startY) :
		TemporaryGoodie(sw, IMID_WATER_POOL, startX, startY, SOUND_GOT_GOODIE)
	{
	}
};

class Sonar : public TemporaryGoodie
{
public:
	Sonar(StudentWorld* sw, int startX, int startY) :
		TemporaryGoodie(sw, IMID_SONAR, startX, startY, SOUND_SONAR)
	{
	}
};

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_
