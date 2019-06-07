#ifndef __BALANCE_H__
#define __BALANCE_H__

#include "Defs.h"

//Structs created so that we don't have to go through all the nodes in balance.xml
//every time we need to get a value (for example, when we create a new enemy).

struct TeslaTrooperInfo
{
	float life_multiplier			= 0.f;
	float life_exponential_base		= 0.f;
	float speed						= 0.f;
	int attack_damage				= 0;
	float attack_range				= 0.f;
	uint attack_frequency			= 0;
	int teleport_max_enemies		= 0;
	float detection_range = 0.f;
};

struct BruteInfo
{
	float speed						= 0.f;
	int attack_damage				= 0;
	float attack_range				= 0.f;
	float attack_frequency			= 0.f;
	float life_multiplier			= 0.f;
	float life_exponential_base		= 0.f;
	float detection_range			= 0.f;
};

struct RocketLauncherInfo
{
	float speed						= 0.f;
	int attack_damage				= 0;
	float attack_range				= 0.f;
	uint attack_frequency			= 0;
	float life_multiplier			= 0.f;
	float life_exponential_base		= 0.f;
	float detection_range			= 0.f;
};

struct SuicidalInfo
{
	float speed						= 0.f;
	int attack_damage				= 0;
	float attack_range				= 0.f;
	uint attack_frequency			= 0;
	float life_multiplier			= 0.f;
	float life_exponential_base		= 0.f;
	float detection_range			= 0.f;
};



class WeaponXMLInfo
{
public:
	float damage_multiplier			= 0.f;
	float damage_exponential_base	= 0.f;
};

class BasicWeaponInfo : public WeaponXMLInfo
{
public:
	float speed = 0.f;
};

class RocketLauncherWeaponInfo : public WeaponXMLInfo
{
public:
	
};

class DoubleMissileInfo : public WeaponXMLInfo
{
public:
	float speed = 0.f;
};

class HealingShotInfo : public WeaponXMLInfo
{
public:

};

class LaserInfo : public WeaponXMLInfo
{
public:

};

class ElectroShotInfo : public WeaponXMLInfo
{
public:

};

class FlameThrowerInfo : public WeaponXMLInfo
{
public:

};

class OilWeaponInfo : public WeaponXMLInfo
{
public:
	float speed = 0.f;
};

#endif // !