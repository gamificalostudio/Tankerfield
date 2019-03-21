#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <list>

#include "PugiXml\src\pugixml.hpp"
#include "p2Point.h"
#include "Animation.h"

#include "j1PerfTimer.h"

struct SDL_Texture;
struct Collider;

class Object 
{
public:

	Object();
	Object(int x, int y);
	virtual ~Object();
	const Collider* GetCollider() const;
	
	virtual bool Start() { return true; };
	virtual bool PreUpdate() { return true; };
	virtual bool Update(float dt);
	virtual bool PostUpdate() { return true; };
	virtual bool CleanUp() { return true; };
	virtual bool Awake(pugi::xml_node&) { return true; };

	virtual void Draw(float dt,SDL_Texture* texture);
	virtual bool Load(pugi::xml_node&) { return true; };
	virtual bool Save(pugi::xml_node&) const { return true; };



public:

	int type = 0;
	fPoint position;
	fPoint velocity;
	fPoint acceleration;
	//

	Animation* current_animation = nullptr;
	
	Collider* coll = nullptr;
};

#endif
