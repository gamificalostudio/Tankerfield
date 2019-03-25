#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "SDL/include/SDL_rect.h"

struct SDL_Texture;
struct Controller;

class j1Scene : public j1Module
{
private:
	Controller** control1 = nullptr;
	SDL_Rect	cube = {100,100,10,10};
public:
	int current_level = 0;
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

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