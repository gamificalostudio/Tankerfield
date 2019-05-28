#ifndef __M_DEBUG_H__
#define __M_DEBUG_H__

#include "Module.h"
#include "SDL/include/SDL_scancode.h"

enum class DebugElement
{
	SELECT_TANK,
	SELECT_WEAPON,
	SELECT_WEAPON_LEVEL,
	SELECT_ENEMY_LEVEL,
	SELECT_OBJECT,
	MAX
};

class M_Debug : public Module
{
public:
	bool Start() override;
	bool PreUpdate() override;

private:
	void SelectDebugNumber(int elem_num);
	int GetNumberFromScancode(int num);

	//Debug functionalities
	void ChangeMap();
	bool SelectElement(SDL_Scancode key, DebugElement elem, int max_num);
	//void ChangeWeapon();

private:
	DebugElement debug_elem = DebugElement::MAX;

	//Pressed number
	int debug_num[(int)DebugElement::MAX] = { 0 };

	//Variables to keep
	int selected_tank = 0;
};

#endif