#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <list>

#include "PugiXml\src\pugixml.hpp"
#include "Point.h"
#include "Animation.h"
#include "M_ObjManager.h"

#include "PerfTimer.h"

#define ISO_COMPENSATION 45 //The isometric view is turned 45 degrees to the right. Used to compensate the difference between the vectors in the isometric space and the angle we need in the spritesheets.

struct SDL_Texture;
class Collider;

enum ROTATION_DIR 
{
	CLOCKWISE,
	COUNTER_CLOCKWISE,
	INVALID
};


struct SDL_Texture;
class Collider;

struct TileSetObject {

	std::string name;
	uint tilewidth = 0;
	uint tileheight = 0;
	uint spacing = 0;
	uint margin = 0;
	uint tilecount = 0;
	uint columns = 0;
	std::string imagePath;
	SDL_Texture* texture = nullptr;
	uint width = 0;
	uint height = 0;
};

struct ObjectInfo
{
	TileSetObject tileset;
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

	virtual bool PostUpdate(float dt);

	virtual bool DrawShadow() { return true; }

	virtual bool CleanUp() { return true; };

	virtual bool Awake(pugi::xml_node&) { return true; };

	virtual bool Load(pugi::xml_node&) { return true; };

	virtual bool Save(pugi::xml_node&) const { return true; };

	// Collision callbacks & methods ========================================

	virtual void OnTriggerEnter(Collider * collider) {}

	virtual void OnTrigger(Collider * collider) {}

	virtual void OnTriggerExit(Collider * collider) {}

	// Clamps the rotation from 0 to 360 degrees ==================

	float ClampRotation(float angle);


	void SetPivot(const float &x, const float &y);  
	void SetRect(int x, int y, int w, int h);

	void DrawDebug();

	uint GetRotatedIndex(uint rect_num, float angle, ROTATION_DIR rot_dir = ROTATION_DIR::COUNTER_CLOCKWISE, float fist_rect_dir = -90);


	bool LoadRects(pugi::xml_node const &node, SDL_Rect * rects);

	bool LoadAnimation(pugi::xml_node &node, Animation &anim);

	int GetHitPoints() const;

	void SetDamage(float damage);
	void ReduceHitPoints(const int& hit_points);

public:


	ObjectType type = ObjectType::NO_TYPE;
	fPoint pos_map		= { 0.f, 0.f };//The position in the isometric grid. Use app->map->MapToScreenF() to get the position in which to Blit() the object.
	fPoint pos_screen	= { 0.f, 0.f };//The position in the screen. Is measured with pixels. Modifying this value wil have no effect because is overwritten in every frame. Use this instead of calling MapToScreenF.
	fPoint velocity		= { 0.f, 0.f };
	fPoint acceleration = { 0.f, 0.f };
	fPoint pivot		= { 0.f, 0.f };
	iPoint draw_offset	= { 0, 0 };	//Pixels to the center of the player. Used to center the player sprite.

	bool to_remove = false;//Set it to true if you want the object to be removed
	
	Animation* current_animation = nullptr;

	ObjectInfo data;

	Collider* coll = nullptr;

	SDL_Rect frame = {0, 0, 0, 0};

private:
	// Different than life in Obj_Tank.h. This is used when an object is attacked and no colliders are necessary.
	int hit_points = 100;
};

#endif
