#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Window.h"
#include "j1Render.h"
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

iPoint  MapToWorld(int x, int y, int tile_width, int tile_height)
{
	iPoint ret;

	ret.x = (x - y) * (tile_width * 0.5f);
	ret.y = (x + y) * (tile_height * 0.5f);

	return ret;
}

fPoint  MapToWorldF(float x, float y, float tile_width, float tile_height)
{
	fPoint ret;

	ret.x = (x - y) * (tile_width * 0.5f);
	ret.y = (x + y) * (tile_height * 0.5f);

	return ret;
}

void DrawIsometricQuad (float x, float y, float w, float h)
{
	fPoint point_1, point_2, point_3, point_4;

	// top_left 
	point_1 = MapToWorldF(x, y, 100.f, 50.f);
	// top_right
	point_2 = MapToWorldF(x + w, y, 100.f, 50.f);
	// bot_left
	point_3 = MapToWorldF(x, y + h, 100.f, 50.f);
	// bot_right
	point_4 = MapToWorldF(x + w, y + h, 100.f, 50.f);


	App->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 0, 0, 255, true);
	App->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 0, 0, 255, true);

}

bool UI_Test::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += floor(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= floor(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += floor(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= floor(200.0f * dt);

	// Draw Grid ==============================================
	int rows = 4, columms = 4, tile_width = 100, tile_height = 50;
	iPoint point_1, point_2;


	for (int i = 0; i <= rows; ++i)
	{
		point_1 = MapToWorld( 0 , i, tile_width, tile_height);
		point_2 = MapToWorld(columms, i, tile_width, tile_height);
		App->render->DrawLine(point_1.x , point_1.y, point_2.x, point_2.y, 255, 255, 255, 255, true);
	}

	for (int i = 0; i <= columms; ++i)
	{
		point_1 = MapToWorld(i, 0, tile_width, tile_height);
		point_2 = MapToWorld(i, rows, tile_width, tile_height);
		App->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 255, 255, 255, true);
	}

	// Draw colliders =========================================
	uchar coll_tiles[4][4]
		= { {1, 1, 1, 1},
			{1, 0, 0, 1},
			{1, 0, 0, 1},
			{1, 1, 1, 1} };
	
	for (int j = 0; j < 4; ++j)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (coll_tiles[i][j] == 1)
			{
				iPoint offset = MapToWorld(i, j, tile_width, tile_height);
				App->render->DrawCircle(offset.x, offset.y, 4, 255, 255, 255, 255, true);
			}
		}
	}



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
