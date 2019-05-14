#ifndef __OBJ_TANK_MAIN_MENU_H__
#define __OBJ_TANK_MAIN_MENU_H__

#include "Object.h"
#include "WeaponInfo.h"
#include "M_Input.h"
#include "Obj_Item.h"
#include "Timer.h"

struct Controller;
struct SDL_Texture;


class Obj_Tank_MainMenu : public Object
{
public:
	Obj_Tank_MainMenu(fPoint pos);
	~Obj_Tank_MainMenu();

public:

	bool Start() override;
	bool Update(float dt) override;

	bool Draw(float dt, Camera * camera) override;
	bool DrawShadow(Camera * camera, float dt) override;

	bool CleanUp() override;

public:

	void SetController(Controller ** controller);

private:
	//- Movement
	void Rotate(float dt);
	void InputController(fPoint & input);

private:

	float tank_scale = 4.f;
	float cos_45 = 0.f;
	float sin_45 = 0.f;
	float base_angle_lerp_factor = 0.f;
	float target_angle = 0.f;

	//- Shooting
	fPoint turr_pos = { 0.f, 0.f };//The position of the turret in the map
	float turr_angle = 0.f;
	float turr_angle_lerp_factor = 0.f;
	float turr_target_angle = 0.f;

	//- Input
	Controller ** controller = nullptr;

	//-- Controller inputs
	int dead_zone = 0;
	Joystick gamepad_rotate;

	//- Drawing
	//-- Base
	Animation rotate_base;
	SDL_Texture * base_tex_blue = nullptr;
	SDL_Texture * base_shadow_tex = nullptr;

	//-- Turret
	Animation rotate_turr;
	SDL_Texture * turr_tex = nullptr;
	SDL_Texture * turr_tex_blue = nullptr;
	SDL_Texture * turr_shadow_tex = nullptr;

public:

};

#endif
