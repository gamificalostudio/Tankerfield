#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <list>

#include "PugiXml\src\pugixml.hpp"
#include "Point.h"
#include "Animation.h"

#include "PerfTimer.h"

struct SDL_Texture;
class Collider;

enum ROTATION_DIR 
{
	CLOCKWISE,
	COUNTER_CLOCKWISE,
	INVALID
};

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

	virtual bool PostUpdate();

	virtual bool CleanUp() { return true; };

	virtual bool Awake(pugi::xml_node&) { return true; };

	virtual bool Load(pugi::xml_node&) { return true; };

	virtual bool Save(pugi::xml_node&) const { return true; };

	// Collision callbakcs ========================================

	virtual void OnTriggerEnter(Collider * collider) {}

	virtual void OnTrigger(Collider * collider) {}

	virtual void OnTriggerExit(Collider * collider) {}

	// Clamps the rotation from 0 to 360 degrees ==================

	float ClampRotation(float angle);

	uint GetRotatedIndex(uint rect_num, float angle, ROTATION_DIR rot_dir = ROTATION_DIR::COUNTER_CLOCKWISE, float fist_rect_dir = 90);

	bool LoadRects(pugi::xml_node const &node, SDL_Rect * rects);

	bool LoadAnimation(pugi::xml_node &node, Animation &anim);

public:

	int type = 0;

	// Transform (all units represented in map) ==================

	fPoint pos_map		= { 0.f, 0.f };

	fPoint velocity		= { 0.f, 0.f };

	fPoint acceleration = { 0.f, 0.f };

	bool to_remove = false;//Set it to true if you want the object to be removed
	
	Animation* current_animation = nullptr;
	
	Collider* coll = nullptr;
};

#endif
