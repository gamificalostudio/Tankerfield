#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

//Class which ONLY holds information about the weapons.
//It doesn't have any functionality more than loading the properties

enum class WEAPON {
	BASIC,
	DOUBLE_MISSILE,
	MAX_WEAPONS,// must be in the last position of the weapons that work. Needed from the creation of pickUps
	FLAMETHROWER,
};

class WeaponInfo
{
public:
	void LoadProperties(const pugi::xml_node & node);

public:

	WEAPON type					= WEAPON::BASIC;
	int level_weapon			= 0;
	int    bullet_damage        = 0;
	float  bullet_speed			= 0.f;
	float  bullet_life_ms		= 0.f;
	float  time_between_bullets	= 0.f;
};

#endif