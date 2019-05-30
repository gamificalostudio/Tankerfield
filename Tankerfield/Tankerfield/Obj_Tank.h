#ifndef __OBJ_TANK_H__
#define __OBJ_TANK_H__

#include "Object.h"
#include "WeaponInfo.h"
#include "M_Input.h"
#include "Obj_Item.h"
#include "Timer.h"
#include "Obj_Portal.h"

struct Controller;
struct SDL_Texture;
class Camera;
class Obj_PickUp;
class Player_GUI;
class UI_IG_Helper;
class Obj_FlamethrowerFlame;

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

	void UpdateWeaponsWithoutBullets(float dt);

	bool Draw(float dt, Camera * camera) override;
	bool DrawShadow(Camera * camera, float dt) override;

	bool CleanUp() override;

	void OnTrigger(Collider* c1);
	void OnTriggerEnter(Collider* c1);
	void OnTriggerExit(Collider* c1);

public:
	//- Logic
	void ReduceLife(int damage);
	void IncreaseLife(int heal);
	void SetItem(ItemType Type);
	void SetWeapon(WEAPON type, uint level);
	void SetColor(const SDL_Color new_color);
	void SetTimeBetweenBullets(int time_between_bullets);

	int GetLife() const;
	int GetMaxLife() const;
	int GetTimeBetweenBullets() const;
	int GetTankNum() const;
	Controller ** GetController();
	bool GetShotAutomatically() const;
	bool GetIsElectroShotCharged() const;
	fPoint GetShotDir() const;
	float GetTurrAngle() const;
	WeaponInfo GetWeaponInfo() const;

	bool IsReady() const;
	void ShotAutormaticallyActivate();
	void ShotAutormaticallyDisactivate();
	std::vector<Object*>* GetEnemiesHitted();

	void CreatePortals();

public:

	//- Pick ups
	void SetPickUp(Obj_PickUp* pick_up);
	void SetGui(Player_GUI* gui);
	bool Alive() const;
	fPoint GetTurrPos() const;

private:
	//- Logic
	void SetLife(int life);//Don't use SetLife directly from other classes, use ReduceLife() or IncreaseLife()

	//- Movement
	void Movement(float dt);
	void ShotRecoilMovement(float &dt);
	void InputMovementKeyboard(fPoint & input);
	void InputMovementController(fPoint & input);

	//- Camera
	void CameraMovement(float dt);

	//- Shooting
	void Aim(float dt);
	void Shoot(float dt);
	void ShootChargedWeapon(float dt);
	void ShootSustainedWeapon();
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
	void ShootDoubleMissileCharged();
	void ShootDoubleMissile();
	void ShootHealingShot();
	void ShootHealingShotCharged();
	void ShootLaserShot();
	void ShootLaserShotCharged();
	void ShootOil();
	void ShootOilCharged();
	void ShootElectroShot();
	void ShootElectroShotCharged();

	void ReleaseFlameThrower();
	void ReleaseBasicShot() {};

	//- TankDeath
	void ReviveTank(float dt);
	void Die();

	//- Item
	void Item();

private:
	//- Logic
	int life								= 0;
	int max_life							= 0;
	int tank_num							= 0;//The number of tank. 0 is the first one.

	static int number_of_tanks;

	bool ready								= false;

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
	float charged_shot_speed				= 0.0f;
	float lerp_factor_recoil				= 0.f;
	Timer movement_timer;
	PerfTimer time_between_portal_tp;

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
	uint shot_type							= (uint)WEAPON::LASER_SHOT;

	//-- Shoot
	WeaponInfo weapon_info;					//Information about the varaibles of the current weapons. Overriden every time you get a new weapon.
	PerfTimer shot_timer;				//Determines how much time it must pass to be albe to shoot another shot again
	PerfTimer charged_shot_timer;
	PerfTimer sustained_shot_timer;
	PerfTimer shot_timer_basic_bullet;
	float charge_time						= 0.f;//Charge time in ms
	float quick_shot_time					= 0.f;//If time is bigger than this, you will start to use the sustained shot and won't use a qucik shot
	uint shot_sound = 0u;
	uint heal_sound = 0u;
	uint laser_sound = 0u;
	void(Obj_Tank::*shot1_function[(uint)WEAPON::MAX_WEAPONS])();//Shot 1 function. The basic shot for charged weapons. The quick shot for sustained weapons.
	void(Obj_Tank::*shot2_function[(uint)WEAPON::MAX_WEAPONS])();//Shot 2 function. The charged shot for charged wepoans. The sustained shot for sustained weapons.
	void(Obj_Tank::*release_shot[(uint)WEAPON::MAX_WEAPONS])();//Used on sustained weapons when you release a shot
	bool show_crosshairs					= false;
	bool shot_automatically					= false;

	//Electro shot
	PerfTimer electro_shot_timer;
	bool is_electro_shot_charged = false;
	uint electro_shot_sound = 0u;

	std::vector<Collider*> electric_shot_colliders_vector;

	std::vector<Collider*> electric_shot_colliders_charged_vector;

	std::vector<Object*> enemies_hitted;

	//Flamethrower shot
	Obj_FlamethrowerFlame * flame = nullptr;

public:

	float flame_coll_w = 3.f;
	float flame_coll_h = 3.f;

private:

	Timer flame_release_time;
	Collider* coll_flame = nullptr;

	//- Items
	ItemType item							= ItemType::NO_TYPE;
	

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
	Joystick gamepad_move							= Joystick::MAX;
	Joystick gamepad_aim							= Joystick::MAX;
	int dead_zone									= 0;
	SDL_GameControllerButton gamepad_interact		= SDL_CONTROLLER_BUTTON_INVALID;
	SDL_GameControllerButton gamepad_item			= SDL_CONTROLLER_BUTTON_INVALID;
	SDL_GameControllerAxis gamepad_shoot			= SDL_CONTROLLER_AXIS_INVALID;
	short int gamepad_shoot_last_frame				= 0;

	// Drawing =============================================
	SDL_Color tank_color = { 255, 255, 255, 255 };
	bool damaged = false;
	SDL_Texture* tur_damaged_tex = nullptr;
	SDL_Texture* base_damaged_tex = nullptr;
	Timer damaged_timer;

	// Base----------------------
	Animation rotate_base;
	SDL_Texture * base_color_tex			= nullptr;
	SDL_Texture * base_common_tex			= nullptr;
	SDL_Texture * base_shadow_tex			= nullptr;
	// Turret -------------------
	Animation rotate_turr;
	SDL_Texture * turret_color_tex			= nullptr;
	SDL_Texture * turret_common_tex			= nullptr;
	SDL_Texture * turret_shadow_tex			= nullptr;

	SDL_Texture * text_charging				= nullptr;
	Animation anim_charging;
	SDL_Texture * text_finished_charged		= nullptr;
	Animation anim_finished_charged;
	SDL_Texture * curr_text_charging = nullptr;
	Animation curr_anim_charging;
	int alpha = 0;
	float charging_scale = 0.f;
	uint charging_ready = 0u;
	bool charging = false;

	iPoint turr_draw_offset						= { 0,0 };
	float turr_scale							= 1.f;

	// Revive -----------------
	SDL_Texture * cycle_bar_tex = nullptr;
	Animation cycle_bar_anim;
	bool draw_revive_cycle_bar = false;

	//sfx 
	uint die_sfx						= 0u;

	bool picking = false;

	bool picked_weapon = false;
	bool started_ep_timer = false;
	fPoint electric_particle_initial_pos = { 0.0f, 0.0f };
	Timer ep_timer;

public:
	Camera* camera_player				= nullptr;

	UI_IG_Helper * tutorial_pick_up = nullptr;

	//- GUI
	Player_GUI*  gui = nullptr;

public:
	Obj_Portal * portal1;
	Obj_Portal * portal2;
	bool hit_no_enemie = false;
};

#endif