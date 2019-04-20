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
	HAPPY_HOUR_ITEM,
	MAX_ITEMS, //must be in the last position of the types of items.Needed from the creation of pickUps.

	//OTHER
	TANK,
	STATIC,
	REWARD_ZONE,
	REWARD_BOX,
	PICK_UP,

	//ENEMIES
	TESLA_TROOPER,
	BRUTE,

	//BULLETS
	BASIC_BULLET,
	BULLET_MISSILE,
	HEALING_BULLET,
	BULLET_LASER,
	EXPLOSION,

	//PARTICLES
	HEALING_ANIMATION,

	NO_TYPE
};

class Object;
class Camera;
class Obj_Tank;

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

	Obj_Tank* GetNearestTank(fPoint pos, float max_distanace = 0);

	
	std::list<Object*> GetObjects() const;

	inline void DrawDebug(const Object* obj, Camera* camera);

private:

	std::list<Object*> objects;
	std::list<Object*> obj_tanks;
};

#endif

