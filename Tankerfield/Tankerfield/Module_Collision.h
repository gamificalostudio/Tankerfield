#ifndef __Module_Collision_H__
#define __Module_Collision_H__

#define MAX_COLLIDERS 50

#include "j1Module.h"
#include "p2Defs.h"
#include "PugiXml/src/pugixml.hpp"
#include "SDL/include/SDL.h"
#include <list>

class Object;

class Collider
{
public:
	enum class ON_TRIGGER_STATE
	{
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

	enum class TYPE
	{
		NONE,
		DYNAMIC,
		STATIC
	};

	Collider(const fPoint pos ,const  float width ,const  float height, const TAG tag, Object* object = nullptr ,j1Module* callback = nullptr) :
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

	j1Module* callback = nullptr;

	Object * object = nullptr;

	friend ModuleCollision;

};


class ModuleCollision : public j1Module
{
public:

	ModuleCollision();

	virtual ~ModuleCollision();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	Collider *AddCollider(fPoint pos, float width , float height, Collider::TAG type, j1Module* callback = nullptr, Object* object = nullptr);

	bool DeleteCollider(Collider* collider);

	void SolveOverlap(Collider* c1 , Collider * c2);


private:

	std::list<Collider*> colliders;

	bool matrix[(int)Collider::TAG::MAX][(int)Collider::TAG::MAX];

	bool debug = false;
};

#endif // __j1Collision_H__
