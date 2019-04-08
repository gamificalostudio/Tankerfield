#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "Object.h"
#include "PerfTimer.h"
#include "M_ObjManager.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

class Obj_Bullet;

class WeaponInfo
{
public:
	void LoadProperties(pugi::xml_node & node);

private:
	int damage					= 0;
	float bullet_speed			= 0.f;
	float bullet_life_ms		= 0.f;
	float time_between_bullets	= 0.f;
};

#endif