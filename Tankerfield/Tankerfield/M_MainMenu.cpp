#include "App.h"
#include "Log.h"
#include "M_MainMenu.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "M_Fonts.h"
#include "M_SceneManager.h"
#include "M_Window.h"
#include "M_Audio.h"
#include "M_Scene.h"
#include "Options_Menu.h"
#include "Controllers_Settings.h"
#include "LeaderBoard.h"

#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Label.h"
#include "UI_InteractiveGroup.h"

#include "Obj_Tank_MainMenu.h"
#include <vector>

bool M_MainMenu::Start()
{
	//Create camera
	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };
	camera = app->render->CreateCamera(iPoint(0, 0), (SDL_Rect)screen);


	// Menus

	options = new Options_Menu();
	controllers_setting[0] = new Controllers_Settings(fPoint(0, 0), 0);
	controllers_setting[1] = new Controllers_Settings(fPoint(screen.w*0.5f, 0), 1);
	controllers_setting[2] = new Controllers_Settings(fPoint(0, screen.h*0.5f), 2);
	controllers_setting[3] = new Controllers_Settings(fPoint(screen.w*0.5f, screen.h*0.5f), 3);

	// Load assets ===========================================

	background_texture = app->tex->Load("textures/ui/main_menu_background.png");
	app->audio->PlayMusic("audio/Music/menu_music.ogg");

	button_enter_sfx = app->audio->LoadFx("audio/Fx/main_menu/button_enter.wav", 20);
	button_select_sfx = app->audio->LoadFx("audio/Fx/main_menu/button_select.wav", 35);
	button_error_sfx = app->audio->LoadFx("audio/Fx/main_menu/button_error.wav", 35);
	selection_finished_sfx = app->audio->LoadFx("audio/Fx/main_menu/selection_finished.wav", 40);

	
	// Create UI Elements ====================================

	// Main menu ------------------------------

	logo_image = app->ui->CreateImage(screen_center + fPoint( - 350.f, - 320.f), UI_ImageDef({10, 710, 915, 260}));
	logo_image->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	play_button = app->ui->CreateButton(screen_center + fPoint(-350.f, -120.f), UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	play_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Play", app->font->button_font_22, { 50, 50, 50, 255 }));

	leaderboard_button = app->ui->CreateButton(screen_center + fPoint(-350.f, 0.f), UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	leaderboard_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Leaderboard", app->font->button_font_22, { 50, 50, 50, 255 }));

	options_menu_button = app->ui->CreateButton(screen_center + fPoint(-350.f, 120.f), UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	options_menu_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Options", app->font->button_font_22, { 50, 50, 50, 255 }));

	credits_menu_button = app->ui->CreateButton(screen_center + fPoint(-350.f, 240.f), UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	credits_menu_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Credits", app->font->button_font_22, { 50, 50, 50, 255 }));

	exit_button = app->ui->CreateButton(screen_center + fPoint(-350.f, 360.f), UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	exit_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Exit", app->font->button_font_22, { 50, 50, 50, 255 }));

	version_label = app->ui->CreateLabel({ screen.GetRight() - 40.f, screen.GetBottom() - 40.f }, UI_LabelDef("v 1.0.0", app->font->label_font_38, {255,255,255,180}));
	version_label->SetPivot(Pivot::X::RIGHT, Pivot::Y::BOTTOM);

	UI_InteractiveGroupDef menu_panel_def( -1, nullptr);

	menu_navigation = app->ui->CreateIntearctiveGroup(screen_center, menu_panel_def, this);
	menu_navigation->AddElement(play_button);
	menu_navigation->AddElement(leaderboard_button);
	menu_navigation->AddElement(options_menu_button);
	menu_navigation->AddElement(credits_menu_button);
	menu_navigation->AddElement(exit_button);


	// Selection screen ------------------------
	player_labels_peg = app->ui->CreateElement(fPoint(), UI_ElementDef());

	float element_side = 126;
	UI_InteractiveGroupDef selection_panel_def( -1, app->ui->CreateImage({ 0.F,0.F }, UI_ImageDef({ 255, 265, 126,126 })));

	fPoint  offset (DEFAULT_PANEL_COLUMNS * element_side * 0.5f , DEFAULT_PANEL_ROWS  * element_side * 0.5f);

	selection_navigation = app->ui->CreateIntearctiveGroup(screen_center, selection_panel_def, this);

	float color_value = 0.f;
	float color_sum = 1.f / (DEFAULT_PANEL_COLUMNS * DEFAULT_PANEL_ROWS + 1);

	for (int y = 0; y < DEFAULT_PANEL_ROWS; ++y)
	{
		for (int x = 0; x < DEFAULT_PANEL_COLUMNS; ++x)
		{
			UI_Element* element = app->ui->CreateImage(fPoint(screen_center.x - offset.x + x * element_side, screen_center.y - offset.y + y * element_side), UI_ImageDef({ 120 ,265 ,126 ,126 }), this);
			element->color_mod  =  colors[x + y* DEFAULT_PANEL_COLUMNS] = GetColor(color_value);
			color_value += color_sum;
			selection_navigation->AddElement(element);
		}
	}

	fPoint tank_offset = { 280.f, 280.f };

	for (int i = 0; i < 4; ++i)
	{
		std::string player_string;

		switch (i)
		{
		case 0:
			players[i].tank_pos = { screen.GetLeft() + tank_offset.x, screen.GetTop() + tank_offset.y };
			player_string = "PLAYER 1";
			break;
		case 1:
			players[i].tank_pos = { screen.GetRight() - tank_offset.x, screen.GetTop() + tank_offset.y };
			player_string = "PLAYER 2";
			break;
		case 2:
			players[i].tank_pos = { screen.GetLeft() + tank_offset.x, screen.GetBottom() - tank_offset.y };
			player_string = "PLAYER 3";
			break;
		case 3:
			players[i].tank_pos = { screen.GetRight() - tank_offset.x, screen.GetBottom() - tank_offset.y };
			player_string = "PLAYER 4";
			break;
		}

		players[i].tank = (Obj_Tank_MainMenu*)app->objectmanager->CreateObject(ObjectType::TANK_MAIN_MENU, players[i].tank_pos);
		players[i].tank->is_isometric = false;

		player_labels[i] = app->ui->CreateLabel(players[i].tank_pos + fPoint(0, 150), UI_LabelDef(player_string, app->font->button_font_40, { 220, 220,220,255 }));
		player_labels[i]->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
		player_labels[i]->SetParent(player_labels_peg);
		players[i].controller = app->input->GetAbleController();
		players[i].tank->SetController(players[i].controller);
	}

	// Credits Menu

	panel_credits = app->ui->CreateImage({ screen.w * 0.5f,screen.h * 0.5f }, UI_ImageDef({ 10,1324,1650,880 }), this);
	panel_credits->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	credits_logo = app->ui->CreateImage({ screen.w * 0.5f+6,225 }, UI_ImageDef({ 10,2215,580,165 }), this);
	credits_logo->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	credits_logo->SetParent(panel_credits);

		// Labels

	jaume_label = app->ui->CreateLabel({ 540, 355 }, UI_LabelDef("Jaume Montagut", app->font->label_font_38, { 255,255,255,180 }));
	jaume_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	jaume_label->SetParent(panel_credits);
	leader_label = app->ui->CreateLabel({ 540, 405 }, UI_LabelDef("Leader", app->font->label_font_38, { 255,255,255,180 }));
	leader_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	leader_label->SetParent(panel_credits);

	aurelio_label = app->ui->CreateLabel({ 1380, 355}, UI_LabelDef("Aurelio Gamarra", app->font->label_font_38, { 255,255,255,180 }));
	aurelio_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	aurelio_label->SetParent(panel_credits);
	ui_label = app->ui->CreateLabel({ 1380, 405 }, UI_LabelDef("UI", app->font->label_font_38, { 255,255,255,180 }));
	ui_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	ui_label->SetParent(panel_credits);

	víctor_label = app->ui->CreateLabel({ 540, 505 }, UI_LabelDef("Víctor Segura", app->font->label_font_38, { 255,255,255,180 }));
	víctor_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	víctor_label->SetParent(panel_credits);
	designer_label = app->ui->CreateLabel({ 540, 555 }, UI_LabelDef("Game Designer", app->font->label_font_38, { 255,255,255,180 }));
	designer_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	designer_label->SetParent(panel_credits);

	jorge_label = app->ui->CreateLabel({ 1380, 505 }, UI_LabelDef("Jorge Gemas", app->font->label_font_38, { 255,255,255,180 }));
	jorge_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	jorge_label->SetParent(panel_credits);
	management_label = app->ui->CreateLabel({ 1380, 555 }, UI_LabelDef("Management", app->font->label_font_38, { 255,255,255,180 }));
	management_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	management_label->SetParent(panel_credits);

	yessica_label = app->ui->CreateLabel({ 540, 655 }, UI_LabelDef("Yessica Servin", app->font->label_font_38, { 255,255,255,180 }));
	yessica_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	yessica_label->SetParent(panel_credits);
	coder_label_yess = app->ui->CreateLabel({ 540, 705}, UI_LabelDef("Coder", app->font->label_font_38, { 255,255,255,180 }));
	coder_label_yess->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	coder_label_yess->SetParent(panel_credits);

	gerard_label = app->ui->CreateLabel({ 1380, 655 }, UI_LabelDef("Gerard Marcos", app->font->label_font_38, { 255,255,255,180 }));
	gerard_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	gerard_label->SetParent(panel_credits);
	coder_label_g = app->ui->CreateLabel({ 1380, 705 }, UI_LabelDef("Coder", app->font->label_font_38, { 255,255,255,180 }));
	coder_label_g->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	coder_label_g->SetParent(panel_credits);

	sergio_label = app->ui->CreateLabel({ 540, 805 }, UI_LabelDef("Sergio Gómez", app->font->label_font_38, { 255,255,255,180 }));
	sergio_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	sergio_label->SetParent(panel_credits);
	art_label = app->ui->CreateLabel({ 540, 855 }, UI_LabelDef("Art & Audio", app->font->label_font_38, { 255,255,255,180 }));
	art_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	art_label->SetParent(panel_credits);

	aitor_label = app->ui->CreateLabel({ 1380, 805 }, UI_LabelDef("Aitor Vélez", app->font->label_font_38, { 255,255,255,180 }));
	aitor_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	aitor_label->SetParent(panel_credits);
	QA_label = app->ui->CreateLabel({ 1380, 855 }, UI_LabelDef("QA", app->font->label_font_38, { 255,255,255,180 }));
	QA_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	QA_label->SetParent(panel_credits);

		// Links Buttons

	website = app->ui->CreateButton({ 1550,200 }, UI_ButtonDef({ 450,1185,99,97 }, { 560, 1185,98,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	website->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	github = app->ui->CreateButton({ 1675,200 }, UI_ButtonDef({ 10,1185,99,97 }, { 120, 1185,98,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	github->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	jaume_github = app->ui->CreateButton({ 340, 380 }, UI_ButtonDef({ 10,1185,99,97 }, { 120, 1185,98,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	jaume_github->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	jaume_linkedin = app->ui->CreateButton({ 740 , 380 }, UI_ButtonDef({ 230,1185,99,97 }, { 340, 1185,99,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	jaume_linkedin->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	aurelio_github = app->ui->CreateButton({ 1180, 380 }, UI_ButtonDef({ 10,1185,99,97 }, { 120, 1185,98,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	aurelio_github->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	aurelio_linkedin = app->ui->CreateButton({ 1580 , 380 }, UI_ButtonDef({ 230,1185,99,97 }, { 340, 1185,99,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	aurelio_linkedin->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	víctor_github = app->ui->CreateButton({ 340, 530 }, UI_ButtonDef({ 10,1185,99,97 }, { 120, 1185,98,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	víctor_github->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	víctor_linkedin = app->ui->CreateButton({ 740 , 530 }, UI_ButtonDef({ 230,1185,99,97 }, { 340, 1185,99,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	víctor_linkedin->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	jorge_github = app->ui->CreateButton({ 1180, 530 }, UI_ButtonDef({ 10,1185,99,97 }, { 120, 1185,98,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	jorge_github->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	jorge_linkedin = app->ui->CreateButton({ 1580 , 530 }, UI_ButtonDef({ 230,1185,99,97 }, { 340, 1185,99,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	jorge_linkedin->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	yessica_github = app->ui->CreateButton({ 340, 680 }, UI_ButtonDef({ 10,1185,99,97 }, { 120, 1185,98,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	yessica_github->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	yessica_linkedin = app->ui->CreateButton({ 740 , 680 }, UI_ButtonDef({ 230,1185,99,97 }, { 340, 1185,99,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	yessica_linkedin->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	gerard_github = app->ui->CreateButton({ 1180, 680 }, UI_ButtonDef({ 10,1185,99,97 }, { 120, 1185,98,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	gerard_github->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	gerard_linkedin = app->ui->CreateButton({ 1580 , 680 }, UI_ButtonDef({ 230,1185,99,97 }, { 340, 1185,99,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	gerard_linkedin->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	sergio_github = app->ui->CreateButton({ 340, 830 }, UI_ButtonDef({ 10,1185,99,97 }, { 120, 1185,98,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	sergio_github->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	sergio_linkedin = app->ui->CreateButton({ 740 , 830 }, UI_ButtonDef({ 230,1185,99,97 }, { 340, 1185,99,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	sergio_linkedin->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	aitor_github = app->ui->CreateButton({ 1180, 830 }, UI_ButtonDef({ 10,1185,99,97 }, { 120, 1185,98,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	aitor_github->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	aitor_linkedin = app->ui->CreateButton({ 1580 , 830 }, UI_ButtonDef({ 230,1185,99,97 }, { 340, 1185,99,97 }, { 667 ,1185,137,130 }, { 808 ,1185,137,130 }), this);
	aitor_linkedin->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

		// Else

	return_credits = app->ui->CreateButton({220,175},UI_ButtonDef({ 10,1080,60,60 }, { 80,1080,60,60 }, { 150,1080,102 ,102 }, { 260 ,1080,102,102 }), this);
	return_credits->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	UI_InteractiveGroupDef credits_navigation_def(-1, nullptr);

	credits_navigation = app->ui->CreateIntearctiveGroup(screen_center,credits_navigation_def,this);

	credits_navigation->AddElement(return_credits);
	credits_navigation->AddElement(website);
	credits_navigation->AddElement(github);
	credits_navigation->AddElement(return_credits);

	credits_navigation->AddElement(jaume_github);
	credits_navigation->AddElement(jaume_linkedin);
	credits_navigation->AddElement(aurelio_github);
	credits_navigation->AddElement(aurelio_linkedin);

	credits_navigation->AddElement(víctor_github);
	credits_navigation->AddElement(víctor_linkedin);
	credits_navigation->AddElement(jorge_github);
	credits_navigation->AddElement(jorge_linkedin);

	credits_navigation->AddElement(yessica_github);
	credits_navigation->AddElement(yessica_linkedin);
	credits_navigation->AddElement(gerard_github);
	credits_navigation->AddElement(gerard_linkedin);

	credits_navigation->AddElement(sergio_github);
	credits_navigation->AddElement(sergio_linkedin);
	credits_navigation->AddElement(aitor_github);
	credits_navigation->AddElement(aitor_linkedin);

	
	// Options =============================================

	options = new Options_Menu();
	
	// Leaderboard =========================================

	panel_leaderboard = app->ui->CreateImage(screen_center, UI_ImageDef({ 1075,395,606,771 }), this);
	panel_leaderboard->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	leaderboard = new LeaderBoard( screen_center + fPoint( 0, 60),"data/leader_board.xml", true);
	leaderboard->FillLeaderBoardTable();

	return_from_leaderboard= app->ui->CreateButton({ screen.w*0.5f-230,225 }, UI_ButtonDef({ 10,1080,60,60 }, { 80,1080,60,60 }, { 150,1080,102 ,102 }, { 260 ,1080,102,102 }), this);
	return_from_leaderboard->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	leaderboard_label= app->ui->CreateLabel({ screen.w*0.5f, 290 }, UI_LabelDef("LeaderBoard", app->font->label_font_38, { 255,255,255,180 }));
	leaderboard_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	leaderboard_label->SetParent(panel_leaderboard);

	UI_InteractiveGroupDef leaderboard_def(-1, nullptr);

	leaderboard_navigation = app->ui->CreateIntearctiveGroup(screen_center, leaderboard_def, this);
	leaderboard_navigation->AddElement(return_from_leaderboard);

	// Set values ==========================================

	SetPlayerObjectsState(false);

	// Controll helper ------------------------

	control_helper_image = app->ui->CreateImage(screen_center + fPoint(-40.f, 400.f), UI_ImageDef(app->ui->button_sprites[(int)CONTROLLER_BUTTON::A]));
	control_helper_image->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	control_helper_label = app->ui->CreateLabel(screen_center + fPoint(10.f, 400.f), UI_LabelDef("Accept", app->font->label_font_24));
	control_helper_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	app->ui->HideAllUI();

	SetState(MENU_STATE::INIT_MENU);

	SDL_ShowCursor(SDL_ENABLE);


	return true;
}

bool M_MainMenu::CleanUp()
{
	app->tex->UnLoad(background_texture);
	app->audio->PauseMusic(2);
	app->render->DestroyCamera(camera);

	delete(options);
	options = nullptr;
	delete(leaderboard);
	leaderboard = nullptr;
	for (int i = 0; i < 4; ++i)
	{
		players[i].tank = nullptr;
	}

	if (options)
	{
		delete options;
		options = nullptr;
	}
	for (uint i = 0; i < 0; ++i)
	{
		if (controllers_setting[i])
		{
			delete controllers_setting[i];
			controllers_setting[i] = nullptr;
		}
	}

	return true;
}

bool M_MainMenu::PreUpdate()
{
	if ( app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || exit_game == true)
	{
		return false;
	}

	return true;
}

bool M_MainMenu::Update(float dt)
{


	return true;
}

bool M_MainMenu::PostUpdate(float dt)
{
	// Blit background ===================================

	SDL_RenderCopy(app->render->renderer, background_texture, NULL, &(SDL_Rect)app->win->GetWindowRect());

	float scale = 2.f;
	SDL_Rect section = { 256, 400, 160, 96 };

	if (menu_state == MENU_STATE::SELECTION || menu_state == MENU_STATE::CHANGE_SCENE)
	{
		app->render->DrawQuad((SDL_Rect)app->win->GetWindowRect(), 0, 0, 0, 180);

		for (int i = 0; i < MAX_PLAYERS; ++i)
		{
			app->render->BlitScaled(app->ui->GetAtlas(), players[i].tank_pos.x - section.w * 0.5f * scale, players[i].tank_pos.y - section.h * 0.5f * scale + 8.f, camera, &section, scale, scale);
		}
	}

	return true;
}


bool M_MainMenu::Reset()
{
	return true;
}

bool M_MainMenu::UI_OnHoverEnter(UI_Element * element)
{
	if (menu_state == MENU_STATE::SELECTION)
	{
		players[current_player].tank->SetColor(element->color_mod);
	}

	app->audio->PlayFx(button_enter_sfx);
	return true;
}

bool M_MainMenu::UI_Selected(UI_Element * element)
{
	if (menu_state == MENU_STATE::SELECTION)
	{
		if (SetPlayerProperties() == true)
		{
			if (current_player < MAX_PLAYERS)
			{
				app->audio->PlayFx(button_select_sfx);
			}
			else
			{
				app->audio->PlayFx(selection_finished_sfx);
			}
		}
		else
		{
			app->audio->PlayFx(button_error_sfx);
		}
	}

	else if (menu_state == MENU_STATE::INIT_MENU)
	{

		if (element == play_button)
		{
			SetState(MENU_STATE::SELECTION);
			app->audio->PlayFx(button_select_sfx);
		}
		else if (element == credits_menu_button)
		{
			SetState(MENU_STATE::CREDITS);
			app->audio->PlayFx(button_select_sfx);
		}
		else if (element == options_menu_button)
		{
			app->audio->PlayFx(button_select_sfx);
			SetState(MENU_STATE::OPTIONS);
		}
		else if (element == exit_button)
		{
			exit_game = true;
			app->audio->PlayFx(button_select_sfx);
		}
		else if (element == leaderboard_button)
		{
			app->audio->PlayFx(button_select_sfx);
			SetState(MENU_STATE::LEADERBOARD);
		}

		app->audio->PlayFx(button_select_sfx);
	}

	else if (menu_state == MENU_STATE::LEADERBOARD)
	{
		if (element == return_from_leaderboard)
		{
			SetState(MENU_STATE::INIT_MENU);
		}

		app->audio->PlayFx(button_select_sfx);
	}

	else if (menu_state == MENU_STATE::CREDITS)
	{
		if (element == github)
		{
			ShellExecute(NULL, "open", "https://github.com/gamificalostudio/Tankerfield", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == website)
		{
			ShellExecute(NULL, "open", "https://gamificalostudio.github.io/Tankerfield/", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == jaume_linkedin)
		{
			ShellExecute(NULL, "open", "https://www.linkedin.com/in/jaume-montagut-guix-7389a4166/", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == jaume_github)
		{
			ShellExecute(NULL, "open", "https://github.com/JaumeMontagut", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == aurelio_github)
		{
			ShellExecute(NULL, "open", "https://github.com/alejandro61299", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == aurelio_linkedin)
		{
			ShellExecute(NULL, "open", "https://www.linkedin.com/in/alejandro-a-gamarra-ni%C3%B1o-568b6b171/", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == víctor_github)
		{
			ShellExecute(NULL, "open", "https://github.com/VictorSegura99", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == víctor_linkedin)
		{
			ShellExecute(NULL, "open", "https://www.linkedin.com/in/v%C3%ADctor-segura-blanco-297458185/", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == jorge_github)
		{
			ShellExecute(NULL, "open", "https://github.com/jorgegh2", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == jorge_linkedin)
		{
			ShellExecute(NULL, "open", "https://www.linkedin.com/in/jorge-gemas-herencia-28140b188/", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == yessica_github)
		{
			ShellExecute(NULL, "open", "https://github.com/YessicaSD", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == yessica_linkedin)
		{
			ShellExecute(NULL, "open", "https://www.linkedin.com/in/yessica-servin-dominguez-663175165/", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == gerard_github)
		{
			ShellExecute(NULL, "open", "https://github.com/vsRushy", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == gerard_linkedin)
		{
			ShellExecute(NULL, "open", "https://www.linkedin.com/in/gerard-marcos-freixas-0a9284158/", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == sergio_github)
		{
			ShellExecute(NULL, "open", "https://github.com/Sersius", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == sergio_linkedin)
		{
			ShellExecute(NULL, "open", "https://www.linkedin.com/in/sergio-g%C3%B3mez-b81b69184/", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == aitor_github)
		{
			ShellExecute(NULL, "open", "https://github.com/AitorVelez", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == aitor_linkedin)
		{
			ShellExecute(NULL, "open", "https://www.linkedin.com/in/aitor-v%C3%A9lez-tolosa-48186a129/", NULL, NULL, SW_SHOWNORMAL);
		}

		else if (element == return_credits)
		{
			SetState(MENU_STATE::INIT_MENU);
		}

		app->audio->PlayFx(button_select_sfx);
	}

	return true;
}


bool M_MainMenu::SetPlayerProperties()
{
	UI_Element* element_focused = selection_navigation->GetFocusedElement();

	if (element_focused == nullptr)
	{
		return false;
	}

	SDL_Color color_1 = element_focused->color_mod;
	SDL_Color color_2 = { 255,255,255,255 };

	if (color_1.r == color_2.r && color_1.g == color_2.g && color_1.b == color_2.b )
	{
		return false;
	}

	app->scene->tank_colors[current_player] = element_focused->color_mod;
	element_focused->color_mod = color_2;
	player_labels[current_player]->color_mod = { 220, 220, 220, 255 };
	current_player += 1;

	if (current_player == MAX_PLAYERS)
	{
		std::vector<Module*> modules_to_reset;
		modules_to_reset.push_back((Module*)app->objectmanager);
		modules_to_reset.push_back((Module*)app->ui);
		modules_to_reset.push_back((Module*)app->debug);
		app->scmanager->FadeToBlack(this, app->scene, 2.f, 2.f, modules_to_reset);
		SetState(MENU_STATE::CHANGE_SCENE);
	}
	else
	{
		player_labels[current_player]->color_mod = { 255, 20, 20, 255 };
	}

	return true;
}


void M_MainMenu::ResetPanelColors()
{
	if (selection_navigation == nullptr)
	{
		return;
	}

	int i = 0;

	for ( std::list<UI_Element*>::iterator iter = selection_navigation->GetGroupList()->begin(); iter != selection_navigation->GetGroupList()->end() ; ++iter)
	{
		(*iter)->color_mod = colors[i];
		++i;
	}
}

void M_MainMenu::SetState(MENU_STATE new_state)
{
	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };

	// If state is equal to current state ========================

	if (new_state == menu_state)
	{
		return;
	}

	// Desactive current state ==================================

	switch (menu_state)
	{
	case MENU_STATE::INIT_MENU:
		current_player = 0;
		menu_navigation->SetStateToBranch(ELEMENT_STATE::HIDDEN);
		logo_image->SetState(ELEMENT_STATE::HIDDEN);
		version_label->SetState(ELEMENT_STATE::HIDDEN);
		break;

	case MENU_STATE::SELECTION:

		break;
	case MENU_STATE::CREDITS:
		current_player = 0;
		panel_credits->SetStateToBranch(ELEMENT_STATE::HIDDEN);
		credits_navigation->SetStateToBranch(ELEMENT_STATE::HIDDEN);
		break;
	case MENU_STATE::OPTIONS:
		options->HideOptionsMenu();
		break;
	case MENU_STATE::LEADERBOARD:
		panel_leaderboard->SetStateToBranch(ELEMENT_STATE::HIDDEN);
		leaderboard->HideLeaderBoard();
		leaderboard_navigation->SetStateToBranch(ELEMENT_STATE::HIDDEN);
		break;

	case MENU_STATE::CONTROLLERS_SETTINGS:
		for (uint i = 0; i < 4; ++i)
		{
			controllers_setting[i]->HideControllersSettings();
		}
		break;
	}

	// Active new state ======================================

	switch (new_state)
	{
	case MENU_STATE::INIT_MENU:
		SetPlayerObjectsState(false);
		menu_navigation->Active();

		menu_navigation->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		logo_image->SetState(ELEMENT_STATE::VISIBLE);
		version_label->SetState(ELEMENT_STATE::VISIBLE);

		control_helper_label->SetPos(screen_center + fPoint(-330, 260));
		control_helper_label->SetText("Accept");
		control_helper_image->SetPos(screen_center + fPoint(-380, 260));
		break;

	case MENU_STATE::SELECTION:

		// Set Values -------------------------------------------
		current_player = 0;
		player_labels[0]->color_mod = { 250, 20, 20, 255 };
		player_labels[1]->color_mod = { 220, 220, 220, 255 };
		player_labels[2]->color_mod = { 220, 220, 220, 255 };
		player_labels[3]->color_mod = { 220, 220, 220, 255 };
		SetPlayerObjectsState(true);

		// Set elements state -----------------------------------

		selection_navigation->Active();
		selection_navigation->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		player_labels_peg->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		control_helper_label->SetPos(screen_center + fPoint(40, 350));
		control_helper_label->SetText("Select Color");
		control_helper_image->SetPos(screen_center + fPoint(-50, 350));

		break;
	case MENU_STATE::CREDITS:
		credits_navigation->Active();
		panel_credits->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		credits_navigation->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		break;
	case MENU_STATE::OPTIONS:
		options->ShowOptionsMenu();
		break;
	case MENU_STATE::CONTROLLERS_SETTINGS:
		{
			for (uint i = 0; i < 4; ++i)
			{
				controllers_setting[i]->ShowControllerSettings();
			}
		}
		break;

	case MENU_STATE::LEADERBOARD:
		leaderboard_navigation->Active();
		leaderboard->ShowLeaderBoard();
		panel_leaderboard->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		leaderboard_navigation->SetStateToBranch(ELEMENT_STATE::VISIBLE);

		break;

	case MENU_STATE::CHANGE_SCENE:
	
		app->ui->DesactiveAllInteractiveGroups();
		break;
	}

	menu_state = new_state;

}

SDL_Color M_MainMenu::GetColor(float value)
{
	if (value > 1.f)
		value = 1.f;
	else if (value < 0.f)
		value = 0.f;

	float array_pos = 6.f * value;
	float int_part = 0;
	float float_part = modf(array_pos, &int_part);

	int   src = (int)int_part;
	int   target = src + 1;

	if (target > 5)
	{
		target = 0;
	}

	float r_value = lerp(R_Color[src], R_Color[target], float_part);
	float g_value = lerp(G_Color[src], G_Color[target], float_part);
	float b_value = lerp(B_Color[src], B_Color[target], float_part);

	SDL_Color ret = { (uint)r_value,  (uint)g_value, (uint)b_value, 255u };

	return ret;
}

void M_MainMenu::SetPlayerObjectsState(bool new_value)
{
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		players[i].tank->active = new_value;
	}
}