#ifndef __M_SCENEMANAGER_H__
#define __M_SCENEMANAGER_H__

#include <vector>
#include "Module.h"
#include "SDL\include\SDL_rect.h"

class M_SceneManager : public Module
{
public:
	M_SceneManager();
	~M_SceneManager();

	bool Start() override;
	bool PostUpdate(float dt);
	bool FadeToBlack(Module* module_off, Module* module_on, float seconds_fade_to  = 2.f, float seconds_fade_from  = 2.f,  std::vector<Module*> modules_to_reset = std::vector<Module*>() );

private:

	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	float aux = 0.f;
	float target_value = 255.f;
	float init_value = 0.f;
	float rate_time_fade_to = 0.f;
	float rate_time_fade_from = 0.f;
	float balck_rect_alpha = 0.f;


	SDL_Rect screen = { 0,0,0,0 };
	Module* module_off = nullptr;
	Module* module_on = nullptr;
	std::vector<Module*> modules_to_reset;

};

#endif //__SCENEMANAGER_H__