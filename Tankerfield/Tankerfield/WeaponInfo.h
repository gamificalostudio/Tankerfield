#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

//Class which ONLY holds information about the weapons.
//It doesn't have any functionality more than loading the properties

enum class WEAPON {
	NO_TYPE = -1,
	BASIC,
	DOUBLE_MISSILE,
	HEALING_SHOT,
	FLAMETHROWER,
	LASER_SHOT,
	MAX_WEAPONS,// must be in the last position of the weapons that work. Needed from the creation of pickUps


};

class WeaponInfo
{
public:

	WEAPON type						= WEAPON::FLAMETHROWER;
	int level_weapon				= 0;
	int    bullet_damage			= 0;
	float  bullet_speed				= 0.f;
	float  bullet_life_ms			= 0.f;
	float  time_between_bullets		= 0.f;
	int		bullet_healing			= 0;

	//Screen shake
	float basic_shot_trauma			= 0.f;//The amount of trauma that it will create when you press use the basic shot, related to the screen shake
	float charged_shot_trauma		= 0.f;//The amount of trauma that it will create when you press use the charged shot, related to the screen shake

	//Controller rumble
	float basic_rumble_strength		= 0.f;
	Uint32 basic_rumble_duration	= 0u;

	float charged_rumble_strength	= 0.f;
	Uint32 charged_rumble_duration	= 0u;
};

#endif