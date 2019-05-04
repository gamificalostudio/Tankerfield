#include <cstdlib>
#include <ctime>
#include <string>

#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_UI.h"
#include "M_Fonts.h"
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
#include "M_MainMenu.h"
#include "Point.h"
#include "Brofiler/Brofiler.h"
#include "Rect.h"
#include "Object.h"
#include "M_PickManager.h"
#include "PerfTimer.h"
#include "Obj_Tank.h"
#include "M_RewardZoneManager.h"
#include "General_HUD.h"
#include "Player_GUI.h"

#include "Obj_TeslaTrooper.h"
#include "Obj_Brute.h"
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

	// Rounds to win the game
	rounds_to_win = config.child("rounds_to_win").attribute("value").as_int();

	time_round_check_frequency = config.child("time_round_check_frequency").attribute("value").as_float();

	// Wave System setup
	time_between_rounds = config.child("time_between_rounds").attribute("value").as_int();
	
	main_music = config.child("music").child("main_music").attribute("music").as_string();

	finish_wave_sound_string = config.child("sounds").child("finish_wave_shot").attribute("sound").as_string();
	wind_sound_string = config.child("sounds").child("wind_sound").attribute("sound").as_string();

	srand(time(NULL));

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

	// Load Fxs
	finish_wave_sound_uint = app->audio->LoadFx(finish_wave_sound_string);
	wind_sound_uint = app->audio->LoadFx(wind_sound_string);
	
	// UI Elements
	std::list<ObjectGroup*>::iterator players_layer = app->map->data.object_layers.begin();
	for (; players_layer != app->map->data.object_layers.end(); ++players_layer)
	{
		if ((*players_layer)->name == "Players")
		{
			break;
		}
	}
	if (players_layer == app->map->data.object_layers.end() || (*players_layer)->size != 4)
	{
		//Create all tanks
		app->objectmanager->obj_tanks[0] = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(0.f, 0.f));
		app->objectmanager->obj_tanks[1] = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(21.5f, 13.5f));
		app->objectmanager->obj_tanks[2] = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(11.5f, 22.5f));
		app->objectmanager->obj_tanks[3] = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, fPoint(22.5f, 22.5f));
	}
	else
	{
		//Create all tanks
		app->objectmanager->obj_tanks[0] = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, (*players_layer)->objects[0].pos);
		app->objectmanager->obj_tanks[1] = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, (*players_layer)->objects[1].pos);
		app->objectmanager->obj_tanks[2] = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, (*players_layer)->objects[2].pos);
		app->objectmanager->obj_tanks[3] = (Obj_Tank*)app->objectmanager->CreateObject(ObjectType::TANK, (*players_layer)->objects[3].pos);
	}
	
	general_hud = DBG_NEW General_HUD();

	round = 0u;
	stat_of_wave = WaveStat::EXIT_OF_WAVE;
	game_over = false;

	app->render->SetTankCameras();//TO IMPROVE: Each tank should create its own camera, not depend on this function to create it

	return true;
}

// Called each loop iteration
bool M_Scene::PreUpdate()
{
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		app->scmanager->FadeToBlack(this, app->main_menu, 1.f, 1.f );
		if (SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE)
		{
			SDL_ShowCursor(SDL_ENABLE);
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_F5) == KeyState::KEY_DOWN)
	{
		app->scmanager->FadeToBlack(this, this, 2.f, 2.f);
	}

	if (app->input->controllers.size())
	{
		control1 = &(*app->input->controllers.begin());
	}

	iPoint mouse_pos;
	app->input->GetMousePosition(mouse_pos.x, mouse_pos.y);
	mouse_pos = app->render->ScreenToWorld(mouse_pos.x, mouse_pos.y, (*app->render->cameras.begin()));
	mouse_pos = app->map->ScreenToMapI(mouse_pos.x, mouse_pos.y);

	if (app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER, (fPoint)mouse_pos);
	}
	if (app->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		app->objectmanager->CreateObject(ObjectType::BRUTE, (fPoint)mouse_pos);
	}
	if (app->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		general_hud->SetRoundNumber(2);
	}
	if (app->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		app->objectmanager->CreateObject(ObjectType::PICK_UP, (fPoint)mouse_pos);
	}
	if (app->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
	{
		app->objectmanager->obj_tanks[0]->SetLife(0);
		app->objectmanager->obj_tanks[1]->SetLife(0);
		app->objectmanager->obj_tanks[2]->SetLife(0);
		app->objectmanager->obj_tanks[3]->SetLife(0);
	}
	if (app->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
	{
		iPoint mouse_pos;
		app->input->GetMousePosition(mouse_pos.x, mouse_pos.y);
		mouse_pos += iPoint(app->render->cameras[0]->rect.x, app->render->cameras[0]->rect.y);
		fPoint map_mouse_pos = app->map->ScreenToMapF(mouse_pos.x, mouse_pos.y);
		app->objectmanager->CreateObject(ObjectType::EXPLOSION, map_mouse_pos);
	}
	if (app->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)
	{
		++this->round;
	}
	if (app->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
	{
		stat_of_wave = WaveStat::WIN_GAME;
	}

	return true;
}

// Called each loop iteration
bool M_Scene::Update(float dt)
{
	BROFILER_CATEGORY("M_SceneUpdate", Profiler::Color::Blue)
	

	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		draw_debug = !draw_debug;




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
		++round;
		NewWave();
		stat_of_wave = WaveStat::IN_WAVE;
		app->audio->PlayMusic(main_music, 2.0f);
		app->audio->PauseFx(finish_wave_sound_channel, 2000);
		app->audio->PauseFx(wind_sound_channel, 2000);
		break;
	}
	case WaveStat::IN_WAVE:
	{
		for (std::list<Object*>::iterator iterator = enemies_in_wave.begin(); iterator != enemies_in_wave.end();)
		{
			if ((*iterator)->to_remove)
			{
				iterator = enemies_in_wave.erase(iterator);
			}
				
			else
			{
				++iterator;
			}
		}
		
		if (enemies_in_wave.size() == 0)
		{
			stat_of_wave = WaveStat::EXIT_OF_WAVE;
		}
		break;
	}
	case WaveStat::EXIT_OF_WAVE:
	{
		//feedback here I guess(animation and souds)
		timer_between_waves.Start();
		app->audio->PauseMusic(3000);
		finish_wave_sound_channel = app->audio->PlayFx(finish_wave_sound_uint);
		wind_sound_channel = app->audio->PlayFx(wind_sound_uint);
		
		stat_of_wave = WaveStat::OUT_WAVE;

		break;
	}
	case WaveStat::OUT_WAVE:
		if (timer_between_waves.ReadMs() >= time_between_rounds || AllPlayersReady())
		{
			stat_of_wave = WaveStat::ENTER_IN_WAVE;
		}
		break;

	case WaveStat::GAME_OVER:
		app->objectmanager->obj_tanks[0]->gui->Fade_GUI(false);
		app->objectmanager->obj_tanks[1]->gui->Fade_GUI(false);
		app->objectmanager->obj_tanks[2]->gui->Fade_GUI(false);
		app->objectmanager->obj_tanks[3]->gui->Fade_GUI(false);
		general_hud->FadeGeneralHUD(false);
		general_hud->FadeGameOverScreen(true, round);
		stat_of_wave = WaveStat::NO_TYPE;
		break;

	case WaveStat::WIN_GAME:
		
		app->objectmanager->obj_tanks[0]->gui->Fade_GUI(false);
		app->objectmanager->obj_tanks[1]->gui->Fade_GUI(false);
		app->objectmanager->obj_tanks[2]->gui->Fade_GUI(false);
		app->objectmanager->obj_tanks[3]->gui->Fade_GUI(false);
		general_hud->FadeGeneralHUD(false);
		general_hud->FadeWinScreen(true);
		stat_of_wave = WaveStat::NO_TYPE;

		break;
	}

	if (!game_over
		&& !app->objectmanager->obj_tanks[0]->Alive()
		&& !app->objectmanager->obj_tanks[1]->Alive()
		&& !app->objectmanager->obj_tanks[2]->Alive()
		&& !app->objectmanager->obj_tanks[3]->Alive())
	{
		stat_of_wave = WaveStat::GAME_OVER;
		game_over = true;
	}
	
	return true;
}

// Called each loop iteration
bool M_Scene::PostUpdate(float dt)
{
	bool ret = true;
	//
	//DebugPathfinding();

	/* Keep track if we reached the maximum round and, therefore, win the game */
	this->accumulated_time += dt * 1000.0f;
	if (accumulated_time >= time_round_check_frequency * 1000.0f)
	{
		perform_round_check = true;
	}


	if (perform_round_check
		&& this->round >= rounds_to_win + 1
		&& !win_game)
	{
		stat_of_wave = WaveStat::WIN_GAME;
		win_game = true;
	}
	return ret;
}

// Called before quitting
bool M_Scene::Reset()
{
	app->audio->PauseMusic(2.f);
	app->map->Unload();
	app->ui->Reset();
	app->objectmanager->Reset();
	app->collision->Reset();

	return true;
}

bool M_Scene::CleanUp()
{
	LOG("Freeing scene");
	app->audio->PauseMusic(2.f);

	RELEASE(general_hud);

	general_hud = nullptr;
	app->objectmanager->obj_tanks[0]->gui = nullptr;
	app->objectmanager->obj_tanks[1]->gui = nullptr;
	app->objectmanager->obj_tanks[2]->gui = nullptr;
	app->objectmanager->obj_tanks[3]->gui = nullptr;

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
		iPoint p = app->render->ScreenToWorld(mousePos.x, mousePos.y, (*app->render->cameras.begin()));
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
					
					for (item_cam = app->render->cameras.begin(); item_cam != app->render->cameras.end(); ++item_cam)
					{
						SDL_RenderSetClipRect(app->render->renderer, &(*item_cam)->screen_section);
					app->render->Blit(path_tex, pos.x + path_tex_offset.x, pos.y + path_tex_offset.y,(*item_cam));
					}
					SDL_RenderSetClipRect(app->render->renderer, nullptr);
				}
			}

		}

		p = app->map->MapToScreenI(p.x, p.y);

		for (item_cam = app->render->cameras.begin(); item_cam != app->render->cameras.end(); ++item_cam)
		{
			SDL_RenderSetClipRect(app->render->renderer, &(*item_cam)->screen_section);
			app->render->Blit(path_tex, p.x + path_tex_offset.x, p.y + path_tex_offset.y, (*item_cam));
		}SDL_RenderSetClipRect(app->render->renderer, nullptr);
	}
}

void M_Scene::CreateEnemyWave()
{
	for (int i = 0; i < Tesla_trooper_units; i++)
	{
		if (app->map->data.spawners_position_enemy.size() != 0)
		{
			uint spawner_random = rand() % app->map->data.spawners_position_enemy.size();
			fPoint pos = app->map->data.spawners_position_enemy.at(spawner_random)->pos;
			Obj_TeslaTrooper* ret = (Obj_TeslaTrooper*)app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER, pos);

			enemies_in_wave.push_back(ret);
		}
	
	}
	if (Brute_units > 0)
	{
		for (int i = 0; i < Brute_units; i++)
		{
			if (app->map->data.spawners_position_enemy.size() != 0)
			{
				uint spawner_random = rand() % app->map->data.spawners_position_enemy.size();
				fPoint pos = app->map->data.spawners_position_enemy.at(spawner_random)->pos;
				Obj_Brute* ret = (Obj_Brute*)app->objectmanager->CreateObject(ObjectType::BRUTE, pos);

				enemies_in_wave.push_back(ret);
			}
		}
	}

}

void M_Scene::NewWave()
{
	if (round == 1)
	{
		Tesla_trooper_units = 100 + (round - 1) * 4 * 6;/*the * 4 is because is coop */
		Brute_units = (round - 5) * 3;
	}
	else if (round >1 && round <= 5)
	{
		Tesla_trooper_units = 300 + (round - 1) * 4 * 6;/*the * 4 is because is coop */
		Brute_units = (round -1) * 5;
	}
	else 
	{
		Tesla_trooper_units = (round * 18 + round * 14) * 6;
		Brute_units = round * 2;
	}
	CreateEnemyWave();
	app->pick_manager->CreateRewardBoxWave();
	general_hud->SetRoundNumber(round);
}

bool M_Scene::AllPlayersReady() const
{
	return (app->objectmanager->obj_tanks[0]->IsReady()
		&&  app->objectmanager->obj_tanks[1]->IsReady()
		&&  app->objectmanager->obj_tanks[2]->IsReady()
		&&  app->objectmanager->obj_tanks[3]->IsReady());
}