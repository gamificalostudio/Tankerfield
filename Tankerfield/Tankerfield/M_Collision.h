#ifndef __Module_Collision_H__
#define __Module_Collision_H__

#define MAX_COLLIDERS 50

#include <list>
#include <vector>
#include <map>
#include <typeinfo>
#include "Rect.h"

#include "PugiXml/src/pugixml.hpp"

#include "Module.h"

class Object;
class M_Collision;
class M_ObjManager;
class QuadTree_Collision;

enum class TAG : int
{
	NONE = 0,
	WALL,
	WATER,
	BULLET,
	BULLET_ENEMY,
	FRIENDLY_BULLET,
	BULLET_LASER,
	BULLET_OIL,
	ELECTRO_SHOT,
	PORTAL,
	ENEMY,
	HEALING_AREA_SHOT,
	GOD,
	REWARD_ZONE,
	PICK_UP,
	REWARD_BOX,
	PLAYER,
	ROAD,
	OIL_POOL,
	FLAMETHROWER,
	MAX
};

enum class BODY_TYPE
{
	DYNAMIC,
	STATIC
};

class Collider
{
public:

	enum class OVERLAP_DIR : int
	{
		NONE = -1,
		LEFT,
		RIGHT,
		UP,
		DOWN,
		MAX
	};

	Collider(const fPoint pos, const  float width, const  float height, const float damage, const TAG tag, BODY_TYPE body, Object* object = nullptr) :
		damage(damage),
		tag(tag),
		body_type(body),
		object(object)
	{
		rect.create(pos.x, pos.y, width, height);
	}

	fRect GetRect() const
	{
		return rect;
	}

	void SetPos(const fPoint pos)
	{
		rect.pos = pos;
	}

	void SetObjOffset(const fPoint offset)
	{
		obj_offset = offset;
	}

	void SetTag(TAG new_tag)
	{
		tag = new_tag;
	}

	void SetPosToObj();

	Object* GetObj() 
	{
		return object;
	}

	void GetSize(float & w, float & h) 
	{
		w = rect.w;
		h = rect.h;
	}

	TAG GetTag() const
	{
		return tag;
	}

	inline bool CheckCollision(Collider*  coll) const
	{
		return !(coll->rect.pos.x >= (rect.pos.x + rect.w) || (coll->rect.pos.x + coll->rect.w) <= rect.pos.x || coll->rect.pos.y >= (rect.pos.y + rect.h) || (coll->rect.pos.y + coll->rect.h) <= rect.pos.y);
	}

	void ActiveOnTrigger(bool value);

	void Destroy();

	bool GetIsActivated() const;

public:

	float damage = 0.f;

	bool to_destroy = false;
  
	bool is_sensor = false; // True = Avoid overlap resolve || Only in dynamic bodies

private:

	fRect  rect;

	fPoint obj_offset = { 0.f, 0.f };

	// Collision vars ==============================================

	bool active_on_trigger = true;

	TAG tag = TAG::NONE;

	std::list<Collider*> collisions_list;

	std::map<Collider*, bool> triggers_list;

	Object * object = nullptr;

	OVERLAP_DIR last_overlap = OVERLAP_DIR::NONE;

	BODY_TYPE body_type = BODY_TYPE::STATIC;

	// Quadtree ===================================================

	bool subdivision_intersection[4];

	friend M_Collision;
	friend M_ObjManager;
	friend QuadTree_Collision;
};

struct Collision_Info
{
	int type = 0;             // 1. Dynamic-Dynamic   2. Dynamic-Static
	Collider* collider_1 = nullptr;
	Collider* collider_2 = nullptr;
};

class M_Collision : public Module
{
public:

	M_Collision();

	virtual ~M_Collision();

	bool Start();

	bool Update(float dt) override;

	bool UpdateQuadtreeMethod(float dt);

	bool UpdateForcedMethod(float dt);

	bool PostUpdate(float dt) override;

	bool CleanUp() override;

	bool Reset();

	Collider* AddCollider(fPoint pos, float width, float height, TAG tag, BODY_TYPE body ,float damage=0.f, Object* object = nullptr);

private:

	void SolveOverlapDS(Collider * c1, Collider * c2); // Solve Static vs Dynamic Overlap

	void SolveOverlapDD(Collider * c1, Collider * c2); // Solve Dynamic vs Dynamic Overlap

	inline void DoOnTrigger(Collider* c1, Collider *c2);

	inline void DoOnTriggerExit(Collider* c1, Collider *c2);

	void DestroyColliders();

public:

	int collisions_per_frame = 0;

private:

	
	// Collision quadtree ====================================================

	QuadTree_Collision* quad_tree_collision = nullptr;

	// Collider lists ========================================================

	std::list<Collider*> static_colliders;

	std::list<Collider*> dynamic_colliders;

	std::list<Collider*> colliders_to_add;

	// Collider matrix ======================================================

	bool on_trigger_matrix[(int)TAG::MAX][(int)TAG::MAX];

	bool solve_overlap_matrix[(int)TAG::MAX][(int)TAG::MAX];

	bool is_updating = false;

	bool debug = false;

	friend Collider;
};

#endif // __j1Collision_H__


