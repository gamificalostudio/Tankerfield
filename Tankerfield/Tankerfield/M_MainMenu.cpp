#include "App.h"
#include "M_MainMenu.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "M_Fonts.h"
#include "M_SceneManager.h"
#include "M_Window.h"
#include "M_Audio.h"
#include "M_Scene.h"

#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Label.h"
#include "UI_InteractiveGroup.h"

#include "Obj_Tank_MainMenu.h"

bool M_MainMenu::Start()
{
	// Load assets ===========================================

	background_texture = app->tex->Load("textures/ui/main_menu_background.png");
	app->audio->PlayMusic("audio/Music/menu_music.ogg");

	// Create UI Elements ====================================

	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };
	camera = app->render->CreateCamera( iPoint(0,0), (SDL_Rect)screen);
	// Main menu ------------------------------
	menu_peg = app->ui->CreateElement(fPoint(), UI_ElementDef());

	logo_image = app->ui->CreateImage({ 151.f, 151.f }, UI_ImageDef({10, 710, 915, 260}));
	logo_image->SetParent(menu_peg);
	menu_elements.push_back(logo_image);

	multi_player_button = app->ui->CreateButton({ 600.f, 522.f }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	multi_player_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Play", app->font->button_font, { 50, 50, 50, 255 }));
	multi_player_button->SetParent(menu_peg);

	app->ui->AddInteractiveElement(multi_player_button);
	menu_elements.push_back(multi_player_button);

	exit_button = app->ui->CreateButton({ 600.f, 632.f }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	exit_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Exit", app->font->button_font, { 50, 50, 50, 255 }));
	exit_button->SetParent(menu_peg);

	app->ui->AddInteractiveElement(exit_button);
	menu_elements.push_back(exit_button);

	version_label = app->ui->CreateLabel({ screen.GetRight() - 40.f, screen.GetBottom() - 40.f }, UI_LabelDef("v .0.5.7", app->font->label_font_38, {255,255,255,180}));
	version_label->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);
	version_label->SetParent(menu_peg);

	menu_elements.push_back(version_label);

	// Selection screen ------------------------

	float element_side = 126;
	UI_InteractiveGroupDef def;
	def.columns = 5;
	def.rows = 5;
	float offset = def.columns * element_side * 0.5f;

	selection_panel = app->ui->CreateIntearctiveGroup(screen_center, def);

	for (int y = 0; y < def.rows; ++y)
	{
		for (int x = 0; x < def.columns; ++x)
		{
			UI_Element* element = app->ui->CreateImage(fPoint(screen_center.x - offset + x * element_side, screen_center.y - offset + y * element_side), UI_ImageDef({ 120 ,265 ,126 ,126 }));
			selection_panel->SetElement(element, iPoint(x, y));
			menu_elements.push_back(element);
		}
	}

	fPoint tank_offset = { 280.f, 280.f };

	for (int i = 0; i < 4; ++i)
	{
		switch (i)
		{
		case 0:
			players[i].tank_pos = { screen.GetLeft() + tank_offset.x, screen.GetTop() + tank_offset.y };
			break;
		case 1:
			players[i].tank_pos = { screen.GetRight() - tank_offset.x, screen.GetTop() + tank_offset.y };
			break;
		case 2:
			players[i].tank_pos = { screen.GetLeft() + tank_offset.x, screen.GetBottom() - tank_offset.y };
			break;
		case 3:
			players[i].tank_pos = { screen.GetRight() - tank_offset.x, screen.GetBottom() - tank_offset.y };
			break;
		}

		players[i].tank = (Obj_Tank_MainMenu*)app->objectmanager->CreateObject(ObjectType::TANK_MAIN_MENU, players[i].tank_pos);
	}

	
	// Set values ==========================================
	SetState(MENU_STATE::INIT_MENU);
	SDL_ShowCursor(SDL_ENABLE);

	return true;
}

bool M_MainMenu::CleanUp()
{
	app->tex->UnLoad(background_texture);
	app->audio->PauseMusic(2);

	if (app->on_clean_up == false)
	{
		for (std::list<UI_Element*>::iterator iter = menu_elements.begin(); iter != menu_elements.end(); ++iter)
		{
			(*iter)->Destroy();
		}

		selection_panel->Destroy();
		menu_peg->Destroy();
	}

	menu_elements.clear();

	logo_image = nullptr;
	single_player_button = nullptr;
	multi_player_button = nullptr;
	exit_button = nullptr;
	version_label = nullptr;

	return true;
}

bool M_MainMenu::PreUpdate()
{
	if ( app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || exit_game == true)
	{
		return false;
	}

	if (app->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)
	{
		SetState(MENU_STATE::INIT_MENU);
	}

	if (app->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
	{
		SetState(MENU_STATE::SELECTION);
	}
	
	if (players[0].controller == nullptr)
	{
		players[0].controller = app->input->GetAbleController();
		players[0].tank->SetController(players[0].controller);
		selection_panel->SetController(players[0].controller);
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

	switch (menu_state)
	{
	case MENU_STATE::INIT_MENU:
		break;
	case MENU_STATE::SELECTION:

		app->render->DrawQuad((SDL_Rect)app->win->GetWindowRect(), 0, 0, 0, 180);

		for (int i = 0; i < 4; ++i)
		{
			app->render->BlitScaled(app->ui->GetAtlas(), players[i].tank_pos.x - section.w * 0.5f * scale, players[i].tank_pos.y - section.h * 0.5f * scale + 8.f, camera, &section, scale, scale);
		}

		break;
	case MENU_STATE::OPTIONS:
		break;
	}
	return true;
}


bool M_MainMenu::Reset()
{
	return true;
}

bool M_MainMenu::OutClick(UI_Element * element)
{
	if (element == multi_player_button)
	{
		app->scmanager->FadeToBlack(this, app->scene, 2.f, 2.f);
	}
	else if (element == exit_button)
	{
		exit_game = true;
	}
	return true;
}

void M_MainMenu::SetState(MENU_STATE new_state)
{
	switch (new_state)
	{
	case MENU_STATE::INIT_MENU:

		menu_peg->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		selection_panel->SetStateToBranch(ELEMENT_STATE::HIDDEN);

		for (int i = 0; i < 4; ++i)
		{
			players[i].tank->active = false;
		}

		break;

	case MENU_STATE::SELECTION:

		menu_peg->SetStateToBranch(ELEMENT_STATE::HIDDEN);
		selection_panel->SetStateToBranch(ELEMENT_STATE::VISIBLE);

		for (int i = 0; i < 4; ++i)
		{
			players[i].tank->active = true;
		}

		break;

	case MENU_STATE::OPTIONS:
		break;
	}

	menu_state = new_state;

}
