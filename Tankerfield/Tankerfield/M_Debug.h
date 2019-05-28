#ifndef __M_DEBUG_H__
#define __M_DEBUG_H__

#include "Module.h"
#include "SDL/include/SDL_scancode.h"
#include "Point.h"

enum class DebugNumericType
{
	SELECT_TANK,
	SELECT_WEAPON,
	SELECT_WEAPON_LEVEL,
	SELECT_ENEMY_LEVEL,
	SELECT_OBJECT,
	MAX
};

struct DebugNumeric
{
	DebugNumericType type = DebugNumericType::MAX;
	int type_num = 0;
	SDL_Scancode key = SDL_SCANCODE_UNKNOWN;
	int num = 0;
	int min_num = 0;
	int max_num = 0;
	bool pressed_numbers = false;//Indicates if it has pressed any number since the SDL_Scancode key has been presssed down
	//If you press the debug key without pressing any numbers, it will perform the action with the last number you've inserted

	void SetValues(DebugNumericType type, SDL_Scancode key, int min_num, int max_num);

	void PressedKey();//Checks if the debug element has been pressed
	void UpdateNumber(int new_digit);
	bool ReleasedKey();//Returns true if the element has been used
};

class M_Debug : public Module
{
public:
	bool Start() override;
	bool PreUpdate() override;

	void ManageNumericDebug(fPoint mouse_pos);

private:
	int GetNumberFromScancode(int num);

	//Debug functionalities
	void ChangeMap();

private:
	DebugNumericType curr_debug_num = DebugNumericType::MAX;
	DebugNumeric debug_numeric[(int)DebugNumericType::MAX];
	
	friend struct DebugNumeric;
};

#endif