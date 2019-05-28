#include "M_Debug.h"
#include "App.h"
#include "M_SceneManager.h"
#include "M_Input.h"
#include "M_ObjManager.h"
#include "Obj_Tank.h"
#include "M_Scene.h"
#include "Log.h"
#include "M_Render.h"
#include "M_Map.h"
#include "Obj_Enemy.h"
#include "Object.h"
#include <assert.h>

bool M_Debug::Start()
{
	debug_numeric[(int)DebugNumericType::SELECT_TANK].SetValues(
		DebugNumericType::SELECT_TANK,
		SDL_SCANCODE_Z,
		0,
		4);

	debug_numeric[(int)DebugNumericType::SELECT_WEAPON].SetValues(
		DebugNumericType::SELECT_WEAPON,
		SDL_SCANCODE_F2,
		0,
		(int)WEAPON::MAX_WEAPONS);

	debug_numeric[(int)DebugNumericType::SELECT_WEAPON_LEVEL].SetValues(
		DebugNumericType::SELECT_WEAPON_LEVEL,
		SDL_SCANCODE_X,
		1,
		INT_MAX);

	debug_numeric[(int)DebugNumericType::SELECT_OBJECT].SetValues(
		DebugNumericType::SELECT_OBJECT,
		SDL_SCANCODE_C,
		0,
		(int)ObjectType::MAX);

	debug_numeric[(int)DebugNumericType::SELECT_ENEMY_LEVEL].SetValues(
		DebugNumericType::SELECT_ENEMY_LEVEL,
		SDL_SCANCODE_V,
		1,
		INT_MAX);

	return true;
}

void M_Debug::ManageNumericDebug(fPoint mouse_pos)
{
	//Checks which numeric debug has been the last one pressed
	//and puts it in curr_debug_num
	for (int debug_num_iter = 0; debug_num_iter < (int)DebugNumericType::MAX; ++debug_num_iter)
	{
		debug_numeric[debug_num_iter].PressedKey();
	}

	if (curr_debug_num == DebugNumericType::MAX)
	{
		return;
	}

	//Update its number based on which keys have been pressed
	for (int keyboard_number = (int)SDL_SCANCODE_1; keyboard_number <= (int)SDL_SCANCODE_0; ++keyboard_number)
	{
		if (app->input->GetKey(keyboard_number) == KEY_DOWN)
		{
			debug_numeric[(int)curr_debug_num].UpdateNumber(GetNumberFromScancode(keyboard_number));
		}
	}

	//Print according label

	//Do whatever you need to do when it is released
	if (debug_numeric[(int)curr_debug_num].ReleasedKey())
	{
		switch (debug_numeric[(int)curr_debug_num].type)
		{
		case DebugNumericType::SELECT_TANK: {
			//Empty intentionally
		}	break;

		case DebugNumericType::SELECT_WEAPON: {
			int selected_tank = debug_numeric[(int)DebugNumericType::SELECT_TANK].num;
			app->objectmanager->obj_tanks[selected_tank]->SetWeapon(
				(WEAPON)debug_numeric[(int)DebugNumericType::SELECT_WEAPON].num,
				app->objectmanager->obj_tanks[selected_tank]->GetWeaponInfo().level_weapon);
		}	break;

		case DebugNumericType::SELECT_WEAPON_LEVEL: {
			int selected_tank = debug_numeric[(int)DebugNumericType::SELECT_TANK].num;
			app->objectmanager->obj_tanks[selected_tank]->SetWeapon(
				app->objectmanager->obj_tanks[selected_tank]->GetWeaponInfo().weapon,
				debug_numeric[(int)DebugNumericType::SELECT_WEAPON_LEVEL].num);
		}	break;

		case DebugNumericType::SELECT_ENEMY_LEVEL: {
			//Empty intentionally
		}	break;

		case DebugNumericType::SELECT_OBJECT: {
			ObjectType obj_type = (ObjectType)debug_numeric[(int)DebugNumericType::SELECT_OBJECT].num;
			Object * obj = app->objectmanager->CreateObject(
				obj_type,
				mouse_pos);
			if (app->objectmanager->IsEnemy(obj_type))
			{
				Obj_Enemy * enemy = (Obj_Enemy*)obj;
				enemy->SetStats(debug_numeric[(int)DebugNumericType::SELECT_ENEMY_LEVEL].num);
			}
		}	break;

		default: {
			LOG("Debug type not set correctly.");
		}	break;

		}
	}
}


bool M_Debug::PreUpdate()
{
	iPoint mouse_pos;
	app->input->GetMousePosition(mouse_pos.x, mouse_pos.y);
	mouse_pos = app->render->ScreenToWorld(mouse_pos.x, mouse_pos.y, (*app->render->cameras.begin()));
	mouse_pos = app->map->ScreenToMapI(mouse_pos.x, mouse_pos.y);

	ManageNumericDebug((fPoint)mouse_pos);
	ChangeMap();
	//Reset scene
	if (app->input->GetKey(SDL_SCANCODE_F5) == KeyState::KEY_DOWN)
	{
		app->scmanager->FadeToBlack(this, this, 2.f, 2.f);
	}

	//GOD MODE
	//+ Add label that says "God Mode"

	//Debug Window with any variable you want to put in (you can add a parameter and it will be printed there with the string and the number you pass it)

	//Attack with only one tank


	//if (app->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	//{
	//	app->pick_manager->CreatePickUp((fPoint)mouse_pos);
	//}
	//if (app->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
	//{
	//	app->objectmanager->obj_tanks[0]->SetLife(0);
	//	app->objectmanager->obj_tanks[1]->SetLife(0);
	//	app->objectmanager->obj_tanks[2]->SetLife(0);
	//	app->objectmanager->obj_tanks[3]->SetLife(0);
	//}
	//if (app->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
	//{
	//	iPoint mouse_pos;
	//	app->input->GetMousePosition(mouse_pos.x, mouse_pos.y);
	//	mouse_pos += iPoint(app->render->cameras[0]->rect.x, app->render->cameras[0]->rect.y);
	//	fPoint map_mouse_pos = app->map->ScreenToMapF(mouse_pos.x, mouse_pos.y);
	//	app->objectmanager->CreateObject(ObjectType::EXPLOSION, map_mouse_pos);
	//}
	//if (app->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)
	//{
	//	++this->round;
	//	general_gui->SetRoundNumber(round);
	//}
	//if (app->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
	//{
	//	game_state = GAME_STATE::GAME_WON;
	//}

	//if (app->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
	//{
	//	app->objectmanager->delete_all_enemies = true;
	//}

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

//Debug numeric

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

void DebugNumeric::SetValues(DebugNumericType type, SDL_Scancode key, int min_num, int max_num)
{
	this->type = type;
	type_num = (int)type;
	this->key = key;
	assert(min_num < max_num);
	this->num = this->min_num = min_num;
	this->max_num = max_num;
}

void DebugNumeric::PressedKey()
{
	if (app->input->GetKey(key) == KEY_DOWN)
	{
		app->debug->curr_debug_num = type;
	}
}

void DebugNumeric::UpdateNumber(int new_digit)
{
	if (!pressed_numbers)
	{
		//Clear the previous number
		num = 0;
		pressed_numbers = true;
	}
	num = num * 10 + new_digit;
	num = MIN(num, max_num);
	num = MAX(num, min_num);
}

bool DebugNumeric::ReleasedKey()
{
	bool ret = false;
	if (app->input->GetKey(key) == KEY_UP)
	{
		pressed_numbers = false;
		app->debug->curr_debug_num = DebugNumericType::MAX;
		//TODO: Hide the label
		ret = true;
	}
	return ret;
}
