#ifndef __OBJ_TANK_H__
#define __OBJ_TANK_H__

#include "Object.h"
#include "WeaponInfo.h"
#include "M_Input.h"
#include "Obj_Item.h"
#include "Timer.h"

struct Controller;
struct SDL_Texture;
class Camera;
class Obj_PickUp;
class Player_GUI;
class UI_IG_Helper;

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

	bool Draw(float dt, Camera * camera) override;
	bool DrawShadow(Camera * camera, float dt) override;

	bool CleanUp() override;

	void OnTrigger(Collider* c1);
	void OnTriggerExit(Collider* c1);

public:
	//- Logic
	void SetLife(int life);
	void SetItem(ObjectType Type);
	void SetWeapon(WEAPON type, uint level);
	WeaponInfo GetWeaponInfo() const;
	void SetTimeBetweenBullets(int time_between_bullets);
	int GetLife();
	int GetMaxLife();
	int GetTimeBetweenBullets();
	fPoint GetShotDir() const;
	bool IsReady() const;
	int GetTankNum();

public:

	//- Pick ups
	void SetPickUp(Obj_PickUp* pick_up);
	void SetGui(Player_GUI* gui);
	bool Alive();
	fPoint GetTurrPos();

private:
	//- Movement
	void Movement(float dt);
	void ShotRecoilMovement(float &dt);
	void InputMovementKeyboard(fPoint & input);
	void InputMovementController(fPoint & input);

	//- Camera
	void CameraMovement(float dt);

	//- Shooting
	void Shoot(float dt);
	void InputShotMouse(const fPoint & shot_pos, fPoint & input_dir, fPoint & iso_dir);
	void InputShotController(const fPoint & shot_pos, fPoint & input, fPoint & iso_dir);
	bool PressShot();
	bool HoldShot();
	bool ReleaseShot();

	//- Input
	void SelectInputMethod();
	void InputReadyKeyboard();
	bool PressInteract();
	bool ReleaseInteract();

	//- Weapons methods
	void InitWeapons();
	void ShootBasic();
	void ShootFlameThrower();
	void ShootDoubleMissile();
	void ShootHealingShot();
	void ShootLaserShot();

	//- TankDeath
	void ReviveTank(float dt);
	void StopTank();

	//- Item
	void Item();

private:
	//- Logic
	int life								= 0;
	int max_life							= 0;
	int tank_num							= 0;//The number of tank. 0 is the first one.

	static int number_of_tanks;

	bool ready								= false;
	bool fire_dead							= false;


	//- Movement
	float curr_speed						= 0.f;
	float speed								= 0.f;
	float road_buff							= 0.f;
	fPoint velocity							= { 0.f, 0.f };
	fPoint max_velocity						= { 0.f, 0.f };
	fPoint velocity_recoil_lerp				= { 0.f, 0.f };
	fPoint velocity_recoil_final_lerp		= { 0.f, 0.f };
	fPoint recoil_dir						= { 0.f, 0.f };
	float velocity_recoil_curr_speed		= 0.f;
	float velocity_recoil_decay				= 0.f;
	float velocity_recoil_speed_max			= 0.f;
	float velocity_recoil_speed_max_charged = 0.f;
	float lerp_factor_recoil				= 0.f;
	Timer movement_timer;
	


	float cos_45							= 0.f;//TODO: Create a macro with its value directly
	float sin_45							= 0.f;
	float base_angle_lerp_factor			= 0.f;
	//-- Move tutorial
	Timer tutorial_move_timer;
	UI_IG_Helper * tutorial_move			= nullptr;
	int tutorial_move_time					= 0;//The time the tutorial move image will appear on screen (ms)
	bool tutorial_move_pressed				= false;


	//- Shooting
	fPoint turr_pos							= { 0.f, 0.f };//The position of the turret in the map
	float turr_angle						= 0.f;
	fPoint shot_dir							= { 0.f, 0.f };
	float cannon_height						= 0.f;//Used to calculate the turr position (position from which bullets are shot)
	float shot_angle_lerp_factor			= 0.f;
	float turr_target_angle					= 0.f;

	//- Revive
	float revive_range						= 0.f;
	float revive_range_squared				= 0.f;
	int	  revive_life						= 0;
	float revive_time						= 0.f;//RTime it takes to revive an ally (in seconds)
	UI_IG_Helper * tutorial_revive			= nullptr;
	bool reviving_tank[4]					= { false };//Is this tank reviving [?] tank?
	Timer revive_timer[4];					//Time that you've been reviving other tanks
	uint revive_sfx							= 0u;

	//-- Basic shoot
	uint shot_type							= (uint)WEAPON::DOUBLE_MISSILE;

	//-- Shoot
	WeaponInfo weapon_info;					//Information about the varaibles of the current weapons. Overriden every time you get a new weapon.
	PerfTimer shot_timer;
	PerfTimer charged_timer;
	float charge_time						= 0.f;//Charge time in ms
	uint shot_sound							= 0u;
	void(Obj_Tank::*basic_shot_function[(uint)WEAPON::MAX_WEAPONS])();
	void(Obj_Tank::*charged_shot_function[(uint)WEAPON::MAX_WEAPONS])();
	bool show_crosshairs					= false;

	//- Items
	ObjectType item							= ObjectType::NO_TYPE;
	UI_IG_Helper * tutorial_pick_up			= nullptr;

	//- GUI
	Player_GUI*  gui                        = nullptr;

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
	SDL_Scancode kb_ready					= SDL_SCANCODE_UNKNOWN;

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
	SDL_Texture * base_tex_orange			= nullptr;
	SDL_Texture * base_tex_green			= nullptr;
	SDL_Texture * base_tex_pink				= nullptr;
	SDL_Texture * base_tex_blue				= nullptr;
	SDL_Texture * base_shadow_tex			= nullptr;

	//-- Turret
	Animation rotate_turr;
	SDL_Texture * turr_tex						= nullptr;
	SDL_Texture * turr_tex_orange				= nullptr;
	SDL_Texture * turr_tex_green				= nullptr;
	SDL_Texture * turr_tex_pink					= nullptr;
	SDL_Texture * turr_tex_blue					= nullptr;
	SDL_Texture * turr_shadow_tex				= nullptr;

	//-- Revive
	SDL_Texture * cycle_bar_tex = nullptr;
	Animation cycle_bar_anim;
	bool draw_revive_cycle_bar = false;

	//sfx 
	uint die_sfx						= 0u;

public:
	Camera* camera_player				= nullptr;
};

#endif