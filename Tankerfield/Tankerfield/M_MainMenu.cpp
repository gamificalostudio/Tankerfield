#include "App.h"
#include "M_MainMenu.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "M_MainMenu.h"


bool M_MainMenu::Start()
{
	background_texture = app->tex->Load("textures/ui/main_menu_background.png");


	return true;
}

bool M_MainMenu::PreUpdate()
{
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
	return true;
}

bool M_MainMenu::Reset()
{
	return true;
}
