#ifndef __WEAPON_FLAMETHROWER_H__
#define __WEAPON_FLAMETHROWER_H__

#include "Weapon.h"

class Weapon_Flamethrower : public Weapon
{
public:
	Weapon_Flamethrower();

public:
	void Shoot() override;
};

#endif
