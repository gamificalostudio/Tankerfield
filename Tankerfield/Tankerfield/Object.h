#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <list>

#include "PugiXml\src\pugixml.hpp"
#include "Point.h"
#include "Animation.h"

#include "PerfTimer.h"

struct SDL_Texture;
class Collider;

class Object 
{
public:

	Object();

	Object(fPoint pos);

	virtual ~Object();

	const Collider* GetCollider() const;
	
	virtual bool Start() { return true; };

	virtual bool PreUpdate() { return true; };

	virtual bool Update(float dt);

	virtual bool PostUpdate(float dt);

	virtual bool CleanUp() { return true; };

	virtual bool Awake(pugi::xml_node&) { return true; };

	virtual bool Load(pugi::xml_node&) { return true; };

	virtual bool Save(pugi::xml_node&) const { return true; };

public:
	// Collision callbacks & methods ========================================

	virtual void OnTriggerEnter(Collider * collider) {}

	virtual void OnTrigger(Collider * collider) {}

	virtual void OnTriggerExit(Collider * collider) {}
public:

	int type = 0;

	// Transform (all units represented in map) ==================

	fPoint pos_map		= { 0.f, 0.f };//The position in the isometric grid. Use app->map->MapToScreenF() to get the position in which to Blit() the object.
	Collider* coll = nullptr;
	bool to_remove				= false;//Set it to true if you want the object to be removed
	//Used in Object::PostUpdate(float dt)
	SDL_Texture * curr_tex		= nullptr;//Points the current texture. Shouldn't allocate memory. Just assign the pointer to other textures already created.
	//Used in Object::PostUpdate(float dt) and for sprite sorting
	Animation * curr_anim		= nullptr;//Points the current animation. Shouldn't allocate memory. Just assign the pointer to other animations already created.
	float angle					= 0.f;//Direction that the object is facing
	iPoint draw_offset			= { 0.f, 0.f };//Change it to make the object not render from the top left in the position
};

#endif
