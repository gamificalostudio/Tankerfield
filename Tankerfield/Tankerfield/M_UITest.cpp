#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_Input.h"
#include "M_Textures.h"
#include "M_Audio.h"
#include "M_Window.h"
#include "M_Render.h"
#include "M_Fonts.h"
#include "M_UITest.h"
#include "UI_Element.h"
#include "UI_Button.h"
#include "UI_Label.h"
#include "M_ObjManager.h"

M_UITest::M_UITest() : Module()
{
	name.assign("UI Test");
}

M_UITest::~M_UITest()
{}

bool M_UITest::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

bool M_UITest::Start()
{
	player_pos = { 2,2 };

	uint win_width = 0u , win_height = 0u;
	app->win->GetWindowSize(win_width, win_height);

	//Button_Definition buttton_def({0,0,280, 140}, {280, 0 , 280, 140}, {560, 0, 280, 140});
	//Label_Definition label_def(App->font->Load("fonts/pixelart.ttf", 30));
	//button_test = App->ui->CreateButton({ win_width * 0.5f ,win_height * 0.5f }, buttton_def, this);
	//button_test->IsDraggable(true);
	//button_test->SetLabel({ 0,0 }, "i wanna die", label_def);

	return true;
}

bool M_UITest::PreUpdate()
{


	return true;
}


void M_UITest::DrawIsometricQuad (float x, float y, float w, float h)
{
	fPoint point_1, point_2, point_3, point_4;

	// top_left 
	point_1 = MapToWorldF(x, y, 100.f, 50.f);
	// top_right
	point_2 = MapToWorldF(x + w, y, 100.f, 50.f);
	// bot_right
	point_3 = MapToWorldF(x + w, y + h, 100.f, 50.f);
	// bot_left
	point_4 = MapToWorldF(x, y + h, 100.f, 50.f);

	app->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 0, 0, 255, true);
	app->render->DrawLine(point_2.x, point_2.y, point_3.x, point_3.y, 255, 0, 0, 255, true);
	app->render->DrawLine(point_3.x, point_3.y, point_4.x, point_4.y, 255, 0, 0, 255, true);
	app->render->DrawLine(point_4.x, point_4.y, point_1.x, point_1.y, 255, 0, 0, 255, true);
}

void M_UITest::DrawIsometricBox(float x, float y, float w, float h, float p)
{
	fPoint point_1, point_2, point_3, point_4;

	// top_left 
	point_1 = MapToWorldF(x, y, 100.f, 50.f);
	// top_right
	point_2 = MapToWorldF(x + w, y, 100.f, 50.f);
	// bot_right
	point_3 = MapToWorldF(x + w, y + h, 100.f, 50.f);
	// bot_left
	point_4 = MapToWorldF(x, y + h, 100.f, 50.f);

	app->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 0, 0, 255, true);
	app->render->DrawLine(point_2.x, point_2.y, point_3.x, point_3.y, 255, 0, 0, 255, true);
	app->render->DrawLine(point_3.x, point_3.y, point_4.x, point_4.y, 255, 0, 0, 255, true);
	app->render->DrawLine(point_4.x, point_4.y, point_1.x, point_1.y, 255, 0, 0, 255, true);
	
	app->render->DrawLine(point_1.x, point_1.y - p, point_2.x, point_2.y - p, 255, 0, 0, 255, true);
	app->render->DrawLine(point_2.x, point_2.y - p, point_3.x, point_3.y - p, 255, 0, 0, 255, true);
	app->render->DrawLine(point_3.x, point_3.y - p, point_4.x, point_4.y - p, 255, 0, 0, 255, true);
	app->render->DrawLine(point_4.x, point_4.y - p, point_1.x, point_1.y - p, 255, 0, 0, 255, true);
	
	app->render->DrawLine(point_1.x, point_1.y, point_1.x, point_1.y - p, 255, 0, 0, 255, true);
	app->render->DrawLine(point_2.x, point_2.y, point_2.x, point_2.y - p, 255, 0, 0, 255, true);
	app->render->DrawLine(point_3.x, point_3.y, point_3.x, point_3.y - p, 255, 0, 0, 255, true);
	app->render->DrawLine(point_4.x, point_4.y, point_4.x, point_4.y - p, 255, 0, 0, 255, true);
}																		 

bool M_UITest::Update(float dt)
{
	// Create basic bullet
	if (app->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
		app->objectmanager->CreateObject(BASIC_BULLET, player_pos);

	

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
				//DrawIsometricQuad(i, j, 1, 1);
			}
		}
	}
	int rows = 100, columms = 100, tile_width = 100, tile_height = 50;
	// Draw Player Pos ========================================
	fPoint player_draw_pos = MapToWorldF(player_pos.x, player_pos.y, tile_width, tile_height);
	app->render->DrawCircle(player_draw_pos.x, player_draw_pos.y, 3, 0, 255, 0, 255, true);
	//DrawIsometricQuad(player_pos.x - .5f, player_pos.y - .5f, 1, 1);
	DrawIsometricBox(player_pos.x - .5f, player_pos.y - .5f, 1, 1, 100);
	return true;
}

bool M_UITest::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

bool M_UITest::CleanUp()
{
	LOG("Freeing ui_test");



	return true;
}

fPoint MapToWorldF(float x, float y, float tile_width, float tile_height)
{
	fPoint ret;

	ret.x = (x - y) * (tile_width * 0.5f);
	ret.y = (x + y) * (tile_height * 0.5f);

	return ret;
}
