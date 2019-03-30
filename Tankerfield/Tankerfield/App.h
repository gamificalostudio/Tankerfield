#ifndef __j1APP_H__
#define __j1APP_H__

#include <list>
#include <string>

#include "PugiXml/src/pugixml.hpp"

#include "Module.h"
#include "j1PerfTimer.h"
#include "j1Timer.h"



class M_Window;
class M_Input;
class M_Render;
class M_Textures;
class M_Audio;
class M_UITest;
class M_Pathfinding;
class M_Map;
class M_Fonts;
class M_UI;
class M_ObjManager;
class M_SceneManager;
class M_Scene;

class App
{
public:
	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;
	float GetDT() const;

	void LoadGame(const char* file);
	void SaveGame(const char* file) const;

private:
	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;

public:
	// Modules
	M_Window*			win = nullptr;
	M_Input*			input = nullptr;
	M_Render*			render = nullptr;
	M_Textures*			tex = nullptr;
	M_Audio*			audio = nullptr;
	M_UITest*			ui_test = nullptr;
	M_Pathfinding*		pathfinding = nullptr;
	M_Map*				map = nullptr;
	M_Fonts*            font = nullptr;
	M_UI*          ui = nullptr;
	M_ObjManager*		objectmanager = nullptr;
	M_SceneManager*		scmanager = nullptr;
	M_Scene*			scene = nullptr;
  
	//XML document
	pugi::xml_node		config;

private:
	std::list<Module*>	modules;
	int					argc;
	char**				args;

	std::string			title;
	std::string			organization;

	mutable bool		want_to_save = false;
	bool				want_to_load = false;
	std::string			load_game;
	mutable std::string	save_game;

	j1PerfTimer			ptimer;
	uint64				frame_count = 0;
	j1Timer				startup_time;
	j1Timer				frame_time;
	j1Timer				last_sec_frame_time;
	uint32				last_sec_frame_count = 0;
	uint32				prev_last_sec_frame_count = 0;
	float				dt = 0.0f;
	int					capped_ms = -1;

	//XML document
	pugi::xml_document	config_file;
	pugi::xml_node		app_config;
};

extern App* app;

#endif