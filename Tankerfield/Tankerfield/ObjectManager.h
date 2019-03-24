#ifndef __OBJECTMANAGER_H__
#define __OBJECTMANAGER_H__

#include <list>

#include "Animation.h"

#include "j1Module.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"


enum ObjectType
{
	PLAYER,

	NO_TYPE
};

class Object;
class Player;
struct SDL_Texture;

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
	void Draw(float dt);
	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;

	bool CleanUp();

	Object* CreateObject(ObjectType type, int x = 0, int y = 0);
	void DeleteEntities();

	Player* GetPlayerData() const;

private:
	void LoadObjectTextures();

private:
	std::list<Object*> objects;

	//Object textures
	SDL_Texture * tank_base = nullptr;
	SDL_Texture * tank_turr = nullptr;

};

#endif

