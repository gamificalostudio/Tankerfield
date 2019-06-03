#include <cstdlib>
#include <ctime>
#include <string>

#include "Brofiler/Brofiler.h"

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
#include "M_PickManager.h"
#include "M_RewardZoneManager.h"
#include "LeaderBoard.h"

#include "UI_Label.h"

#include "Point.h"
#include "Rect.h"
#include "Object.h"
#include "PerfTimer.h"
#include "Obj_Tank.h"

#include "General_HUD.h"
#include "Player_GUI.h"

#include "Obj_TeslaTrooper.h"
#include "Obj_Brute.h"
#include "Obj_RocketLauncher.h"
#include "Obj_Suicidal.h"

#include "Object.h"
#include "Obj_RewardBox.h"


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

	//MUSIC
	main_music = config.child("music").child("main_music").attribute("music").as_string();

	finish_wave_sound_string = config.child("sounds").child("finish_wave_shot").attribute("sound").as_string();
	wind_sound_string = config.child("sounds").child("wind_sound").attribute("sound").as_string();

	srand(time(NULL));

	return ret;
}

// Called before the first frame
bool M_Scene::Start()
{
	//path_tex = app->tex->Load("maps/path.png");

	// Load Fxs
	finish_wave_sound_uint = app->audio->LoadFx(finish_wave_sound_string);
	wind_sound_uint = app->audio->LoadFx(wind_sound_string);

	//Create map quadtrees (need cameras to be created first and cameras are created inside the tank's constructor)
	// Load the first level of the list on first game start -------------------------
	std::list<Levels*>::iterator levelData = app->map->levels.begin();
	std::advance(levelData, current_level);
	app->map->Load((*levelData)->name.c_str());


	//Set pos players
	{
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

		app->objectmanager->obj_tanks[0]->SetColor(tank_colors[0]);
		app->objectmanager->obj_tanks[1]->SetColor(tank_colors[1]);
		app->objectmanager->obj_tanks[2]->SetColor(tank_colors[2]);
		app->objectmanager->obj_tanks[3]->SetColor(tank_colors[3]);

	}
	for (uint i = 0; i < 4; ++i)
	{
		Obj_RewardBox* box = app->pick_manager->CreateRewardBox(app->objectmanager->obj_tanks[i]->pos_map + fPoint{ 2.f, -2.f });
		box->SetTypeBox(PICKUP_TYPE::WEAPON);
	}

	round = 0u;
	game_state = GAME_STATE::ENTER_IN_WAVE;
	game_over = false;

	Tesla_trooper_units = 0u;
	Brute_units = 0u;
	Suicidal_units = 0u;
	RocketLauncher_units = 0u;

	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };

	general_gui = DBG_NEW General_GUI();
	leaderboard = DBG_NEW LeaderBoard(screen_center,"data/leader_board.xml",false);

	return true;
}

// Called each loop iteration
bool M_Scene::PreUpdate()
{
	//LOG("Enemy number %i", number_of_enemies);

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)//TODO: Go to pause screen
	{
		app->scmanager->FadeToBlack(this, app->main_menu, 1.f, 1.f );
	}

	return true;
}

// Called each loop iteration
bool M_Scene::Update(float dt)
{
	BROFILER_CATEGORY("M_SceneUpdate", Profiler::Color::Blue);

	bool input_accept = false;

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		input_accept = true;
	}

	for (int i = 0; i < MAX_PLAYERS && input_accept == false; ++i) {

		int controller = app->objectmanager->obj_tanks[i]->GetController();

		if (controller != -1 && app->input->GetControllerButtonState(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
		{
			input_accept = true;
		}
	}

	switch (game_state)
	{
	case GAME_STATE::ENTER_IN_WAVE:
	{
		++round;
		NewWave();
		game_state = GAME_STATE::IN_WAVE;
		app->audio->PlayMusic(main_music, 2.0f);
		app->audio->PauseFx(finish_wave_sound_channel, 2000);
		app->audio->PauseFx(wind_sound_channel, 2000);
		break;
	}
	case GAME_STATE::IN_WAVE:
	{
		if (number_of_enemies<=5)//Hardcode to go to next round when there are less enemies
		{
			game_state = GAME_STATE::EXIT_OF_WAVE;
		}
		break;
	}
	case GAME_STATE::EXIT_OF_WAVE:
	{
		//feedback here I guess(animation and souds)
		timer_between_waves.Start();
		app->audio->PauseMusic(3000);
		finish_wave_sound_channel = app->audio->PlayFx(finish_wave_sound_uint);
		wind_sound_channel = app->audio->PlayFx(wind_sound_uint);
		
		game_state = GAME_STATE::OUT_WAVE;
		general_gui->RoundFX();
		break;
	}
	case GAME_STATE::OUT_WAVE:

		if (timer_between_waves.ReadSec() >= 3 || AllPlayersReady())
		{
			game_state = GAME_STATE::ENTER_IN_WAVE;
		}

		break;

	case GAME_STATE::GAME_OVER:

		for (int i = 0; i < MAX_PLAYERS; ++i) {
			app->objectmanager->obj_tanks[i]->gui->Fade_GUI(false);
		}
		app->audio->PlayMusic("audio/Music/defeat.ogg", 2.0f);
		general_gui->FadeGeneralHUD(false);
		general_gui->FadeGameOverScreen(true, round);
		game_state = GAME_STATE::WAITING_GAMEOVER;
		break;

	case GAME_STATE::WAITING_GAMEOVER:

		if (input_accept == true)
		{
			game_state = GAME_STATE::LEADER_BOARD;
		}

		break;

	case GAME_STATE::LEADER_BOARD:

		if (leaderboard->UpdateLeaderBoard(round) == true)
		{
			leaderboard->FillLeaderBoardTable();
		}
		
		general_gui->FadeGameOverScreen(false);
		leaderboard->FadeLeaderBoardScreen(true);
		game_state = GAME_STATE::WAITING_LEADERBOARD;

		break;

	case GAME_STATE::WAITING_LEADERBOARD:

		leaderboard->SetInputTextToNameLabel();

		if (input_accept == true)
		{
			leaderboard->UpdateLeaderBoardSquadName();
			app->scmanager->FadeToBlack(this, app->main_menu, 1.f, 1.f);
		}
	}

	if (!game_over
		&& !app->objectmanager->obj_tanks[0]->Alive()
		&& !app->objectmanager->obj_tanks[1]->Alive()
		&& !app->objectmanager->obj_tanks[2]->Alive()
		&& !app->objectmanager->obj_tanks[3]->Alive())
	{
		game_state = GAME_STATE::GAME_OVER;
		game_over = true;
	}
	
	return true;
}

// Called each loop iteration
bool M_Scene::PostUpdate(float dt)
{
	bool ret = true;

	//DebugPathfinding();

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

	RELEASE(general_gui);

	general_gui = nullptr;

	for (std::vector<Obj_Tank*>::iterator i = app->objectmanager->obj_tanks.begin(); i != app->objectmanager->obj_tanks.end() ; ++i)
	{
		(*i)->gui = nullptr;
	}

	delete(leaderboard);
	leaderboard = nullptr;

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

void M_Scene::ReduceNumEnemies()
{
	number_of_enemies -= 1;
	if (number_of_enemies < 0)
	{
		number_of_enemies = 0;
	}
	//if (label_number_of_enemies != nullptr)
	//	label_number_of_enemies->SetText("number of enemies:" + std::to_string(number_of_enemies));

}

void M_Scene::CreateEnemyWave()
{
	number_of_enemies = 0;
	number_of_enemies += Tesla_trooper_units;
	number_of_enemies += Brute_units;

	//number_of_enemies += Suicidal_units;

	/*label_number_of_enemies->SetText("number of enemies:" + std::to_string(number_of_enemies));*/

	for (int i = 0; i < Tesla_trooper_units; i++)
	{
		if (app->map->data.spawners_position_enemy.size() != 0)
		{
			uint spawner_random = rand() % app->map->data.spawners_position_enemy.size();
			fPoint pos = app->map->data.spawners_position_enemy.at(spawner_random)->pos;
			Obj_TeslaTrooper * enemy = (Obj_TeslaTrooper*)app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER, pos);
			enemy->SetStats(app->scene->round);
		}
	
	}

	for (int i = 0; i < Brute_units; i++)
	{
		if (app->map->data.spawners_position_enemy.size() != 0)
		{
			uint spawner_random = rand() % app->map->data.spawners_position_enemy.size();
			fPoint pos = app->map->data.spawners_position_enemy.at(spawner_random)->pos;
			Obj_Brute * enemy = (Obj_Brute*)app->objectmanager->CreateObject(ObjectType::BRUTE, pos);
			enemy->SetStats(app->scene->round);
		}
	}

	for (int i = 0; i < RocketLauncher_units; i++)
	{
		if (app->map->data.spawners_position_enemy.size() != 0)
		{
			uint spawner_random = rand() % app->map->data.spawners_position_enemy.size();
			fPoint pos = app->map->data.spawners_position_enemy.at(spawner_random)->pos;
			Obj_RocketLauncher * enemy = (Obj_RocketLauncher*) app->objectmanager->CreateObject(ObjectType::ROCKETLAUNCHER, pos);
			enemy->SetStats(app->scene->round);
		}
	}

	for (int i = 0; i < Suicidal_units; i++)
	{
		if (app->map->data.spawners_position_enemy.size() != 0)
		{
			uint spawner_random = rand() % app->map->data.spawners_position_enemy.size();
			fPoint pos = app->map->data.spawners_position_enemy.at(spawner_random)->pos;
			Obj_Suicidal * enemy = (Obj_Suicidal*)app->objectmanager->CreateObject(ObjectType::SUICIDAL, pos);
			enemy->SetStats(app->scene->round);
		}
	}


}

void M_Scene::NewWave()
{
	Tesla_trooper_units = 30 + 40 * round;

	if (round >= 2)
	{
		Brute_units += round - 1;
		RocketLauncher_units += round - 1;
		//Suicidal_units += round - 1;
	}

	CreateEnemyWave();
	app->pick_manager->CreateRewardBoxWave();
	general_gui->SetRoundNumber(round);
}

bool M_Scene::AllPlayersReady() const
{
	return (app->objectmanager->obj_tanks[0]->IsReady()
		&&  app->objectmanager->obj_tanks[1]->IsReady()
		&&  app->objectmanager->obj_tanks[2]->IsReady()
		&&  app->objectmanager->obj_tanks[3]->IsReady());
}