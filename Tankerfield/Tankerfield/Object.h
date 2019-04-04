#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <list>

#include "PugiXml\src\pugixml.hpp"
#include "Point.h"
#include "Animation.h"

#include "PerfTimer.h"

enum ROTATION_DIR {
	CLOCKWISE,
	COUNTER_CLOCKWISE,
	INVALID
};

struct SDL_Texture;
class Collider;

struct TileSetEntity {

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
	TileSetEntity tileset;
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
	virtual bool CleanUp() { return true; };
	virtual bool Awake(pugi::xml_node&) { return true; };

	virtual bool Load(pugi::xml_node&) { return true; };
	virtual bool Save(pugi::xml_node&) const { return true; };

	//Clamps the rotation from 0 to 360 degrees
	float ClampRotation(float angle);
	uint GetRotatedIndex(uint rect_num, float angle, ROTATION_DIR rot_dir = ROTATION_DIR::COUNTER_CLOCKWISE, float fist_rect_dir = 90);

	void SetPivot(const float &x, const float &y);
  
	bool LoadRects(pugi::xml_node const &node, SDL_Rect * rects);
	bool LoadAnimation(pugi::xml_node &node, Animation &anim);

public:

	int type = 0;
	fPoint pos_map		= { 0.f, 0.f };//The position in the isometric grid. Use app->map->MapToScreenF() to get the position in which to Blit() the object.
	fPoint velocity		= { 0.f, 0.f };
	fPoint acceleration = { 0.f, 0.f };
	fPoint pivot		= { 0.f, 0.f };
	iPoint size			= { 0, 0 };
	bool to_remove = false;//Set it to true if you want the object to be removed
	//
	Animation* current_animation = nullptr;

	ObjectInfo data;

	Collider* coll = nullptr;
};

#endif
