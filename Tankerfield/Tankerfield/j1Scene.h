#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "SDL/include/SDL_rect.h"

struct SDL_Texture;

class j1Scene : public j1Module
{
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


	SDL_Rect* Direction(SDL_Rect* rect, int rect_num, float angle);
private:

	SDL_Rect * tesla_trooper_rects = nullptr;

	SDL_Texture* texture = nullptr;
	SDL_Rect player = { 0,0 };
	std::list<SDL_Rect*>	rects;
};

#endif // __j1SCENE_H__