//#ifndef __OBJ_ENEMY_H__
//#define __OBJ_ENEMY_H__
//
//#include "Object.h"
//#include "Circle.h"
//enum class ENEMY_STATE
//{
//	SPAWN,
//	IDLE,
//	GET_PATH,
//	MOVE,
//	RECHEAD_POINT,
//	GET_TELEPORT_POINT,
//	TELEPORT_IN,
//	TELEPORT_OUT,
//	DEAD,
//};
//
//class Obj_Enemy : public Object
//{
//public:
//	Obj_Enemy(fPoint pos);
//
//	bool Update(float dt) override;
//
//	void Attack();
//
//	void Movement(float &dt);
//
//	void OnTriggerEnter(Collider * collider);
//
//	void OnTrigger(Collider* collider);
//
//	inline bool IsOnGoal(fPoint goal); //const?
//
//	void DrawDebug(const Camera* camera);
//
//public:
//	//Obj_Tank * caster = nullptr;
//	//ItemType type = ItemType::NO_TYPE;
//
//protected:
//	int life = 0;
//	SDL_Texture * tex = nullptr;
//	SDL_Texture * tex_damaged = nullptr;
//
//	ENEMY_STATE state = ENEMY_STATE::IDLE;
//
//	Timer damaged_sprite_timer;
//	int damaged_sprite_time = 0;
//
//	Animation idle;
//	Animation walk;
//	Animation attack;
//	Animation death;
//
//	uint sfx_attack = 0;
//	uint sfx_spawn = 0;
//	uint sfx_hit = 0;
//	uint sfx_death = 0;
//
//	Circle range_pos;
//
//	Obj_Tank* target = nullptr;
//	std::vector<fPoint> path;
//
//	PerfTimer perf_timer;
//
//	/* Attack properties */
//	float attack_frequency = 0.f;
//	float attack_range = 0.f;//Tile distance in which the enemy can attack
//	float attack_range_squared = 0.f;
//	int attack_damage = 0;
//};
//
//#endif
