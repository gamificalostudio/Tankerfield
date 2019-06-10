#ifndef __OBJ_TANK_H__
#define __OBJ_TANK_H__

#include <string>
#include <list>

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

enum class INPUT_METHOD {
	KEYBOARD_MOUSE,
	CONTROLLER
};

struct MovementBuff
{
	std::string source = "";
	float bonus_speed = 0.f;
	bool has_decay = false;
	bool decaying = false;//If the buff is being reduced by the decay rate. For example this is set to true when the tank exits a road
	//The decay that the bonus will have each second
	float decay_rate = 0.f;
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

	bool Draw(Camera * camera) override;
	void DrawCrosshair(Camera * camera);
	bool DrawShadow(Camera * camera, float dt) override;

	bool CleanUp() override;

	void OnTrigger(Collider* c1, float dt);
	void OnTriggerEnter(Collider* c1, float dt);
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
	int GetController();
	bool GetShotAutomatically() const;
	bool GetIsElectroShotCharged() const;
	fPoint GetShotDir() const;
	float GetTurrAngle() const;
	WeaponInfo GetWeaponInfo() const;

	void ShotAutormaticallyActivate();
	void ShotAutormaticallyDisactivate();
	std::vector<Object*>* GetEnemiesHitted();
	void CreatePortals();

	void NewRound(int round);
	
	//- Pick ups
	void SetPickUp(Obj_PickUp* pick_up);
	void SetGui(Player_GUI* gui);
	bool Alive() const;
	fPoint GetTurrPos() const;

private:
	//- Movement
	void Movement(float dt);
	void InputMovementKeyboard(fPoint & input);
	void InputMovementController(fPoint & input);
	bool UpdateMaxSpeedBuffs(float dt);
	bool AddMaxSpeedBuff(MovementBuff buff);
	bool RemoveMaxSpeedBuff(std::string source);
	float GetMaxSpeed();//Returns the maximum speed of the tank, tanking into account the bonuses it has
	void ReduceSpeed(float reduction);
	void SetSpeed(float speed);
	float GetCurrSpeed();

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
	float base_max_speed							= 0.f;
	float speed_colliding_with_building				= 0.f;
	MovementBuff road_buff;
	MovementBuff charged_shot_buff;
	MovementBuff recoil_buff;

	float acceleration_power				= 0.f;
	fPoint velocity_map						= { 0.f, 0.f };
	fPoint acceleration_map					= { 0.f, 0.f};
	float cos_45							= 0.f;//TODO: Create a macro with its value directly
	float sin_45							= 0.f;
	float base_angle_lerp_factor			= 0.f;
	std::list<MovementBuff> movement_buffs;

	//-- Move tutorial
	Timer tutorial_move_timer;
	UI_IG_Helper * tutorial_move			= nullptr;
	int tutorial_move_time					= 0;//The time the tutorial move image will appear on screen (ms)
	bool tutorial_move_pressed				= false;

	//- Run over
	float run_over_damage_multiplier		= 0.f;//The damage it does when it hits an enemy (it is multiplied by the current speed)
	float run_over_speed_reduction			= 0.f;//The speed it loses every time it hits an enemy


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
	uint pick_item_sound = 0u;
	uint pick_weapon_sound = 0u;
	void(Obj_Tank::*shot1_function[(uint)WEAPON::MAX_WEAPONS])();//Shot 1 function. The basic shot for charged weapons. The quick shot for sustained weapons.
	void(Obj_Tank::*shot2_function[(uint)WEAPON::MAX_WEAPONS])();//Shot 2 function. The charged shot for charged wepoans. The sustained shot for sustained weapons.
	void(Obj_Tank::*release_shot[(uint)WEAPON::MAX_WEAPONS])();//Used on sustained weapons when you release a shot
	bool show_crosshairs					= false;
	bool shot_automatically					= false;
	float recoil_speed						= 0.f;
	float brake_power						= 0.f;

	//Teleport
	PerfTimer time_between_portal_tp;

	//Electro shot
	PerfTimer electro_shot_timer;
	bool is_electro_shot_charged = false;
	uint electro_shot_sound = 0u;

	std::vector<Collider*> electric_shot_colliders_vector;

	std::vector<Collider*> electric_shot_colliders_charged_vector;

	std::vector<Object*> enemies_hitted;

	//Flamethrower
	Obj_Emitter * flame_emitter = nullptr;

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
	int controller = -1;

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
	CONTROLLER_BUTTON gamepad_interact		= CONTROLLER_BUTTON::NONE;
	CONTROLLER_BUTTON gamepad_item			= CONTROLLER_BUTTON::NONE;
	CONTROLLER_BUTTON gamepad_shoot			= CONTROLLER_BUTTON::NONE;
	short int gamepad_shoot_last_frame				= 0;
	float vibration_percentage = 1.0f;

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

	SDL_Texture * crosshair_tex				= nullptr;

	iPoint turr_draw_offset						= { 0,0 };
	float turr_scale							= 1.f;

	// Revive -----------------
	SDL_Texture * cycle_bar_tex = nullptr;
	Animation cycle_bar_anim;
	bool draw_revive_cycle_bar = false;

	//sfx 
	uint die_sfx						= 0u;

	bool picking = false;

public:
	Camera* camera_player				= nullptr;

	UI_IG_Helper * tutorial_pick_up = nullptr;

	//- GUI
	Player_GUI*  gui = nullptr;

public:
	bool hit_no_enemie = false;
};

#endif