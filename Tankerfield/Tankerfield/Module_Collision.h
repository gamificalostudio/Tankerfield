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

	Collider(SDL_Rect rectangle, TYPE type, j1Module* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}

	void SetPos(const int x,const  int y)
	{
		rect.x = x;
		rect.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const;

	SDL_Rect rect = { 0, 0, 0, 0 };
	TYPE type = TYPE::NONE;
	j1Module* callback = nullptr;
};


class ModuleCollision : public j1Module
{
public:

	ModuleCollision();

	virtual ~ModuleCollision();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	Collider  *AddCollider(SDL_Rect rect, Collider::TYPE type, j1Module* callback = nullptr);

	//Collider::OFFSET_DIR  SolveOverlap(Collider *dynamic_col, Collider *static_col, fPoint &position, fPoint &velocity);

	bool CheckOverlap(std::list<Collider::OFFSET_DIR> &directions, Collider *dynamic_col, Collider::TYPE type, fPoint &position, fPoint &velocity);

	bool DeleteCollider(Collider* collider);


private:
	std::list<Collider*> colliders;
	bool matrix[(int)Collider::TYPE::MAX][(int)Collider::TYPE::MAX];
	bool debug = false;
};

#endif // __j1Collision_H__
