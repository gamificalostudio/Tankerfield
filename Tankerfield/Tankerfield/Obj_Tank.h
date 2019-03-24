#include "Object.h"

class Obj_Tank : public Object
{
public:
	Obj_Tank();
	~Obj_Tank();

public:
	bool Awake(pugi::xml_node & tank_node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

private:

public:

private:
	static SDL_Texture * base_tex;
	static SDL_Texture * turr_tex;
	static SDL_Texture * base_shadow_tex;
	static SDL_Texture * turr_shadow_tex;

	static SDL_Rect * base_rects;
	static SDL_Rect * turr_rects;

	float speed = 0.f;
};