#ifndef __M_DEBUG_H__
#define __M_DEBUG_H__

#include "Module.h"
#include "SDL/include/SDL_scancode.h"

enum class DebugElement
{
	INVALID = -1,
	WEAPON,
	OBJECT
};

class M_Debug : public Module
{
public:
	bool PreUpdate() override;

private:
	void SelectDebugNumber();
	void ClearDebugNumber();
	int GetNumberFromScancode(int num);

	//Debug functionalities
	void ChangeMap();
	bool SelectElement(SDL_Scancode key, DebugElement elem, int max_num);
	//void ChangeWeapon();

private:
	DebugElement debug_elem = DebugElement::INVALID;

	//Pressed number
	int debug_num = 0;

	//Variables to keep
	int selected_tank = 0;
};

#endif