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

bool M_MainMenu::Start()
{
	background_texture = app->tex->Load("textures/ui/main_menu_background.png");

	logo_button = app->ui->CreateImage({ 151.f, 151.f }, UI_ImageDef({10, 710, 915, 260}));

	single_player_button = app->ui->CreateButton({ 600.f, 512.f }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	single_player_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Single Player", app->font->button_font, {50, 50, 50, 255}));
	app->ui->AddInteractiveElement(single_player_button);

	multi_player_button = app->ui->CreateButton({ 600.f, 622.f }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	multi_player_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Multi Player", app->font->button_font, { 50, 50, 50, 255 }));
	app->ui->AddInteractiveElement(multi_player_button);

	exit_button = app->ui->CreateButton({ 600.f, 732.f }, UI_ButtonDef({ 10,980,232,88 }, { 255, 980,232,88 }, { 495,970,280 ,136 }, { 785 ,970,280,136 }), this);
	exit_button->SetLabel({ 0.f,2.f }, UI_LabelDef("Exit", app->font->button_font, { 50, 50, 50, 255 }));
	app->ui->AddInteractiveElement(exit_button);

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
	}

	logo_button = nullptr;
	single_player_button = nullptr;
	multi_player_button = nullptr;
	exit_button = nullptr;

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
		app->scmanager->FadeToBlack(this, app->scene, 2.f, 4.f);
	}
	else if (element == exit_button)
	{
		exit_game = true;
	}
	return true;
}
