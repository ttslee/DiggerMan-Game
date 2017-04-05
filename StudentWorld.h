#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "GameWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <algorithm>
#include <time.h>
#include <cassert>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir), m_ticks(1)
	{
		srand(static_cast<unsigned int>(time(nullptr)));
	}
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	std::string formatGameStats() const;
	void addActor(std::shared_ptr<Actor> obj);

	std::shared_ptr<DiggerMan>				getDiggerMan()
	{
		return m_diggerMan;
	}
	std::vector<std::shared_ptr<Protester>>	getProtesters()	
	{ 
		return m_protesters; 
	}
	std::vector<std::shared_ptr<Actor>>		getActors()		
	{
		return m_actors; 
	}
	std::vector<std::vector<std::unique_ptr<Dirt>>>* getDirt()		
	{ 
		return &m_dirt; 
	}

	const int getDiggerHealth() const										
	{
		return m_diggerMan->getHealth(); 
	}
	const int getNumSquirtsLeft() const									
	{
		return m_diggerMan->getNumSquirts(); 
	}
	const int getTicks() const											
	{
		return m_ticks; 
	}
	const int getBoulder() const
	{
		return m_boulderCount;
	}
	const int getGold() const
	{
		return m_goldCount;
	}
	const int getOil() const
	{
		return m_oilCount;
	}
	void decOil()													
	{ 
		m_oilCount--;
	}
	void setMaxProt();
	void initNumBould();
	void initNumGold();
	void initNumOil();
	void initProbGoodie();
	void initTicks();
	double distance(int x1, int x2, int y1, int y2);

private:
	void fillGoodies(int, int, ActorType);
	void fillDirt();
	void initProtTicks();
	void incTicks();
	void initMaxProt();
	void initProbHard();	
	
	std::vector<std::shared_ptr<Actor>>				m_actors;
	std::vector<std::shared_ptr<Protester>>	    m_protesters;
	std::shared_ptr<DiggerMan>						m_diggerMan;
	std::vector<std::vector<std::unique_ptr<Dirt>>>	m_dirt;
	
	int m_oilCount;
	int m_boulderCount;
	int m_goldCount;
	int	m_ticks;
	int m_maxProt;
	int	m_protTicks;
	int	m_probHard;
	int m_protesterCount;
	int m_probGoodie;
};


#endif // STUDENTWORLD_H_
