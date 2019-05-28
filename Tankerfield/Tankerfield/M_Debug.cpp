#include "M_Debug.h"
#include "App.h"
#include "M_SceneManager.h"
#include "M_Input.h"
#include "M_ObjManager.h"
#include "Obj_Tank.h"
#include "M_Scene.h"
#include "Log.h"

bool M_Debug::PreUpdate()
{
	ChangeMap();
	//ChangeWeapon();
	if (SelectElement(SDL_SCANCODE_F2, DebugElement::WEAPON, (int)WEAPON::MAX_WEAPONS))
	{
		app->objectmanager->obj_tanks[0]->SetWeapon(
			(WEAPON)debug_num,
			app->objectmanager->obj_tanks[0]->GetWeaponInfo().level_weapon);
		ClearDebugNumber();
	}

	//GOD MODE
	//+ Add label that says "God Mode"

	//Debug Window with any variable you want to put in (you can add a parameter and it will be printed there with the string and the number you pass it)

	//focus tank pressing one of the buttons (1,2,3,4)
	//they shoot, etc

	//Change to the selected weapon
	//1. select tank to change the weapon
	//2. change the weapon with a number
	//if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	//{
	//	int new_weapon = (int)app->objectmanager->obj_tanks[0]->GetWeaponInfo().weapon;
	//	++new_weapon;
	//	if (new_weapon == (int)WEAPON::MAX_WEAPONS)
	//	{
	//		new_weapon = (int)WEAPON::DOUBLE_MISSILE;
	//	}
	//	app->objectmanager->obj_tanks[0]->SetWeapon((WEAPON)new_weapon, app->scene->round);
	//}

	//Detect input for numbers

	LOG("number; %i", debug_num);

	//Attack with only one tank
	return true;
}

void M_Debug::ChangeMap()
{
	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		if (app->scene->current_level == 0)
		{
			app->scene->current_level = 1;
			app->scmanager->FadeToBlack(app->scene, app->scene, 0.5f, 0.5f);
		}
		else
		{
			app->scene->current_level = 0;
			app->scmanager->FadeToBlack(app->scene, app->scene, 0.5f, 0.5f);
		}
	}
}

bool M_Debug::SelectElement(SDL_Scancode key, DebugElement elem, int max_num)
{
	bool ret = false;
	if (app->input->GetKey(key) == KEY_DOWN)
	{
		debug_elem = elem;
	}
	if (app->input->GetKey(key) == KEY_DOWN
		|| app->input->GetKey(key) == KEY_REPEAT
		&& debug_elem == elem)
	{
		SelectDebugNumber();
		debug_num = MIN(debug_num, max_num);
	}
	if (app->input->GetKey(key) == KEY_UP
		&& debug_elem == elem)
	{
		ret = true;
	}
	return ret;
}

//void M_Debug::ChangeWeapon()
//{
//	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
//	{
//		debug_elem = DebugElement::WEAPON;
//	}
//	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN
//		|| app->input->GetKey(SDL_SCANCODE_F2) == KEY_REPEAT
//		&& debug_elem == DebugElement::WEAPON)
//	{
//		SelectDebugNumber();
//		debug_num = MIN(debug_num, (int)WEAPON::MAX_WEAPONS);
//	}
//	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_UP
//		&& debug_elem == DebugElement::WEAPON)
//	{
//		app->objectmanager->obj_tanks[0]->SetWeapon(
//			(WEAPON)debug_num,
//			app->objectmanager->obj_tanks[0]->GetWeaponInfo().level_weapon);
//		ClearDebugNumber();
//	}
//}

void M_Debug::SelectDebugNumber()
{
	for (int keyboard_number = (int)SDL_SCANCODE_1; keyboard_number <= (int)SDL_SCANCODE_0; ++keyboard_number)
	{
		if (app->input->GetKey(keyboard_number) == KEY_DOWN)
		{
			debug_num = debug_num * 10 + GetNumberFromScancode(keyboard_number);
		}
	}
}

void M_Debug::ClearDebugNumber()
{
	debug_num = 0;
}

int M_Debug::GetNumberFromScancode(int num)
{
	if (num == (int)SDL_SCANCODE_0)//0 is placed in last position instead of first
	{
		return 0;
	}
	else
	{
		return num - (int)SDL_SCANCODE_1 + 1;
	}
}
