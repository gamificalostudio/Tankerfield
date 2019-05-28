#include "M_Debug.h"
#include "App.h"
#include "M_SceneManager.h"
#include "M_Input.h"
#include "M_ObjManager.h"
#include "Obj_Tank.h"
#include "M_Scene.h"

bool M_Debug::PreUpdate()
{

	//SWITCH BETWEEN MAP AND TEST MAP
	if(app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		if (app->scene->current_level == 0)
		{
			app->scene->current_level = 1;
			app->scmanager->FadeToBlack(app->scene, app->scene, 2.f, 2.f);
		}
		else
		{
			app->scene->current_level = 0;
			app->scmanager->FadeToBlack(app->scene, app->scene, 2.f, 2.f);
		}
	}

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

	//Attack with only one tank
	return true;
}