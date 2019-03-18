#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Window.h"
#include "UI_Test.h"
#include "UI_Object.h"
#include "Button_Input.h"

UI_Test::UI_Test() : j1Module()
{
	name.assign("UI Test");
}

UI_Test::~UI_Test()
{}

bool UI_Test::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

bool UI_Test::Start()
{
	Button_Definition buttton_def({0,0,280, 140}, {280, 0 , 280, 140}, {560, 0, 280, 140});
	button_test = App->ui->CreateButton({ 1920 * 0.5f ,1080 * 0.5f }, buttton_def, this);
	return true;
}

bool UI_Test::PreUpdate()
{


	return true;
}

bool UI_Test::Update(float dt)
{



	return true;
}

bool UI_Test::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

bool UI_Test::CleanUp()
{
	LOG("Freeing ui_test");



	return true;
}
