#include <cstdlib>
#include <ctime>

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
#include "Object.h"

M_Scene::M_Scene() : Module()
{
	name.assign("scene");
}

// Destructor
M_Scene::~M_Scene()
{}

// Called before render is available
bool M_Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	/* Wave System setup */
	time_between_rounds = config.child("time_between_rounds").attribute("value").as_int();
	initial_generated_units = config.child("initial_generated_units").attribute("value").as_int();
	distance_range = config.child("distance_range").attribute("value").as_int();
	min_distance_from_center = config.child("min_distance_from_center").attribute("value").as_int();
	check_complete_round = config.child("check_complete_round").attribute("value").as_int();

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

	app->audio->PlayMusic("audio/Music/indeep.ogg", 0.0f);

	app->objectmanager->CreateObject(ObjectType::REWARD_ZONE, fPoint(3.f, 3.f));
	app->objectmanager->CreateObject(ObjectType::REWARD_ZONE, fPoint(6.f, 6.f));
	tank_1 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(19.f, 19.f));

	//tank_2 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(0.f, 0.f));
	//tank_2 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(1.f, 1.f));
	//app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER, fPoint(1.f, 1.f));

	//app->objectmanager->CreateObject(ObjectType::STATIC, fPoint(7.55f, 4.f));

	/* Generate first wave units */
	srand(time(NULL));
	CreateEnemyWave();

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
	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
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
	}

	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		draw_debug = !draw_debug;

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KeyState::KEY_DOWN)
	{
		++current_level;

		if (current_level == app->map->GetMaxLevels())
			current_level = 0;

		app->scmanager->FadeToBlack(app->scene, app->scene, 1.F);
	}

	/* Check if a round is over. It is only checked after x time. */
	accumulated_time += dt * 1000;
	if (accumulated_time >= (float)check_complete_round)
	{
		perform_objects_check = true;
		initial_generated_units = 12;
	}

	if (perform_objects_check)
	{
		// == 3 because of the objects that are not enemies. Possible solution 2: check the type of objects with counters and check
		if (app->objectmanager->GetObjects().size() == 3) // TOFIX: Here we are checking objects of type static I think too...
		{
			/* Generate new wave */
			CreateEnemyWave();
		}

		accumulated_time = 0.0f;
		perform_objects_check = false;
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
					app->render->Blit(path_tex, pos.x + path_tex_offset.x, pos.y + path_tex_offset.y);
				}
			}

		}

		p = app->map->MapToScreenI(p.x, p.y);

		app->render->Blit(path_tex, p.x + path_tex_offset.x, p.y + path_tex_offset.y);
	}
}

void M_Scene::CreateEnemyWave()
{
	for (int i = 0; i < initial_generated_units; i++)
	{
		//iPoint random_tile_position = { -10 + rand() % 21, -10 + rand() % 21 };
		iPoint random_tile_position = { rand() % (distance_range * 2 + 1) - distance_range,
			rand() % (distance_range * 2 + 1) - distance_range };

		// TODO: At this point, we know the map columns / rows -> 40. We must get these values without magic numbers.
		int map_rows = 40, map_columns = 40;

		if (random_tile_position.x >= 0 && random_tile_position.y >= 0)
		{
			app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER,
				fPoint(map_rows / 2 + (float)random_tile_position.x + (float)min_distance_from_center,
					map_columns / 2 + (float)random_tile_position.y + (float)min_distance_from_center));
		}
		else if (random_tile_position.x < 0 && random_tile_position.y < 0)
		{
			app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER,
				fPoint(map_rows / 2 + (float)random_tile_position.x - (float)min_distance_from_center,
					map_columns / 2 + (float)random_tile_position.y - (float)min_distance_from_center));
		}
		else if (random_tile_position.x >= 0 && random_tile_position.y < 0)
		{
			app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER,
				fPoint(map_rows / 2 + (float)random_tile_position.x + (float)min_distance_from_center,
					map_columns / 2 + (float)random_tile_position.y - (float)min_distance_from_center));
		}
		else if (random_tile_position.x < 0 && random_tile_position.y >= 0)
		{
			app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER,
				fPoint(map_rows / 2 + (float)random_tile_position.x - (float)min_distance_from_center,
					map_columns / 2 + (float)random_tile_position.y + (float)min_distance_from_center));
		}
	}
}
