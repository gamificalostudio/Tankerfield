#ifndef __OBJ_ENEMY_H__
#define __OBJ_ENEMY_H__

#include "Object.h"
#include "Circle.h"
#include "Timer.h"

enum class ENEMY_STATE
{
	SPAWN,
	IDLE,
	GET_PATH,
	MOVE,
	RECHEAD_POINT,
	GET_TELEPORT_POINT,
	TELEPORT_IN,
	TELEPORT_OUT,
	BURN,
	DEAD,
	STUNNED,
	STUNNED_CHARGED,
};

class Obj_Enemy : public Object
{
public:
	Obj_Enemy(fPoint pos);

	bool Update(float dt) ;

	virtual void ChangeTexture();

	virtual void Attack();

	void OnTriggerEnter(Collider * collider);

	inline bool IsOnGoal(fPoint goal); //const?

	void DrawDebug(const Camera* camera)override;

	virtual bool Draw(float dt, Camera* camera)override;

	virtual  bool Start() override;

protected:
	inline void UpdateVelocity();

	inline void UpdatePos(const float& dt);

	inline virtual void Movement(float &dt);

	virtual void Spawn(const float& dt) {};

	inline void RecheadPoint();

	inline void Dead();

	void ElectroDead();

	inline virtual void Idle();

	inline virtual int Move(float & dt);

	inline virtual void GetPath();

	inline virtual void GetTeleportPoint() {};

	inline virtual void TeleportIn(float & dt) {};

	inline virtual void TeleportOut(float & dt) {};

	inline virtual void Burn(const float& dt);

	bool CleanUp() override;

	void Oiled();

protected:


	int life = 0;
	float speed = 0.f;
	float original_speed = 0.f;
	SDL_Texture * tex = nullptr;
	SDL_Texture * tex_damaged = nullptr;
	SDL_Texture * oiled_tex = nullptr;
	SDL_Texture * tex_electro_dead = nullptr;

	/* Attack properties */
	float attack_frequency = 0.f;
	float attack_range = 0.f;//Tile distance in which the enemy can attack
	float attack_range_squared = 0.f;
	int attack_damage = 0;

	iPoint normal_draw_offset = { 0, 0 };
	iPoint electrocuted_draw_offset = { 0, 0 };

	SDL_Texture* burn_texture = nullptr;
	SDL_Texture* last_texture = nullptr;

	bool in_white = false;

	bool bool_electro_dead = false;

	ENEMY_STATE state = ENEMY_STATE::IDLE;
	ENEMY_STATE state_saved = ENEMY_STATE::IDLE;
	Animation* anim_saved;
	SDL_Texture* tex_saved = nullptr;
	bool is_electro_dead = false;

	Timer update_velocity_vec;
	Timer damaged_sprite_timer;
	Timer oiled_timer;
	int damaged_sprite_time = 0;

	bool oiled = false;

	Animation idle;
	Animation walk;
	Animation attack;
	Animation death;
	Animation burn;
	Animation dying_burn;
	

	Animation electro_dead;

	float scale = 0.f;

	float coll_w = 0.f;
	float coll_h = 0.f;

	uint sfx_attack = 0;
	uint sfx_spawn = 0;
	uint sfx_hit = 0;
	uint sfx_death = 0;

	Circle range_pos;

	Obj_Tank* target = nullptr;
	std::vector<iPoint> path;

	PerfTimer perf_timer;
	Timer path_timer;

	fPoint move_vect = { 0.0f, 0.0f };
	float check_path_time = 0.f;
	fPoint next_pos = { 0.f, 0.f };
	float detection_range = 0.0f;


	uint times_to_repeat_animation = 0u;
	uint times_animation_repeated = 0u;
	bool stun_charged = false;

	uint electocuted;
	uint channel_electrocuted;

	// Burn state variables------
	bool burn_fist_enter = true;
	Timer timer_change_direction;
	

	float max_time_change_direction = 0.5f;
	float fire_damage = 0;

	Collider* life_collider = nullptr;


};

#endif
