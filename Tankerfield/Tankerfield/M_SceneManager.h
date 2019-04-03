#ifndef __M_SCENEMANAGER_H__
#define __M_SCENEMANAGER_H__

#include "Module.h"
#include "Rect.h"

class M_SceneManager : public Module
{
public:
	M_SceneManager();
	~M_SceneManager();

	bool Start() override;
	bool Update(float dt) override;
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
	iRect screen;
	Module* ModuleOff = nullptr;
	Module* ModuleOn = nullptr;
};

#endif //__SCENEMANAGER_H__