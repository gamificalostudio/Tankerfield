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
#include "M_PickManager.h"
#include "PerfTimer.h"
#include "Obj_Tank.h"

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
	enemies_to_increase = config.child("enemies_to_increase").attribute("value").as_int();

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


	tank_1 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(70.f, 60.f));
	tank_2 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(21.5f, 13.5f));
	tank_3 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(11.5f, 22.5f));
	tank_4 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(22.5f, 22.5f));

	app->objectmanager->CreateObject(ObjectType::STATIC, fPoint(6.f, 8.f));
	//app->objectmanager->CreateObject(ObjectType::PICK_UP, fPoint(12.5f, 14.5f));

	//tank_2 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(0.f, 0.f));
	//tank_2 = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(1.f, 1.f));
	//app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER, fPoint(1.f, 1.f));

	//app->objectmanager->CreateObject(ObjectType::STATIC, fPoint(7.55f, 4.f));

	/* Generate first wave units */
	srand(time(NULL));
	//CreateEnemyWave();
	number_current_wave = 1;
	stat_of_wave = WaveStat::EXIT_OF_WAVE;

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
	

	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		draw_debug = !draw_debug;

	/*if (app->input->GetKey(SDL_SCANCODE_RETURN) == KeyState::KEY_DOWN)
	{
		++current_level;

		if (current_level == app->map->GetMaxLevels())
			current_level = 0;

		app->scmanager->FadeToBlack(app->scene, app->scene, 1.F);
	}*/

	/* Check if a round is over. It is only checked after x time. */
	//accumulated_time += dt * 1000.0f;
	//if (accumulated_time >= (float)check_complete_round)
	//{
	//	perform_objects_check = true;
	//}

	//if (perform_objects_check)
	//{
	//	

	//	accumulated_time = 0.0f;
	//	perform_objects_check = false;
	//}

	switch (stat_of_wave)
	{
	case WaveStat::ENTER_IN_WAVE:
	{
		/* Generate new wave, restart the vars and increase units number */
		NewWave();
		stat_of_wave = WaveStat::IN_WAVE;
		break;
	}
	case WaveStat::IN_WAVE:
	{
		if (enemies_dead == initial_generated_units)
		{
			stat_of_wave = WaveStat::EXIT_OF_WAVE;
		}
		break;
	}
	case WaveStat::EXIT_OF_WAVE:
	{
		//feedback here I guess(animation and souds)
		timer_between_waves.Start();
		stat_of_wave = WaveStat::OUT_WAVE;

		break;
	}
	case WaveStat::OUT_WAVE:
		if (timer_between_waves.ReadMs() >= time_between_rounds || AllPlayersReady())
		{
			stat_of_wave = WaveStat::ENTER_IN_WAVE;
		}

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
		std::vector<Camera*>::iterator item_cam;
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
					
					for (item_cam = app->render->camera.begin(); item_cam != app->render->camera.end(); ++item_cam)
					{
						SDL_RenderSetClipRect(app->render->renderer, &(*item_cam)->viewport);
					app->render->Blit(path_tex, pos.x + path_tex_offset.x, pos.y + path_tex_offset.y,(*item_cam));
					}
					SDL_RenderSetClipRect(app->render->renderer, nullptr);
				}
			}

		}

		p = app->map->MapToScreenI(p.x, p.y);

		for (item_cam = app->render->camera.begin(); item_cam != app->render->camera.end(); ++item_cam)
		{
			SDL_RenderSetClipRect(app->render->renderer, &(*item_cam)->viewport);
			app->render->Blit(path_tex, p.x + path_tex_offset.x, p.y + path_tex_offset.y, (*item_cam));
		}SDL_RenderSetClipRect(app->render->renderer, nullptr);
	}
}

void M_Scene::CreateEnemyWave()
{
	/*for (int i = 0; i < initial_generated_units; i++)
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
	}*/
	for (int i = 0; i < initial_generated_units; i++)
	{
		uint spawner_random = rand() % app->map->data.spawners_position_enemy.size();
		fPoint pos = app->map->data.spawners_position_enemy.at(spawner_random)->pos;
		app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER,
			pos);
		
	}

}

void M_Scene::NewWave()
{
	initial_generated_units += enemies_to_increase;

	CreateEnemyWave();
	app->pick_manager->CreateRewardBoxWave();

	++number_current_wave;
	enemies_dead = 0u;
}

bool M_Scene::AllPlayersReady() const
{
	return (tank_1->IsReady()
		&& tank_2->IsReady()
		&& tank_3->IsReady()
		&& tank_4->IsReady());
}