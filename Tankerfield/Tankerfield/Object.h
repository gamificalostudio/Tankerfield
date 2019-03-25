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
	virtual void Draw(float dt, SDL_Texture* texture);
	virtual bool CleanUp() { return true; };
	virtual bool Awake(pugi::xml_node&) { return true; };

	virtual bool Load(pugi::xml_node&) { return true; };
	virtual bool Save(pugi::xml_node&) const { return true; };
  
	SDL_Rect* GetRotatedSprite(SDL_Rect* rect, int rect_num, float angle, float fist_rect_dir = 90);
	//Same as GetRotatedSprite but with animations
	Animation* GetRotatedAnimation(Animation* animations, int num_animations, float angle, float fist_rect_dir = 90);
  
	bool LoadRects(pugi::xml_node const &node, SDL_Rect * rects);
	bool LoadAnimation(pugi::xml_node &node, Animation &anim);

public:

	int type = 0;
	fPoint pos;
	fPoint velocity;
	fPoint acceleration;
	bool to_remove = false;//Set it to true if you want the object to be removed
	//

	Animation* current_animation = nullptr;
	
	Collider* coll = nullptr;
};

#endif
