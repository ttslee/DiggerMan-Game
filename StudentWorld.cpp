#include "StudentWorld.h"
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

int StudentWorld::init()
{
	initTicks();
	initNumBould();
	initNumGold();
	initNumOil();
	initProbGoodie();
	m_protesterCount = 0;
	int randX = 0, randY = 0;
	initProtTicks();
	initMaxProt();
	initProbHard();
	m_diggerMan = std::make_shared<DiggerMan>(this);
	fillDirt();
	for (int i = 0; i < 3; i++)
	{	// Fills StudentWorld with Goodies
		switch (i)
		{
		case 0:
			fillGoodies(randX, randY, boulder);
			break;
		case 1:
			fillGoodies(randX, randY, gold);
			break;
		case 2:
			fillGoodies(randX, randY, oil);
		}
	}
	m_protesters.emplace_back(make_shared<RegularProtester>(this));
	//TESTING DISTANCES BETWEEN GOODIES
	//for (size_t i = 0; i < m_actors.size(); i++)
	//{// Nested for loop checking all actors against eachother
	//	for (size_t j = 0; j < m_actors.size(); j++) 
	//	{
	//		if (i == j)
	//			continue;
	//		cout << distance(m_actors[i]->getX(), m_actors[j]->getX(), m_actors[i]->getY(), m_actors[j]->getY()) << endl; // Outputs distances to console. 
	//		assert(distance(m_actors[i]->getX(), m_actors[j]->getX(), m_actors[i]->getY(), m_actors[j]->getY() > 6));
	//	}
	//}
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{

	if (!m_diggerMan->isDead())
	{
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		incTicks();
		setGameStatText(formatGameStats());
		if (m_oilCount == 0)
		{
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}
		/*else if (m_ticks % m_protTicks == 0 && m_protesterCount < m_maxProt)
		{
		int regOrhard = rand() % 100;
		if (regOrhard < m_probHard)
		m_protesters.emplace_back(make_shared<HardcoreProtester>(this));
		else
		m_protesters.emplace_back(make_shared<RegularProtester>(this));
		m_protesterCount++;
		}*/
		else if (rand() % m_probGoodie == 1)
		{
			if ((rand() % 5 + 1) < 4)
			{
				m_actors.emplace_back(make_shared<Water>(this, 4, 60));
			}
			else
				m_actors.emplace_back(make_shared<Sonar>(this, 0, 60));
		}
		m_diggerMan->doSomething();
		for (auto & a : m_actors)
		{
			if (!a->isDead())
				a->doSomething();
		}
		for (auto & p : m_protesters)
		{
			if (!p->isDead())
				p->doSomething();
		}
		getDiggerMan()->setSonarFlag(false);
		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(t2 - t1).count();

		std::cout << duration;
		return GWSTATUS_CONTINUE_GAME;
	}
	else
	{
		decLives();
		if (getLives() == 0)
			playSound(SOUND_PLAYER_DIE);
		else
			playSound(SOUND_PLAYER_GIVE_UP);
		return GWSTATUS_PLAYER_DIED;
	}

}

void StudentWorld::cleanUp()
{
	m_actors.clear();
	m_protesters.clear();
	for (auto &a : m_dirt)
	{
		a.clear();
	}
	m_dirt.clear();
}

std::string StudentWorld::formatGameStats() const
{
	ostringstream os;

	os << left << "Lvl: " << setw(2) << getLevel() << " Hlth: " << setw(3) << m_diggerMan->getHealth() << "% Wtr: "
		<< setw(2) << (m_diggerMan->getNumSquirts() * 10) << " Gld: " << setw(2) << m_diggerMan->getGold()
		<< " Sonar: " << setw(2) << m_diggerMan->getSonarCount() << " Oil Left: " << setw(2) << getOil()
		<< " Scr: " << setfill('0') << setw(6) << getScore();

	return os.str();
}
void StudentWorld::addActor(std::shared_ptr<Actor> obj)
{
	m_actors.emplace_back(obj);
}
void StudentWorld::fillGoodies(int randX, int randY, ActorType gdy)
{
	switch (gdy)
	{
	case boulder:
	{
		for (int j = 0; j < m_boulderCount; j++)
		{
			if (j == 0)
			{
				do
				{
					randX = rand() % 60;
					randY = ((rand() % 37) + 20);
				} while (randX >= 27 && randX <= 33 && randY > 0);
				m_actors.emplace_back(std::make_shared<Boulder>(this, randX, randY));
			}
			else
			{
				do
				{
					randX = rand() % 60;
					randY = ((rand() % 37) + 20);
					if ((randX >= 27 && randX <= 33) && randY > 0)
						continue;
					size_t b = 0;
					do
					{
						if (distance(randX, m_actors[b]->getX(), randY, m_actors[b]->getY()) > 6)
						{
							b++;
							continue;
						}
						break;
					} while (b < m_actors.size());
					if (b == m_actors.size())
					{
						m_actors.emplace_back(std::make_shared<Boulder>(this, randX, randY));
						break;
					}
				} while (true);
			}
		}
		for (auto e : m_actors)
			std::cout << "BOULDER X: " << e->getX() << "  BOULDER Y: " << e->getY() << endl;
		break;
	}
	case gold:
	case oil:
	{
		int count = 0;
		if (gdy == gold)
			count = m_goldCount;
		else
			count = m_oilCount;
		for (int j = 0; j < count; j++)
		{
			do
			{
				randX = rand() % 60;
				randY = rand() % 57;
				if ((randX >= 27 && randX <= 33) && randY > 0)
					continue;
				size_t b = 0;
				do
				{
					if (distance(randX, m_actors[b]->getX(), randY, m_actors[b]->getY()) > 6)
					{
						b++;
						continue;
					}
					break;
				} while (b < m_actors.size());
				if (b == m_actors.size()) {
					if (gdy == gold)
						m_actors.emplace_back(std::make_shared<Nugget>(this, randX, randY));
					else
						m_actors.emplace_back(std::make_shared<Oil>(this, randX, randY));
					break;
				}
			} while (true);
		}
		break;
	}
	}
}
void StudentWorld::fillDirt() {
	for (int i = 0; i < 60; i++)
	{
		m_dirt.push_back(std::vector<std::unique_ptr<Dirt>>());
		for (int j = 0; j < 64; j++)
		{
			if ((i >= 4) && (j >= 30 && j <= 33))
			{
				m_dirt[i].emplace_back(std::make_unique<Dirt>(this, j, i, false, true));
				continue;
			}
			m_dirt[i].emplace_back(std::make_unique<Dirt>(this, j, i));
		}
	}
}
void StudentWorld::getShortestPathOut(int x, int y)
{

}
void StudentWorld::initProtTicks()
{
	m_protTicks = max(25, (200 - static_cast<int>(getLevel())));
}
void StudentWorld::initMaxProt()
{
	m_maxProt = min(15, static_cast<int>(2 + getLevel() * 1.5));
}
void StudentWorld::initProbHard()
{
	m_probHard = min(90, static_cast<int>(getLevel()) * 10 + 30);
}

void StudentWorld::initNumBould()
{
	m_boulderCount = min(static_cast<int>(getLevel()) / 2 + 2, 7);
}
void StudentWorld::initNumGold()
{
	m_goldCount = max(5 - static_cast<int>(getLevel()) / 2, 2);
}
void StudentWorld::initNumOil()
{
	m_oilCount = min(2 + static_cast<int>(getLevel()), 18);
}
void StudentWorld::initProbGoodie()
{
	m_probGoodie = getLevel() * 25 + 300;
}
void StudentWorld::incTicks()
{
	m_ticks++;
}
void StudentWorld::initTicks()
{
	m_ticks = 0;
}
double StudentWorld::distance(int x1, int x2, int y1, int y2)
{
	return sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
