#ifndef __Module_Collision_H__
#define __Module_Collision_H__

#define MAX_COLLIDERS 50

#include "j1Module.h"
#include "p2Defs.h"
#include "PugiXml/src/pugixml.hpp"
#include "SDL/include/SDL.h"
#include <list>

class Collider
{
	public:

	enum class OFFSET_DIR : uint
	{
		RIGHT,
		LEFT,
		UP,
		DOWN,
		UNKNOWN
	};

	enum class TYPE : int
	{
		NONE = -1,
		WALL,
		PLAYER,
		ENEMY,
		GOD,
		MAX
	};

	Collider(fPoint pos , float width , float height, TYPE type, j1Module* callback = nullptr) :
		position(pos),
		width(width),
		height(height),
		type(type),
		callback(callback)
	{}

	void SetPos(const float x,const  float y)
	{
		position = { x, y };
	}

	bool CheckCollision(Collider*  coll) const;

private:

	fPoint position = { 0.f , 0.f };

	float width = 0.f;
		
	float height = 0.f;

	TYPE type = TYPE::NONE;

	j1Module* callback = nullptr;

	bool is_static = true;

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

	Collider  *AddCollider(fPoint pos, float width , float height, Collider::TYPE type, j1Module* callback = nullptr);

	bool CheckOverlap(std::list<Collider::OFFSET_DIR> &directions, Collider *dynamic_col, Collider::TYPE type, fPoint &position, fPoint &velocity);

	bool DeleteCollider(Collider* collider);


private:

	std::list<Collider*> colliders;

	bool matrix[(int)Collider::TYPE::MAX][(int)Collider::TYPE::MAX];

	bool debug = false;
};

#endif // __j1Collision_H__
