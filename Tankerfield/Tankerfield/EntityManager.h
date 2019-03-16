#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "j1Module.h"
#include "Animation.h"
#include <list>

enum EntityType
{
	PLAYER,

	NO_TYPE
};

class Entity;
class Player;

class EntityManager : public j1Module
{
public:

	EntityManager();
	~EntityManager();

	bool Awake(pugi::xml_node& config);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();
	void Draw(float dt);
	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;

	bool CleanUp();

	Entity* CreateEntity(EntityType type, int x = 0, int y = 0);
	void DeleteEntities();

	Player* GetPlayerData() const;


private:

	std::list<Entity*> entities;

};

#endif

