#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

enum class WEAPON {
	NO_TYPE = -1,
	BASIC,
	DOUBLE_MISSILE,
	HEALING_SHOT,
	LASER_SHOT,
	MAX_WEAPONS,// must be in the last position of the weapons that work. Needed from the creation of pickUps
	FLAMETHROWER,
};

//Charged weapons have two shots:
//- Normal: when you release the button before the charge time.
//- Charged: when you release the button before the charge time.
//Sustained wepons have two shots:
//- Quick shot: when you release the button quickly.
//- Sustained: when you mantain the button pressed.
enum class WEAPON_TYPE {
	CHARGED,
	SUSTAINED
};

//Class which ONLY holds information about the weapons.
//It doesn't have any functionality more than loading the properties
class WeaponInfo
{
public:
	WEAPON weapon					= WEAPON::BASIC;
	WEAPON_TYPE type				= WEAPON_TYPE::CHARGED;
	int level_weapon				= 0;
	int    bullet_damage			= 0;
	float  bullet_speed				= 0.f;
	float bullet_life_ms			= 0.f;
	float time_between_bullets		= 0.f;
	int bullet_healing				= 0;
	int explosion_damage			= 0;

	//Screen shake
	float basic_shot_trauma			= 0.f;//The amount of trauma that it will create when you press use the basic shot, related to the screen shake
	float charged_shot_trauma		= 0.f;//The amount of trauma that it will create when you press use the charged shot, related to the screen shake

	//Controller rumble
	//Shot 1 (basic shot on charged weapons and quick shot on sustained weapons)
	float shot1_rumble_strength		= 0.f;
	Uint32 shot1_rumble_duration	= 0u;

	//Shot 2 (charged shot on charged weapons and sustained shot on sustained weapons)
	float shot2_rumble_strength	= 0.f;
	Uint32 shot2_rumble_duration	= 0u;
};

#endif