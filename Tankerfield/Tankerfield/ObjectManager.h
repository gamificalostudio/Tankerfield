#ifndef __OBJECTMANAGER_H__
#define __OBJECTMANAGER_H__

#include <list>

#include "Animation.h"

#include "j1Module.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"


enum ObjectType
{
  TANK,
	PLAYER,
	TESLA_TROOPER,
	NO_TYPE
};

class Object;

class ObjectManager : public j1Module
{
public:

	ObjectManager();
	~ObjectManager();

	bool Awake(pugi::xml_node& config) override;

	bool Start() override;

	bool PreUpdate() override;

	bool Update(float dt) override;

	bool PostUpdate() override;

	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;

	bool CleanUp() override;

	Object* CreateObject(ObjectType type, float x = 0.0f, float y = 0.0f);
	void DeleteObjects();
	bool DeleteObject(Object* object);
  
private:
	std::list<Object*> objects;
};

#endif

