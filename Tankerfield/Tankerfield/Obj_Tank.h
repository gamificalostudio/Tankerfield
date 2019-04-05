#ifndef __OBJ_TANK_H__
#define __OBJ_TANK_H__

#include "Object.h"
#include "Weapon.h"
#include <map>
#include "M_Input.h"

struct Controller;
struct SDL_Texture;

enum class WEAPON_TYPE {
	BASIC,
	FLAMETHROWER,
	MAX
};

enum class INPUT_METHOD {
	KEYBOARD_MOUSE,
	CONTROLLER
};


class Obj_Tank : public Object
{
public:
	Obj_Tank();
	Obj_Tank(fPoint pos);
	~Obj_Tank();

public:

	bool Awake(pugi::xml_node & tank_node) override;
	bool Start() override;
	bool PreUpdate() override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;

	void OnTrigger(Collider* c1);

private:
	void Movement(float dt);
	void InputMovementKeyboard(fPoint & input,float dt);
	void InputMovementController(fPoint & input);

	void Shoot();
	void InputShotMouse(fPoint & input_dir, fPoint & iso_dir);
	void InputShotController(fPoint & input, fPoint & iso_dir);
	bool IsShooting();

	void SelectInputMethod();

public:
	WEAPON_TYPE weapon_type = WEAPON_TYPE::BASIC;

private:

	static SDL_Texture * base_tex;
	static SDL_Texture * turr_tex;
	static SDL_Texture * base_shadow_tex;
	static SDL_Texture * turr_shadow_tex;

	float base_angle = 0.f;
	float base_angle_lerp_factor = 0.f;
	float turr_angle = 0.f;
	fPoint shot_dir = { 0.f, 0.f };
	static SDL_Rect * base_rects;
	static SDL_Rect * turr_rects;
	static int rects_num;

	float speed = 0.f;
	Controller ** controller = nullptr;

	float cos_45 = 0.f;
	float sin_45 = 0.f;

	iPoint draw_offset = { 0, 0 };//Pixels to the center of the player. Used to center the player sprite.

	PerfTimer time_between_bullets_timer;

	std::map<WEAPON_TYPE, Weapon*> weapons;

	INPUT_METHOD last_input					= INPUT_METHOD::KEYBOARD_MOUSE;//Starts as keyboard and switch to last pressed input
	//- Keyboard inputs
	int kb_shoot							= 0;
	SDL_Scancode kb_up						= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_left					= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_down					= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_right					= SDL_SCANCODE_UNKNOWN;
	//- Controller inputs
	Joystick gamepad_move					= Joystick::INVALID;
	Joystick gamepad_aim					= Joystick::INVALID;
	SDL_GameControllerAxis gamepad_shoot	= SDL_CONTROLLER_AXIS_INVALID;
};

#endif