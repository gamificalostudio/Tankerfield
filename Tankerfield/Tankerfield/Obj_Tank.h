#ifndef __OBJ_TANK_H__
#define __OBJ_TANK_H__

#include "Object.h"
#include "Weapon.h"
#include <map>
#include "M_Input.h"

struct Controller;
struct SDL_Texture;

enum class WEAPON {
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
	Obj_Tank(fPoint pos);

public:

	bool Start() override;
	bool PreUpdate() override;
	bool Update(float dt) override;
	bool Draw(float dt) override;
	bool DrawShadow() override;
	bool CleanUp() override;

	void OnTrigger(Collider* c1);

private:
	void Movement(float dt);
	void InputMovementKeyboard(fPoint & input,float dt);
	void InputMovementController(fPoint & input);

	void Shoot();
	void InputShotMouse(const fPoint & shot_pos, fPoint & input_dir, fPoint & iso_dir);
	void InputShotController(const fPoint & shot_pos, fPoint & input, fPoint & iso_dir);
	bool IsShooting();

	void SelectInputMethod();

private:
	//- Static variables (remember to inicialize them in the .cpp)
	static SDL_Texture * base_tex;
	static SDL_Texture * turr_tex;
	static SDL_Texture * base_shadow_tex;
	static SDL_Texture * turr_shadow_tex;
	static Animation * rotate_base;
	static Animation * rotate_turr;
	static WeaponInfo * weapons_info;

	//- Movement
	float speed								= 0.f;
	fPoint velocity							= { 0.f, 0.f };
	float cos_45							= 0.f;//TODO: Create a macro with its value directly
	float sin_45							= 0.f;
	float base_angle_lerp_factor			= 0.f;

	//- Shooting
	float turr_angle						= 0.f;
	fPoint shot_dir							= { 0.f, 0.f };
	float cannon_height						= 0.f;//Used to calculate the shot position
	float cannon_length						= 0.f;//The offset at which the bullet will spawn from the shot position (pos + shot height)
	PerfTimer time_between_bullets_timer;

	//-- Secondary shoot
	WEAPON weapon_type						= WEAPON::BASIC;

	//- Input
	INPUT_METHOD move_input					= INPUT_METHOD::KEYBOARD_MOUSE;//Starts as keyboard and switch to last pressed input
	INPUT_METHOD shot_input					= INPUT_METHOD::KEYBOARD_MOUSE;
	Controller ** controller = nullptr;

	//-- Keyboard inputs
	int kb_shoot							= 0;
	SDL_Scancode kb_up						= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_left					= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_down					= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_right					= SDL_SCANCODE_UNKNOWN;

	//-- Controller inputs
	Joystick gamepad_move					= Joystick::INVALID;
	Joystick gamepad_aim					= Joystick::INVALID;
	SDL_GameControllerAxis gamepad_shoot	= SDL_CONTROLLER_AXIS_INVALID;

};

#endif