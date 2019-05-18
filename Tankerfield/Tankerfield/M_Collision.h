#ifndef __Module_Collision_H__
#define __Module_Collision_H__

#define MAX_COLLIDERS 50

#include <list>
#include <vector>
#include <map>
#include <typeinfo>

#include "PugiXml/src/pugixml.hpp"

#include "Module.h"

class Object;
class M_Collision;

enum class TAG : int
{
	NONE = 0,
	WALL,
	WATER,
	BULLET,
	FRIENDLY_BULLET,
	BULLET_LASER,
	BULLET_OIL,
	ELECTRO_SHOT,
	PORTAL,
	ENEMY,
	GOD,
	REWARD_ZONE,
	PICK_UP,
	REWARD_BOX,
	PLAYER,
	ROAD,
	OIL_POOL,
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

	enum class ON_TRIGGER_STATE
	{
		NONE,
		ENTER,
		STAY,
		EXIT
	};



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
		position(pos),
		width(width),
		height(height),
		damage(damage),
		tag(tag),
		body_type(body),
		object(object)
	{}

	void SetPos(const fPoint pos)
	{
		position = pos;
	}

	void SetObjOffset(const fPoint offset)
	{
		obj_offset = offset;
	}

	void SetPosToObj();

	Object* GetObj() 
	{
		return object;
	}

	void GetSize(float & w, float & h) 
	{
		w = width;
		h = height;
	}

	inline bool CheckCollision(Collider*  coll) const;

	TAG GetTag() const
	{
		return tag;
	}

	void SetTag(TAG new_tag)
	{
		tag = new_tag;
	}

	void ActiveOnTrigger(bool value);

	void Destroy();


	bool GetIsActivated() const;

public:

	float damage = 0.f;

	bool to_destroy = false;
  
	bool is_sensor = false; // True = Avoid overlap resolve || Only in dynamic bodies

private:

	fPoint position = { 0.f , 0.f };

	fPoint obj_offset = { 0.f, 0.f };

	float width = 0.f;
		
	float height = 0.f;

	// Collision vars ==============================================

	bool active_on_trigger = true;

	TAG tag = TAG::NONE;

	std::list<Collider*> collisions_list;




	Object * object = nullptr;

	OVERLAP_DIR last_overlap = OVERLAP_DIR::NONE;

	BODY_TYPE body_type = BODY_TYPE::STATIC;

	friend M_Collision;
	friend M_ObjManager;
};

class M_Collision : public Module
{
public:

	M_Collision();

	virtual ~M_Collision();

	bool Update(float dt) override;

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

private:

	std::list<Collider*> static_colliders;

	std::list<Collider*> dynamic_colliders;

	std::list<Collider*> colliders_to_add;

	std::map<Collider* ,bool> mod_on_trigger_colliders;

	bool on_trigger_matrix[(int)TAG::MAX][(int)TAG::MAX];

	bool solve_overlap_matrix[(int)TAG::MAX][(int)TAG::MAX];

	bool is_updating = false;

	bool debug = false;

	friend Collider;
};

#endif // __j1Collision_H__


