#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Window.h"
#include "j1Fonts.h"
#include "UI_Test.h"
#include "UI_Object.h"
#include "Button_Input.h"
#include "Label.h"

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
	uint win_width = 0u , win_height = 0u;
	App->win->GetWindowSize(win_width, win_height);

	Button_Definition buttton_def({0,0,280, 140}, {280, 0 , 280, 140}, {560, 0, 280, 140});
	Label_Definition label_def(App->font->Load("fonts/pixelart.ttf", 30));
	button_test = App->ui->CreateButton({ win_width * 0.5f ,win_height * 0.5f }, buttton_def, this);
	button_test->IsDraggable(true);
	label_test = App->ui->CreateLabel({ win_width * 0.5f ,win_height * 0.5f },"MORIR", label_def, this);
	label_test->SetParent(button_test);
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
