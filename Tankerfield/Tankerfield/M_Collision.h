#ifndef __Module_Collision_H__
#define __Module_Collision_H__

#define MAX_COLLIDERS 50

#include "Module.h"
#include "PugiXml/src/pugixml.hpp"
#include <list>

class Object;
class M_Collision;



class Collider
{
public:
	
	enum class TYPE
	{
		NONE,
		DYNAMIC,
		STATIC,
		SENSOR
	};

	enum class ON_TRIGGER_STATE
	{
		NONE,
		ENTER,
		STAY,
		EXIT
	};

	enum class TAG : int
	{
		NONE = -1,
		WALL,
		PLAYER,
		ENEMY,
		GOD,
		MAX
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

	Collider(const fPoint pos ,const  float width ,const  float height, const TAG tag, Object* object = nullptr ,Module* callback = nullptr) :
		position(pos),
		width(width),
		height(height),
		tag(tag),
		object(object),
		callback(callback)
	{}

	void SetPos(const float x,const  float y)
	{
		position = { x, y };
	}

	void SetType(const Collider::TYPE new_type)
	{
		type = new_type;
	}

	bool CheckCollision(Collider*  coll) const;

private:

	fPoint position = { 0.f , 0.f };

	float width = 0.f;
		
	float height = 0.f;

	TAG tag = TAG::NONE;

	TYPE type = TYPE::STATIC;

	ON_TRIGGER_STATE on_trigger_state = ON_TRIGGER_STATE::NONE;

	Module* callback = nullptr;

	Object * object = nullptr;

	OVERLAP_DIR last_overlap = OVERLAP_DIR::NONE;

	friend M_Collision;
};

class M_Collision : public Module
{
public:

	M_Collision();

	virtual ~M_Collision();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	Collider *AddCollider(fPoint pos, float width , float height, Collider::TAG type, Module* callback = nullptr, Object* object = nullptr);

	bool DeleteCollider(Collider* collider);

	void SolveOverlapDS(Collider * c1, Collider * c2); // Solve Static vs Dynamic Overlap

	void SolveOverlapDD(Collider * c1, Collider * c2); // Solve Dynamic vs Dynamic Overlap


private:

	std::list<Collider*> colliders;

	bool matrix[(int)Collider::TAG::MAX][(int)Collider::TAG::MAX];

	bool debug = false;
};

#endif // __j1Collision_H__
