#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

//Class which holds information about the weapons

class WeaponInfo
{
public:
	void LoadProperties(const pugi::xml_node & node);

public:
	int damage					= 0;
	float bullet_speed			= 0.f;
	float bullet_life_ms		= 0.f;
	float time_between_bullets	= 0.f;
};

#endif