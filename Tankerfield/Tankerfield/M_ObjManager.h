#ifndef __M_OBJMANAGER_H__
#define __M_OBJMANAGER_H__

#include <list>

#include "Animation.h"

#include "Module.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"
#include "Point.h"

enum class ObjectType
{
	TANK,
	TESLA_TROOPER,
	BASIC_BULLET,
	STATIC,
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

	bool PostUpdate(float dt) override;

	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;

	bool CleanUp() override;

	Object* CreateObject(ObjectType type, fPoint pos);

	static bool SortByYPos(Object * ent1, Object * ent2);

	void DeleteObjects();
  
private:

	std::list<Object*> objects;

};

#endif

