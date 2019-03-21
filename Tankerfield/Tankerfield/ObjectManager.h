#ifndef __OBJECTMANAGER_H__
#define __OBJECTMANAGER_H__

#include <list>

#include "Animation.h"

#include "j1Module.h"


enum ObjectType
{
	PLAYER,
	TESLA_TROOPER,

	NO_TYPE
};

class Object;
class Player;

class ObjectManager : public j1Module
{
public:

	ObjectManager();
	~ObjectManager();

	bool Awake(pugi::xml_node& config);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();
	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;

	bool CleanUp();

	Object* CreateObject(ObjectType type, float x = 0.0f, float y = 0.0f);
	void DeleteObjects();

	//Player* GetPlayerData() const;

private:

	std::list<Object*> objects;
	SDL_Texture* texture;
};

#endif

