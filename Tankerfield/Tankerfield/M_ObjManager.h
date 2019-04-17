#ifndef __M_OBJMANAGER_H__
#define __M_OBJMANAGER_H__

#include <list>

#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

#include "Animation.h"

#include "Module.h"
#include "Point.h"

enum class ObjectType
{
	//ITEMS need to be first because of the random selection from Obj_PickUp .
	HEALTH_BAG = 0,
	MAX_ITEMS, //must be in the last position of the types of items.Needed from the creation of pickUps.

	//OTHER
	TANK,
	STATIC,
	REWARD_ZONE,
	REWARD_BOX,
	PICK_UP,

	//ENEMIES
	TESLA_TROOPER,

	//BULLETS
	BASIC_BULLET,
	BULLET_MISSILE,
	EXPLOSION,

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

	Object* CreateObject(ObjectType type, fPoint map_pos);

	static bool SortByYPos(Object * obj1, Object * obj2);

	void DeleteObjects();

	Object* GetNearestTank(fPoint pos);
	
	std::list<Object*> GetObjects() const;

	inline void DrawDebug(const Object* obj);

private:

	std::list<Object*> objects;
	std::list<Object*> obj_tanks;
};

#endif

