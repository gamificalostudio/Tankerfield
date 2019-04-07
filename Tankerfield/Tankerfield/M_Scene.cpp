#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_Input.h"
#include "M_Textures.h"
#include "M_Audio.h"
#include "M_Render.h"
#include "M_Window.h"
#include "M_Scene.h"
#include "M_SceneManager.h"
#include "M_Map.h"
#include "M_Pathfinding.h"
#include "M_ObjManager.h"
#include "M_Collision.h"
#include "Point.h"
#include "Brofiler/Brofiler.h"
#include "Rect.h"
#include "Obj_Tank.h"

M_Scene::M_Scene() : Module()
{
	name.assign("scene");
}

// Destructor
M_Scene::~M_Scene()
{}

// Called before render is available
bool M_Scene::Awake(pugi::xml_node&)
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool M_Scene::Start()
{
	path_tex = app->tex->Load("maps/path.png");

	// Load the first level of the list on first game start -------------------------
	std::list<Levels*>::iterator levelData = app->map->levels.begin();
	std::advance(levelData, current_level);
	app->map->Load((*levelData)->name.c_str());

	//app->audio->PlayMusic("audio/Music/indeep.ogg",0.0f);

	//app->objectmanager->CreateObject(ObjectType::REWARD_ZONE, fPoint(3.f, 3.f));
	//app->objectmanager->CreateObject(ObjectType::REWARD_ZONE, fPoint(6.f, 6.f));
	tank_1 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(20.f, 20.f));
	tank_2 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(30.f, 30.f));
	tank_3 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(40.f, 40.f));
	tank_4 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(50.f, 50.f));

	tank_1->camera_player->number_player = 1;
	tank_2->camera_player->number_player = 2;
	tank_3->camera_player->number_player = 3;
	tank_4->camera_player->number_player = 4;

	return true;
}

// Called each loop iteration
bool M_Scene::PreUpdate()
{
	if (app->input->controllers.size())
	{
		control1 = &(*app->input->controllers.begin());
	}

	iPoint mouse_pos;
	app->input->GetMousePosition(mouse_pos.x, mouse_pos.y);
	mouse_pos = app->render->ScreenToWorld(mouse_pos.x, mouse_pos.y);
	mouse_pos = app->map->ScreenToMapI(mouse_pos.x, mouse_pos.y);
	if (app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER, (fPoint)mouse_pos);
	}

	return true;
}

// Called each loop iteration
bool M_Scene::Update(float dt)
{
	BROFILER_CATEGORY("M_SceneUpdate", Profiler::Color::Blue)
	/*if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		app->render->camera.y -= floor(200.0f * dt);
	}
	if(app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		app->render->camera.y += floor(200.0f * dt);
	}
	if(app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		app->render->camera.x -= floor(200.0f * dt);
	}
	if(app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		app->render->camera.x += floor(200.0f * dt);
	}*/

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KeyState::KEY_DOWN)
	{
		++current_level;

		if (current_level == app->map->GetMaxLevels())
			current_level = 0;

		app->scmanager->FadeToBlack(app->scene, app->scene, 1.F);
	}
	return true;
}

// Called each loop iteration
bool M_Scene::PostUpdate(float dt)
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	
	DebugPathfinding();

	return ret;
}

// Called before quitting
bool M_Scene::CleanUp()
{
	LOG("Freeing scene");
	app->map->Unload();
	app->objectmanager->DeleteObjects();

	if (path_tex != nullptr)
		app->tex->UnLoad(path_tex);

	return true;
}

void M_Scene::DebugPathfinding()
{
	if (test_path)
	{
		static iPoint origin;
		static bool origin_selected = false;
		static bool createdDebugPath = false;

		iPoint mousePos;
		app->input->GetMousePosition(mousePos.x, mousePos.y);
		iPoint p = app->render->ScreenToWorld(mousePos.x, mousePos.y);
		p = app->map->ScreenToMapI(p.x, p.y);

		if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN)
		{
			if (origin_selected == true)
			{
				origin_selected = false;

				if (app->pathfinding->CreatePath(origin, p) != -1)
				{
					createdDebugPath = true;
				}

			}
			else
			{
				origin = p;
				origin_selected = true;
				createdDebugPath = false;
				debug_path.clear();
			}
		}

		if (createdDebugPath)
		{
			uint debugPathSize = debug_path.size();
			if (debugPathSize == 0)
			{

				const std::vector<iPoint>* path = app->pathfinding->GetLastPath();
				uint sizeArray = path->size();
				for (uint i = 0; i < sizeArray; ++i)
				{
					debug_path.push_back(path->at(i));
				}
			}
			else
			{
				for (uint i = 0; i < debugPathSize; ++i)
				{
					iPoint pos = app->map->MapToScreenI(debug_path.at(i).x, debug_path.at(i).y);
					for (std::vector<Camera*>::iterator item_cam = app->render->camera.begin(); item_cam != app->render->camera.end(); item_cam++)
					{
						app->render->Blit(path_tex, pos.x + path_tex_offset.x, pos.y + path_tex_offset.y, (*item_cam));
					}
				}
			}

		}

		p = app->map->MapToScreenI(p.x, p.y);
		for (std::vector<Camera*>::iterator item_cam = app->render->camera.begin(); item_cam != app->render->camera.end(); item_cam++)
		{
			app->render->Blit(path_tex, p.x + path_tex_offset.x, p.y + path_tex_offset.y, (*item_cam));
		}
	}
}
