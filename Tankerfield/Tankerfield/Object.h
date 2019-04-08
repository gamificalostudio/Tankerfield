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

	void DrawDebug();

	void SetDamage(float damage);

public:


	ObjectType type = ObjectType::NO_TYPE;
	fPoint pos_map		= { 0.f, 0.f };//The position in the isometric grid. Use app->map->MapToScreenF() to get the position in which to Blit() the object.
	fPoint pos_screen	= { 0.f, 0.f };//The position in the screen. Is measured with pixels. Modifying this value wil have no effect because is overwritten in every frame. Use this instead of calling MapToScreenF.
	fPoint velocity		= { 0.f, 0.f };
	fPoint acceleration = { 0.f, 0.f };
	bool to_remove = false;//Set it to true if you want the object to be removed
	ObjectInfo data;
	Collider* coll = nullptr;
	float angle				= 0.f;//Direction that the object is facing
	SDL_Texture * curr_tex	= nullptr;			//Points the current texture. Shouldn't allocate memory. Just assign the pointer to other textures already created. Used in Object::PostUpdate(float dt)
	Animation * curr_anim	= nullptr;			//Points the current animation. Shouldn't allocate memory. Just assign the pointer to other animations already created. Used in Object::PostUpdate(float dt) and for sprite sorting
	iPoint draw_offset		= { 0.f, 0.f };		//Change it to make the object not render from the top left in the position //Pixels to the center of the player. Used to center the player sprite. Used in Object::PostUpdate(float dt) and for sprite sorting
	fPoint pivot			= { 0.f, 0.f };		//Offset from the position. Used for sprite sorting.
	SDL_Rect frame			= { 0, 0, 0, 0 };	//A rect representing the sprite. Used for camera culling. Automatically set if you use curr_anim.
};

#endif
