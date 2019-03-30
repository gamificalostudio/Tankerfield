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
	void GetKeyboardInput(fPoint & input);
	void GetControllerInput(fPoint & input);
	bool IsShooting();
	void Shoot();
	bool IsHold();

public:
	WEAPON_TYPE weapon_type;

private:
	static SDL_Texture * base_tex;
	static SDL_Texture * turr_tex;
	static SDL_Texture * base_shadow_tex;
	static SDL_Texture * turr_shadow_tex;

	static SDL_Rect * base_rects;
	static int base_rects_num;
	//static SDL_Rect * turr_rects;

	float speed = 5.f;
	float angle = 0.f;
	Controller ** controller = nullptr;

	float cos_45 = 0.f;
	float sin_45 = 0.f;

	PerfTimer time_between_bullets_timer;
	float time_between_bullets = 0.f;

	std::map<WEAPON_TYPE, Weapon*> weapons;
};

#endif