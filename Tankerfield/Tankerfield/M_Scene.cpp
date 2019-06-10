#include <cstdlib>
#include <ctime>
#include <string>
#include <math.h>

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
#include "LeaderBoard.h"

#include "UI_Label.h"
#include "UI_Image.h"

#include "Point.h"
#include "Rect.h"
#include "Object.h"
#include "PerfTimer.h"
#include "Obj_Tank.h"

#include "Controllers_Settings.h"

#include "Options_Menu.h"
#include "Pause_Menu.h"
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
	SDL_ShowCursor(SDL_DISABLE);

	tank_colors[0] = {255, 0, 0, 255};
	tank_colors[1] = {248, 243, 43, 255};
	tank_colors[2] = {0, 255, 0, 255};
	tank_colors[3] = {0, 0, 255, 255};
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

	round = 1u;
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
	pause_menu = DBG_NEW Pause_Menu();
	options_menu = DBG_NEW Options_Menu(MENU_TYPE::PAUSE_MENU);

	SetMenuState( MENU_STATE::NO_TYPE);

	new_round_animation.Start();

	return true;
}

// Called each loop iteration
bool M_Scene::PreUpdate()
{
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KeyState::KEY_DOWN)
	{
		if (app->IsPaused() == true)
		{
			SetMenuState(MENU_STATE::NO_TYPE);
		}
		else
		{
			SetMenuState(MENU_STATE::INIT_MENU);
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_TAB) == KeyState::KEY_DOWN)
	{
		if (general_gui->GetIsVisible() == false)
		{
			general_gui->ShowGeneralGUI();
		}
		else
		{
			general_gui->HideGeneralGUI();
		}

		for (std::vector<Obj_Tank*>::iterator itr = app->objectmanager->obj_tanks.begin(); itr != app->objectmanager->obj_tanks.end(); ++itr)
		{
			if ((*itr)->gui == nullptr)
			{
				continue;
			}

			if ((*itr)->gui->GetIsVisible() == false)
			{
				(*itr)->gui->ShowGUI();
			}
			else
			{
				(*itr)->gui->HideGUI();
			}
		}

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
		NewWave();
		for (int i = 0; i < 4; ++i)
		{
			app->objectmanager->obj_tanks[i]->NewRound(round);
		}
		app->audio->PlayMusic(main_music, 2.0f);
		app->audio->PauseFx(finish_wave_sound_channel, 2000);
		app->audio->PauseFx(wind_sound_channel, 2000);
		game_state = GAME_STATE::IN_WAVE;
		break;
	}
	case GAME_STATE::IN_WAVE:
	{
		if (app->objectmanager->GetNumberOfEnemies()<=0)
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
		new_round_animation.PrepareAnimation();
		game_state = GAME_STATE::OUT_WAVE;
		break;
	}
	case GAME_STATE::OUT_WAVE:
		new_round_animation.Update(dt);
		//INFO: New round animation changes the game_state to ENTER_IN_WAVE when it has finished
		//In this way, it doens't need to be checking every frame if the animation has finished
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
		if (input_accept)
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
		break;
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

	iPoint mouse_pos;
	app->input->GetMousePosition(mouse_pos.x, mouse_pos.y);
	mouse_pos = app->render->ScreenToWorld(mouse_pos.x, mouse_pos.y, (*app->render->cameras.begin()));
	mouse_pos = app->map->ScreenToMapI(mouse_pos.x, mouse_pos.y);

	//copied from particle system
	//if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	//{
	//	eFire = (Obj_Emitter*)app->objectmanager->CreateObject(ObjectType::EMITTER_FIRE, (fPoint)mouse_pos);
	//}
	//app->render->Blit(torchTex, pos.x - 43, pos.y - 270, &rect);

	//if (eFire != nullptr)
	//{
	//	eFire->pos_map = (fPoint)mouse_pos;
	//}


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

	pause_menu->Delete();

	return true;
}

void M_Scene::SetMenuState(MENU_STATE new_state)
{
	// If state is equal to current state ========================

	if (new_state == menu_state)
	{
		return;
	}

	// Desactive current state ==================================

	switch (menu_state)
	{
	case MENU_STATE::NO_TYPE:
		SDL_ShowCursor(SDL_ENABLE);
		break;
	case MENU_STATE::INIT_MENU:
		pause_menu->HidePauseMenu();
		break;
	case MENU_STATE::OPTIONS:
		options_menu->HideOptionsMenu();
		break;
	case MENU_STATE::CONTROLLERS_SETTINGS:
		for (uint i = 0; i < 4; ++i)
		{
			pause_menu->controllers_setting[i]->HideControllersSettings();
		}
		break;
	}

	// Active new state ======================================

	menu_state = new_state;

	switch (menu_state)
	{
	case MENU_STATE::NO_TYPE:
		SDL_ShowCursor(SDL_DISABLE);
		app->ResumeGame();
		break;
	case MENU_STATE::INIT_MENU:
		app->PauseGame();
		pause_menu->ShowPauseMenu();
		break;
	case MENU_STATE::OPTIONS:
		options_menu->ShowOptionsMenu();
		break;
	case MENU_STATE::CONTROLLERS_SETTINGS:

		for (uint i = 0; i < 4; ++i)
		{
			pause_menu->controllers_setting[i]->ShowControllerSettings();
		}
		break;
	}
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
			Obj_TeslaTrooper * enemy = (Obj_TeslaTrooper*)app->objectmanager->GetObjectFromPool(ObjectType::TESLA_TROOPER, pos);
			enemy->SetStats(app->scene->round);
		}
	
	}

	for (int i = 0; i < Brute_units; i++)
	{
		if (app->map->data.spawners_position_enemy.size() != 0)
		{
			uint spawner_random = rand() % app->map->data.spawners_position_enemy.size();
			fPoint pos = app->map->data.spawners_position_enemy.at(spawner_random)->pos;
			Obj_Brute * enemy = (Obj_Brute*)app->objectmanager->GetObjectFromPool(ObjectType::BRUTE, pos);
			enemy->SetStats(app->scene->round);
		}
	}

	for (int i = 0; i < RocketLauncher_units; i++)
	{
		if (app->map->data.spawners_position_enemy.size() != 0)
		{
			uint spawner_random = rand() % app->map->data.spawners_position_enemy.size();
			fPoint pos = app->map->data.spawners_position_enemy.at(spawner_random)->pos;
			Obj_RocketLauncher * enemy = (Obj_RocketLauncher*) app->objectmanager->GetObjectFromPool(ObjectType::ROCKETLAUNCHER, pos);
			enemy->SetStats(app->scene->round);
		}
	}

	for (int i = 0; i < Suicidal_units; i++)
	{
		if (app->map->data.spawners_position_enemy.size() != 0)
		{
			uint spawner_random = rand() % app->map->data.spawners_position_enemy.size();
			fPoint pos = app->map->data.spawners_position_enemy.at(spawner_random)->pos;
			Obj_Suicidal * enemy = (Obj_Suicidal*)app->objectmanager->GetObjectFromPool(ObjectType::SUICIDAL, pos);
			enemy->SetStats(app->scene->round);
		}
	}


}

void M_Scene::NewWave()
{
	Tesla_trooper_units = 30 + 40 * round;
	Brute_units = 0u;
	RocketLauncher_units = 0u;
	Suicidal_units = 0u;

	if (round >= 2)
	{
		uint number_of_special_enemies = 0.8f * round;
		for (uint iter = 0; iter < number_of_special_enemies; ++iter)
		{
			uint r = rand() % 3 + 1;
			if (r == 1)
			{
				Brute_units += 1;
			}

			else if (r == 2)
			{
				RocketLauncher_units += 1;
			}

			else
			{
				Suicidal_units += 1;
			}
		}
	}
	CreateEnemyWave();
	app->pick_manager->CreateRewardBoxWave();
}