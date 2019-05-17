#ifndef __BALANCE_H__
#define __BALANCE_H__

#include "Defs.h"

//Structs created so that we don't have to go through all the nodes in balance.xml
//every time we need to get a value (for example, when we create a new enemy).

struct TankInfo
{

};

struct TeslaTrooperInfo
{
	float life_multiplier			= 0.f;
	float life_exponential_base		= 0.f;
	float speed						= 0.f;
	int attack_damage				= 0;
	float attack_range				= 0.f;
	uint attack_frequency			= 0;
	int teleport_max_enemies		= 0;
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
};

struct SuicidalInfo
{
	float speed						= 0.f;
	int attack_damage				= 0;
	float attack_range				= 0.f;
	uint attack_frequency			= 0;
	float life_multiplier			= 0.f;
	float life_exponential_base		= 0.f;
};

class WeaponXMLInfo
{
public:
	float damage_multiplier			= 0.f;
	float damage_exponential_base	= 0.f;
};

class BasicWeaponInfo : public WeaponXMLInfo
{

};

class DoubleMissileInfo : public WeaponXMLInfo
{

};

class HealingShotInfo : public WeaponXMLInfo
{

};

class LaserInfo : public WeaponXMLInfo
{

};

class ElectroShotInfo : public WeaponXMLInfo
{

};

class FlameThrowerInfo : public WeaponXMLInfo
{

};

class OilWeaponInfo : public WeaponXMLInfo
{

};

#endif // !