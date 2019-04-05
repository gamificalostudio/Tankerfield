#ifndef __Module_Collision_H__
#define __Module_Collision_H__

#define MAX_COLLIDERS 50

#include <list>
#include <vector>
#include <typeinfo>

#include "PugiXml/src/pugixml.hpp"

#include "Module.h"

class Object;
class M_Collision;


class Collider
{
public:
	
	enum class TAG : int
	{
		NONE = -1,
		WALL,
		BULLET,
		PLAYER,
		ENEMY,
		GOD,
		REWARD_ZONE,
		MAX
	};

	enum class ON_TRIGGER_STATE
	{
		NONE,
		ENTER,
		STAY,
		EXIT
	};

	enum class BODY_TYPE
	{
		DYNAMIC,
		STATIC,
		SENSOR
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

	Collider(const fPoint pos,const  float width, const  float height, const TAG tag, Object* object = nullptr) :
		position(pos),
		width(width),
		height(height),
		tag(tag),
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

	template<typename TYPE>

	TYPE* GetObject() 
	{
		if (object != nullptr & typeid(TYPE) == typeid(object))
		{
			return (TYPE*)object;
		}
		else
		{
			return nullptr;
		}
	}

	TAG GetTag() const
	{
		return tag;
	}

	void AddRigidBody(const Collider::BODY_TYPE new_body_type)
	{
		body_type = new_body_type;
	}

	void Destroy();

	bool CheckCollision(Collider* collider) const;

private:

	fPoint position = { 0.f , 0.f };

	fPoint obj_offset = { 0.f, 0.f };

	float width = 0.f;
		
	float height = 0.f;

	Object * object = nullptr;

	TAG tag = TAG::NONE;

	std::list<Collider*> collisions_list;

	bool to_destroy = false;

	// Body vars ===================================================

	BODY_TYPE body_type = BODY_TYPE::STATIC;

	OVERLAP_DIR last_overlap = OVERLAP_DIR::NONE;

	friend M_Collision;
};

class M_Collision : public Module
{
public:

	M_Collision();

	virtual ~M_Collision();

	bool Update(float dt) override;

	bool PostUpdate(float dt) override;

	bool CleanUp() override;

	Collider* AddCollider(fPoint pos, float width , float height, Collider::TAG type, Object* object = nullptr);

	void SolveOverlapDS(Collider * c1, Collider * c2); // Solve Static vs Dynamic Overlap

	void SolveOverlapDD(Collider * c1, Collider * c2); // Solve Dynamic vs Dynamic Overlap

	inline void DoOnTrigger(Collider* c1, Collider *c2);

	inline void DoOnTriggerExit(Collider* c1, Collider *c2);

private:

	std::list<Collider*> colliders;

	bool matrix[(int)Collider::TAG::MAX][(int)Collider::TAG::MAX];

	bool debug = false;
};

#endif // __j1Collision_H__


