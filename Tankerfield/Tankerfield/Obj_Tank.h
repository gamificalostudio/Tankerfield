#ifndef __OBJ_TANK_H__
#define __OBJ_TANK_H__

#include "Object.h"
#include "WeaponInfo.h"
#include "M_Input.h"
#include "Obj_Item.h"

struct Controller;
struct SDL_Texture;
class Camera;
class Obj_PickUp;


enum class INPUT_METHOD {
	KEYBOARD_MOUSE,
	CONTROLLER
};


class Obj_Tank : public Object
{
public:
	Obj_Tank(fPoint pos);
	~Obj_Tank();

public:

	bool Start() override;
	bool PreUpdate() override;
	bool Update(float dt) override;

	void CameraMovement(float dt);

	bool Draw(float dt, Camera * camera) override;
	bool DrawShadow(Camera * camera) override;


	bool CleanUp() override;

	void OnTrigger(Collider* c1);

public:
	//- Logic
	void SetLife(int life);
	void SetItem(ObjectType Type);
	void SetWeapon(WEAPON type);
	int GetLife();
	int GetMaxLife();

public:
	//- Pick ups
	void SetPickUp(Obj_PickUp* pick_up);

private:
	//- Movement
	void Movement(float dt);
	void InputMovementKeyboard(fPoint & input,float dt);
	void InputMovementController(fPoint & input);

	//- Shooting
	void Shoot();
	void InputShotMouse(const fPoint & shot_pos, fPoint & input_dir, fPoint & iso_dir);
	void InputShotController(const fPoint & shot_pos, fPoint & input, fPoint & iso_dir);
	bool PressShot();
	bool ReleaseShot();

	//- Input
	void SelectInputMethod();

	//- Weapons methods
	void ShootBasic();
	void ShootFlameThrower();
	void ShootDoubleMissile();
	void ShootHealingShot();

	//- TankDeath
	void ReviveTank();
	void StopTank();

	//- Item
	void Item();

private:
	//- Logic
	int life								= 0;
	int max_life							= 0;
	bool alive								= true;
	int tank_num							= 0;//The number of tank. 0 is the first one.

	static int number_of_tanks;

	//- Movement
	float speed								= 0.f;
	fPoint velocity							= { 0.f, 0.f };
	float cos_45							= 0.f;//TODO: Create a macro with its value directly
	float sin_45							= 0.f;
	float base_angle_lerp_factor			= 0.f;

	//- Shooting
	fPoint turr_pos							= { 0.f, 0.f };//The position of the turret in the map
	float turr_angle						= 0.f;
	fPoint shot_dir							= { 0.f, 0.f };
	float cannon_height						= 0.f;//Used to calculate the shot position
	float cannon_length						= 0.f;//The offset at which the bullet will spawn from the shot position (pos + shot height)

	//-- Shoot
	WeaponInfo weapon_info;					//Information about the varaibles of the current weapons. Overriden every time you get a new weapon.
	PerfTimer shot_timer;
	PerfTimer charged_timer;
	float charge_time						= 0.f;//Charge time in ms
	uint shot_sound							= 0u;
	void(Obj_Tank::*basic_shot_function[(uint)WEAPON::MAX])();
	void(Obj_Tank::*charged_shot_function[(uint)WEAPON::MAX])();

	//- Items
	ObjectType item							= ObjectType::NO_TYPE;

	//- Input
	INPUT_METHOD move_input					= INPUT_METHOD::KEYBOARD_MOUSE;//Starts as keyboard and switch to last pressed input
	INPUT_METHOD shot_input					= INPUT_METHOD::KEYBOARD_MOUSE;
	Controller ** controller = nullptr;

	//-- Keyboard inputs
	SDL_Scancode kb_item					= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_interact				= SDL_SCANCODE_UNKNOWN;
	int kb_shoot							= 0;
	SDL_Scancode kb_up						= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_left					= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_down					= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_right					= SDL_SCANCODE_UNKNOWN;

	//-- Controller inputs
	Joystick gamepad_move							= Joystick::INVALID;
	Joystick gamepad_aim							= Joystick::INVALID;
	SDL_GameControllerButton gamepad_interact		= SDL_CONTROLLER_BUTTON_INVALID;
	SDL_GameControllerButton gamepad_item			= SDL_CONTROLLER_BUTTON_INVALID;
	SDL_GameControllerAxis gamepad_shoot			= SDL_CONTROLLER_AXIS_INVALID;
	short int gamepad_shoot_last_frame				= 0;

	//- Drawing

	//-- Base
	Animation rotate_base;
	SDL_Texture * base_tex_yellow		= nullptr;
	//SDL_Texture * base_tex_orange		= nullptr;
	SDL_Texture * base_tex_red			= nullptr;
	//SDL_Texture * base_tex_light_green= nullptr;
	SDL_Texture * base_tex_pink			= nullptr;
	SDL_Texture * base_tex_light_blue	= nullptr;
	//SDL_Texture * base_tex_dark_blue	= nullptr;
	//SDL_Texture * base_tex_purple		= nullptr;
	SDL_Texture * base_shadow_tex		= nullptr;

	//-- Turret
	Animation rotate_turr;
	SDL_Texture * turr_tex				= nullptr;
	SDL_Texture * turr_shadow_tex		= nullptr;

public:
	Camera* camera_player				= nullptr;

};

#endif