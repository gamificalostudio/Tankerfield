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

bool M_Debug::PreUpdate()
{
	iPoint mouse_pos;
	app->input->GetMousePosition(mouse_pos.x, mouse_pos.y);
	mouse_pos = app->render->ScreenToWorld(mouse_pos.x, mouse_pos.y, (*app->render->cameras.begin()));
	mouse_pos = app->map->ScreenToMapI(mouse_pos.x, mouse_pos.y);

	SelectElement(SDL_SCANCODE_Z, DebugElement::SELECT_TANK, 4);

	if (SelectElement(SDL_SCANCODE_F2, DebugElement::SELECT_WEAPON, (int)WEAPON::MAX_WEAPONS))
	{
		app->objectmanager->obj_tanks[selected_tank]->SetWeapon(
			(WEAPON)debug_num[(int)DebugElement::SELECT_WEAPON],
			app->objectmanager->obj_tanks[selected_tank]->GetWeaponInfo().level_weapon);
	}

	if (SelectElement(SDL_SCANCODE_X, DebugElement::SELECT_WEAPON_LEVEL, INT_MAX))
	{
		app->objectmanager->obj_tanks[selected_tank]->SetWeapon(
			app->objectmanager->obj_tanks[selected_tank]->GetWeaponInfo().weapon,
			debug_num[(int)DebugElement::SELECT_WEAPON_LEVEL]);
	}

	if (SelectElement(SDL_SCANCODE_C, DebugElement::SELECT_OBJECT, (int)ObjectType::MAX))
	{
		app->objectmanager->CreateObject(
			(ObjectType)debug_num[(int)DebugElement::SELECT_OBJECT],
			(fPoint)mouse_pos);
	}

	//Switch between map and test map
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

bool M_Debug::SelectElement(SDL_Scancode key, DebugElement elem, int max_num)
{
	bool ret = false;
	int elem_num = (int)elem;

	if (app->input->GetKey(key) == KEY_DOWN)
	{
		debug_elem = elem;
		debug_num[elem_num] = 0;
	}
	if (app->input->GetKey(key) == KEY_DOWN
		|| app->input->GetKey(key) == KEY_REPEAT
		&& debug_elem == elem)
	{
		SelectDebugNumber(elem_num);
		debug_num[elem_num] = MIN(debug_num[elem_num], max_num);
	}
	if (app->input->GetKey(key) == KEY_UP
		&& debug_elem == elem)
	{
		ret = true;
	}
	return ret;
}

void M_Debug::SelectDebugNumber(int elem_num)
{
	for (int keyboard_number = (int)SDL_SCANCODE_1; keyboard_number <= (int)SDL_SCANCODE_0; ++keyboard_number)
	{
		if (app->input->GetKey(keyboard_number) == KEY_DOWN)
		{
			debug_num[elem_num] = debug_num[elem_num] * 10 + GetNumberFromScancode(keyboard_number);
		}
	}
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
