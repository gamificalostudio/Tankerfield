#include <iostream>
#include <sstream>

#include "SDL/include/SDL_timer.h"
#include "Brofiler/Brofiler.h"
//#pragma comment(lib, "Brofiler/ProfilerCore32.lib")

#include "Defs.h"
#include "Log.h"

#include "App.h"
#include "M_Window.h"
#include "M_Input.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "M_Audio.h"
#include "M_Scene.h"
#include "M_Pathfinding.h"
#include "M_SceneManager.h"
#include "M_Map.h"
#include "M_Fonts.h"
#include "M_UI.h"
#include "M_ObjManager.h"
#include "M_Collision.h"
#include "M_PickManager.h"
#include "M_AnimationBank.h"
#include "M_RewardZoneManager.h"
#include "M_MainMenu.h"
#include "M_Debug.h"
#include "Options_Menu.h"

// Constructor
App::App(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(ptimer);


	input = DBG_NEW M_Input();
	win = DBG_NEW M_Window();
	render = DBG_NEW M_Render();
	tex = DBG_NEW M_Textures();
	audio = DBG_NEW M_Audio();
	pathfinding = DBG_NEW M_Pathfinding();
	map = DBG_NEW M_Map();
	scene = DBG_NEW M_Scene();
	font = DBG_NEW M_Fonts();
	ui = DBG_NEW M_UI();
	objectmanager = DBG_NEW M_ObjManager();
	pick_manager = DBG_NEW M_PickManager();
	scmanager = DBG_NEW M_SceneManager();
	collision = DBG_NEW M_Collision();
	anim_bank = DBG_NEW M_AnimationBank();
	reward_zone_manager = DBG_NEW M_RewardZoneManager();
	main_menu = DBG_NEW M_MainMenu();
	debug = DBG_NEW M_Debug();
	// Ordered for awake / Start / Update
	// Reverse order of CleanUp

	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(font);
	AddModule(audio);
	AddModule(pathfinding);
	AddModule(map);
	AddModule(scene);
	AddModule(main_menu);
	AddModule(objectmanager);
	AddModule(pick_manager);
	AddModule(reward_zone_manager);
	AddModule(collision);
	AddModule(ui);
	AddModule(anim_bank);
	AddModule(scmanager);
	AddModule(debug);
	AddModule(render);      // Render last to swap buffer

	PERF_PEEK(ptimer);
}

// Destructor
App::~App()
{
	// release modules
	std::list<Module*>::reverse_iterator item = modules.rbegin();

	while (item != modules.rend())
	{
		RELEASE(*item);
		item++;
	}

	modules.clear();
}

void App::AddModule(Module* module)
{
	modules.push_back(module);
}

// Called before render is available
bool App::Awake()
{
	PERF_START(ptimer);

	bool ret = false;

	config = LoadConfig(config_file);

	if (config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title.assign(app_config.child("title").child_value());
		organization.assign(app_config.child("organization").child_value());

	
		switch (app_config.child("mode").attribute("value").as_int(0))
		{
		case 0:
			mode = APP_MODE::RELEASE;
			scene->active = false;
			break;
		case 1:
			mode = APP_MODE::DEBUG_MULTIPLAYER;
			main_menu->active = false;
			break;
		case 2:
			mode = APP_MODE::DEBUG_MAIN_MENU;
			scene->active = false;
			break;
		}
		debug->enabled = false;

		int cap = app_config.attribute("framerate_cap").as_int(-1);

		if (cap > 0)
		{
			capped_ms = 1000 / cap;
		}
	}

	if (ret == true)
	{
		std::list<Module*>::const_iterator item;
		item = modules.begin();

		while (item != modules.end() && ret == true)
		{
			pugi::xml_node aux = config.child((*item)->name.data());
			ret = (*item)->Awake(aux);
			item++;
		}
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool App::Start()
{
	PERF_START(ptimer);

	bool ret = true;
	
	title.assign("Tankerfield");
	organization.assign("Gamificalo Studio");
	
	int cap = 60;

	if (cap > 0)
	{
		capped_ms = 1000 / cap;
	}

	std::list<Module*>::const_iterator item;
	item = modules.begin();

	while (item != modules.end())
	{
		if ((*item)->active == true)
		{
			(*item)->enabled = true;
			ret = (*item)->Start();
		}
		++item;
	}
	
	startup_time.Start();

	PERF_PEEK(ptimer);

	return ret;
}

// Called each loop iteration
bool App::Update()
{
	bool ret = true;
	PrepareUpdate();
	if (input->GetKey(SDL_SCANCODE_P) == KeyState::KEY_DOWN)
	{
		pause = !pause;
		frame_time.Start();
	}
	if (input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if (ret == true)
		ret = PreUpdate();

	if (ret == true)
		ret = DoUpdate();

	if (ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if (result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void App::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;
	if (pause)
	{
		dt = 0.0F;
	}
	else
	{
		dt = frame_time.ReadSec();

		if (dt > 0.32f)
		{
			dt = 0.32f;
		}

		frame_time.Start();
	}
}

// ---------------------------------------------
void App::FinishUpdate()
{
	if (want_to_save == true)
		SavegameNow();

	if (want_to_load == true)
		LoadGameNow();

	// Framerate calculations --
	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}
	
	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;
	
	static char title[256];
	sprintf_s(title, 256, "Tankerfield | FPS: %u", frames_on_last_update);
	app->win->SetTitle(title);
	
	//if (capped_ms > 0 && last_frame_ms < capped_ms)
	//{
	//	SDL_Delay(capped_ms - last_frame_ms);
	//}
}

// Call modules before each loop iteration
bool App::PreUpdate()
{
	bool ret = true;
	std::list<Module*>::const_iterator item;
	item = modules.begin();
	Module* pModule = NULL;

	for (item = modules.begin(); item != modules.end() && ret == true; item++)
	{
		pModule = *item;

		if (pModule->active == false) {
			continue;
		}

		ret = (*item)->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool App::DoUpdate()
{
	bool ret = true;
	std::list <Module*>::const_iterator item;
	item = modules.begin();
	Module* pModule = NULL;

	for (item = modules.begin(); item != modules.end() && ret == true; item++)
	{
		pModule = *item;

		if (pModule->active == false) {
			continue;
		}

		ret = (*item)->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool App::PostUpdate()
{
	bool ret = true;
	std::list<Module*>::const_iterator item;
	Module* pModule = NULL;

	for (item = modules.begin(); item != modules.end() && ret == true; item++)
	{
		pModule = *item;

		if (pModule->active == false) {
			continue;
		}
		ret = (*item)->PostUpdate(dt);
	}

	return ret;
}

// Called before quitting
bool App::CleanUp()
{
	PERF_START(ptimer);
	bool ret = true;
	std::list<Module*>::reverse_iterator item;
	item = modules.rbegin();
	on_clean_up = true;

	while (item != modules.rend() && ret == true)
	{
		if ((*item)->active == true)
		{
			ret = (*item)->CleanUp();
		}
		item++;
	}

	PERF_PEEK(ptimer);
	return ret;
}

// ---------------------------------------
int App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* App::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* App::GetTitle() const
{
	return title.data();
}

// ---------------------------------------
float App::GetDT() const
{
	return dt;
}

// ---------------------------------------
const char* App::GetOrganization() const
{
	return organization.data();
}


// Load / Save
void App::LoadGame(const char* file)
{
	want_to_load = true;
}

// ---------------------------------------
void App::SaveGame(const char* file) const
{

	want_to_save = true;
	save_game.assign(file);
}

// ---------------------------------------

bool App::LoadGameNow()
{
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.load_file(load_game.data());

	if (result != NULL)
	{
		LOG("Loading new Game State from %s...", load_game.data());

		root = data.child("game_state");

		std::list<Module*>::const_iterator item = modules.begin();
		ret = true;

		while (item != modules.end() && ret == true)
		{
			pugi::xml_node aux = root.child((*item)->name.data());
			ret = (*item)->Load(aux);
			item++;
		}

		data.reset();
		if (ret == true)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", ((*item) != NULL) ? (*item)->name.data() : "unknown");
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", load_game.data(), result.description());

	want_to_load = false;
	return ret;
}

bool App::SavegameNow() const
{
	bool ret = true;

	LOG("Saving Game State to %s...", save_game.data());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	root = data.append_child("game_state");

	std::list<Module*>::const_iterator item = modules.begin();

	while (item != modules.end() && ret == true)
	{
		pugi::xml_node aux = root.append_child((*item)->name.data());
		ret = (*item)->Save(aux);
		item++;
	}

	if (ret == true)
	{
		std::stringstream stream;
		data.save(stream);

		LOG("... finished saving", save_game.data());
	}
	else
		LOG("Save process halted from an error in module %s", ((*item) != NULL) ? (*item)->name.data() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
}
