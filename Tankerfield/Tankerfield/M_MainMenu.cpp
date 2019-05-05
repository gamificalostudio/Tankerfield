#include "App.h"
#include "M_MainMenu.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "M_MainMenu.h"

#include "UI_Image.h"
#include "UI_Button.h"
#include "M_Fonts.h"
#include "UI_Label.h"
#include "M_SceneManager.h"
#include "M_Scene.h"
#include "M_Window.h"
#include "M_Audio.h"

bool M_MainMenu::Start()
{
	fRect screen = app->win->GetWindowRect();
	background_texture = app->tex->Load("textures/ui/main_menu_background.png");

	app->audio->PlayMusic("audio/Music/menu_music.ogg");

	logo_button = app->ui->CreateImage({ 151.f, 151.f }, UI_ImageDef({10, 710, 915, 260}));

	//single_player_button = app->ui->CreateButton({ 600.f, 512.f }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	//single_player_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Single Player", app->font->button_font, {50, 50, 50, 255}));
	//app->ui->AddInteractiveElement(single_player_button);

	multi_player_button = app->ui->CreateButton({ 600.f, 522.f }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	multi_player_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Play", app->font->button_font, { 50, 50, 50, 255 }));
	app->ui->AddInteractiveElement(multi_player_button);

	exit_button = app->ui->CreateButton({ 600.f, 632.f }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	exit_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Exit", app->font->button_font, { 50, 50, 50, 255 }));
	app->ui->AddInteractiveElement(exit_button);

	version_label = app->ui->CreateLabel({ screen.GetRight() - 40.f, screen.GetBottom() - 40.f }, UI_LabelDef("v .0.5.7", app->font->label_font_38, {255,255,255,180}));
	version_label->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);
	app->ui->able_axis = FOCUS_AXIS::Y;

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
	app->render->BlitUI(background_texture, 0, 0, NULL);
	return true;
}

bool M_MainMenu::CleanUp()
{
	app->tex->UnLoad(background_texture);
	app->audio->PauseMusic(2);

	if (app->on_clean_up == false)
	{
		if (logo_button != nullptr)
		{
			logo_button->Destroy();
		}
		if (single_player_button != nullptr)
		{
			single_player_button->Destroy();
		}
		if (multi_player_button != nullptr)
		{
			multi_player_button->Destroy();
		}
		if (exit_button != nullptr)
		{
			exit_button->Destroy();
		}
		if (version_label != nullptr)
		{
			version_label->Destroy();
		}
	}

	logo_button = nullptr;
	single_player_button = nullptr;
	multi_player_button = nullptr;
	exit_button = nullptr;
	version_label = nullptr;

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
		if (SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE)
		{
			SDL_ShowCursor(SDL_DISABLE);
		}
	}
	else if (element == exit_button)
	{
		exit_game = true;
	}
	return true;
}
