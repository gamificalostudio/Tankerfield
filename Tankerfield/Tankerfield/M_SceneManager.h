#ifndef __SCENEMANAGER_H__
#define __SCENEMANAGER_H__

#include "Module.h"
#include "SDL\include\SDL_rect.h"

class SceneManager : public Module
{
public:
	SceneManager();
	~SceneManager();

	bool Start();
	bool Update(float dt);
	bool FadeToBlack(Module* module_off, Module* module_on,float time = 2.0f);

private:

	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen;
	Module* ModuleOff = nullptr;
	Module* ModuleOn = nullptr;
};

#endif //__SCENEMANAGER_H__