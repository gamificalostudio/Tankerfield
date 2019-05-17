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
	//OTHER
	TANK,
	STATIC,
	REWARD_ZONE,
	REWARD_BOX,
	PICK_UP,
	PORTAL,
	TANK_MAIN_MENU,

	//ENEMIES
	TESLA_TROOPER,
	BRUTE,
	SUICIDAL,
	ROCKETLAUNCHER,

	//BULLETS
	BASIC_BULLET,
	BULLET_MISSILE,
	HEALING_BULLET,
	BULLET_LASER,
	EXPLOSION,
	CANNON_FIRE,

	//PARTICLES
	HEALING_ANIMATION,
	FIRE_DEAD,
	ELECTRO_SHOT_ANIMATION,

	NO_TYPE
};

enum class ItemType
{
	NO_TYPE = -1,
	HEALTH_BAG ,
	HAPPY_HOUR_ITEM,
	INSTANT_HELP,
	MAX_ITEMS, //must be in the last position of the types of items.Needed from the creation of pickUps.
};

class Object;
class Obj_Item;
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

	bool Reset();

	Object* CreateObject(ObjectType type, fPoint map_pos);
	Obj_Item* CreateItem(ItemType type, fPoint map_pos);

	static bool SortByYPos(Object * obj1, Object * obj2);

	void DeleteObjects();

	Obj_Tank* GetNearestTank(fPoint pos, float max_distanace = FLT_MAX);

	std::list<Object*> GetObjects() const;

	inline void DrawDebug(const Object* obj, Camera* camera);

	uint GetNumberOfEnemies()
	{
		return enemies.size();
	 }

public:
	pugi::xml_node balance_xml_node;
	std::vector<Obj_Tank*> obj_tanks;

private:
	pugi::xml_document balance_xml_doc;
	std::list<Object*> objects;
	std::list<Object*> enemies;
};

#endif

