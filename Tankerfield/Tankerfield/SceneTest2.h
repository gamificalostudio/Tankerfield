#ifndef __SCENETEST2_H__
#define __SCENETEST2_H__

#include "j1Module.h"
#include "SDL/include/SDL_rect.h"

struct SDL_Texture;

class SceneTest2 : public j1Module
{
public:

	SceneTest2();

	// Destructor
	virtual ~SceneTest2();

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