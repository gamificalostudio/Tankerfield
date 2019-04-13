#ifndef __OBJ_TANK_H__
#define __OBJ_TANK_H__

#include "Object.h"
#include "WeaponInfo.h"
#include "M_Input.h"
#include "Obj_Item.h"

struct Controller;
struct SDL_Texture;
class Camera;

enum class WEAPON {
	BASIC,
	FLAMETHROWER,
	DOUBLE_MISSILE,
	HEALING_SHOT,
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

	void CameraMovement(float dt);

	bool Draw(float dt, Camera * camera) override;
	bool DrawShadow(Camera * camera) override;


	bool CleanUp() override;

	void OnTrigger(Collider* c1);

public:
	//- Logic
	void SetLife(int life);
	int GetLife();
	int GetMaxLife();
public:
	//-Logic
	int life = 0;
	int max_life = 0;

	Obj_Tank* receiver = nullptr;

private:
	//- Movement
	void Movement(float dt);
	void InputMovementKeyboard(fPoint & input,float dt);
	void InputMovementController(fPoint & input);

	//- Shooting
	void Shoot();
	void InputShotMouse(const fPoint & shot_pos, fPoint & input_dir, fPoint & iso_dir);
	void InputShotController(const fPoint & shot_pos, fPoint & input, fPoint & iso_dir);
	bool IsShootingBasic();
	bool IsShootingSpecial();

	//- Input
	void SelectInputMethod();

	//- Weapons methods
	void ShootBasic();
	void ShootFlameThrower();
	void ShootDoubleMissile();
	void ShootHealingShot();

	//- Item
	void Item();


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
	fPoint turr_pos							= { 0.f, 0.f };//The position of the turret in the map
	float turr_angle						= 0.f;
	fPoint shot_dir							= { 0.f, 0.f };
	float cannon_height						= 0.f;//Used to calculate the shot position
	float cannon_length						= 0.f;//The offset at which the bullet will spawn from the shot position (pos + shot height)

	//-- Basic shoot
	uint basic_shot							= (uint)WEAPON::BASIC;
	PerfTimer basic_shot_timer;
  
	//-- Special shoot
	uint special_shoot						= (uint)WEAPON::HEALING_SHOT;
	PerfTimer special_shot_timer;
	void(Obj_Tank::*shot_function[(uint)WEAPON::MAX])();

	//- Items
	ObjectType item							= ObjectType::NO_TYPE;

	//- Input
	INPUT_METHOD move_input					= INPUT_METHOD::KEYBOARD_MOUSE;//Starts as keyboard and switch to last pressed input
	INPUT_METHOD shot_input					= INPUT_METHOD::KEYBOARD_MOUSE;
	Controller ** controller = nullptr;

	//-- Keyboard inputs
	SDL_Scancode kb_item					= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_interact			= SDL_SCANCODE_UNKNOWN;
	int kb_shoot_basic						= 0;
	int kb_shoot_special					= 0;
	SDL_Scancode kb_up						= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_left					= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_down					= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_right					= SDL_SCANCODE_UNKNOWN;

	//-- Controller inputs
	Joystick gamepad_move					= Joystick::INVALID;
	Joystick gamepad_aim					= Joystick::INVALID;
	SDL_GameControllerButton gamepad_interact	= SDL_CONTROLLER_BUTTON_INVALID;
	SDL_GameControllerButton gamepad_item		= SDL_CONTROLLER_BUTTON_INVALID;
	SDL_GameControllerAxis gamepad_shoot_basic		= SDL_CONTROLLER_AXIS_INVALID;
	SDL_GameControllerAxis gamepad_shoot_special	= SDL_CONTROLLER_AXIS_INVALID;
public:
	Camera* camera_player					= nullptr;
};

#endif