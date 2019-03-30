#ifndef __M_SCENE_H__
#define __M_SCENE_H__

#include "Module.h"
#include "SDL/include/SDL_rect.h"

struct SDL_Texture;
struct Controller;

class M_Scene : public Module
{
private:
	Controller** control1 = nullptr;
	
public:
	int current_level = 0;
public:

	M_Scene();

	// Destructor
	virtual ~M_Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

private:

};

#endif // __j1SCENE_H__