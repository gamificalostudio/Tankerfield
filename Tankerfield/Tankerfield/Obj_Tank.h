#ifndef __OBJ_TANK_H__
#define __OBJ_TANK_H__

#include "Object.h"
#include "Weapon.h"
#include <map>

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
	Obj_Tank(int x, int y);
	~Obj_Tank();

public:
	bool Awake(pugi::xml_node & tank_node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

private:
	void Movement(float dt);
	void InputMovementKeyboard(fPoint & input);
	void InputMovementController(fPoint & input);

	void Shoot();
	void InputShotMouse(fPoint & input);
	void InputShotController(fPoint & input);
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
	float turr_angle = 0.f;
	static SDL_Rect * base_rects;
	static SDL_Rect * turr_rects;
	static int rects_num;

	float speed = 2.5f;
	Controller ** controller = nullptr;

	float cos_45 = 0.f;
	float sin_45 = 0.f;

	PerfTimer time_between_bullets_timer;

	std::map<WEAPON_TYPE, Weapon*> weapons;

	INPUT_METHOD last_input					= INPUT_METHOD::KEYBOARD_MOUSE;
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