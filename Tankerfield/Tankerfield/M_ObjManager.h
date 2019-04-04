#ifndef __M_OBJMANAGER_H__
#define __M_OBJMANAGER_H__

#include <list>

#include "Animation.h"

#include "Module.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"
#include "Point.h"

enum ObjectType
{
	TANK,
	PLAYER,
	TESLA_TROOPER,
	BASIC_BULLET,
	REWARD_ZONE,
	NO_TYPE
};

class Object;

class M_ObjManager : public Module
{
public:

	M_ObjManager();
	~M_ObjManager();

	bool Awake(pugi::xml_node& config) override;

	bool Start() override;

	bool PreUpdate() override;

	bool Update(float dt) override;

	bool PostUpdate() override;

	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;

	bool CleanUp() override;

	Object* CreateObject(ObjectType type, fPoint pos);

private:

	void DeleteObject(Object * obj);

	void DeleteObjects();

private:

	std::list<Object*> objects;

};

#endif

