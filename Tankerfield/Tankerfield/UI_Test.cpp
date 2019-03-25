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
#include "ObjectManager.h"

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
	player_pos = { 2,2 };

	uint win_width = 0u , win_height = 0u;
	App->win->GetWindowSize(win_width, win_height);

	Button_Definition buttton_def({0,0,280, 140}, {280, 0 , 280, 140}, {560, 0, 280, 140});
	Label_Definition label_def(App->font->Load("fonts/pixelart.ttf", 30));
	button_test = App->ui->CreateButton({ win_width * 0.5f ,win_height * 0.5f }, buttton_def, this);
	button_test->IsDraggable(true);
	button_test->SetLabel({ 0,0 }, "i wanna die", label_def);

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

void UI_Test::DrawIsometricQuad (float x, float y, float w, float h)
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

	App->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 0, 0, 255, true);
	App->render->DrawLine(point_2.x, point_2.y, point_3.x, point_3.y, 255, 0, 0, 255, true);
	App->render->DrawLine(point_3.x, point_3.y, point_4.x, point_4.y, 255, 0, 0, 255, true);
	App->render->DrawLine(point_4.x, point_4.y, point_1.x, point_1.y, 255, 0, 0, 255, true);
}

void DrawIsometricBox(float x, float y, float w, float h, float p)
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

	App->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 0, 0, 255, true);
	App->render->DrawLine(point_2.x, point_2.y, point_3.x, point_3.y, 255, 0, 0, 255, true);
	App->render->DrawLine(point_3.x, point_3.y, point_4.x, point_4.y, 255, 0, 0, 255, true);
	App->render->DrawLine(point_4.x, point_4.y, point_1.x, point_1.y, 255, 0, 0, 255, true);

	App->render->DrawLine(point_1.x, point_1.y - p, point_2.x, point_2.y - p, 255, 0, 0, 255, true);
	App->render->DrawLine(point_2.x, point_2.y - p, point_3.x, point_3.y - p, 255, 0, 0, 255, true);
	App->render->DrawLine(point_3.x, point_3.y - p, point_4.x, point_4.y - p, 255, 0, 0, 255, true);
	App->render->DrawLine(point_4.x, point_4.y - p, point_1.x, point_1.y - p, 255, 0, 0, 255, true);

	App->render->DrawLine(point_1.x, point_1.y, point_1.x, point_1.y - p, 255, 0, 0, 255, true);
	App->render->DrawLine(point_2.x, point_2.y, point_2.x, point_2.y - p, 255, 0, 0, 255, true);
	App->render->DrawLine(point_3.x, point_3.y, point_3.x, point_3.y - p, 255, 0, 0, 255, true);
	App->render->DrawLine(point_4.x, point_4.y, point_4.x, point_4.y - p, 255, 0, 0, 255, true);
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
	
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		player_pos.y -= 1.5f * dt;

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		player_pos.y += 1.5f * dt;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		player_pos.x -= 1.5f * dt;

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		player_pos.x += 1.5f * dt;

	// Create basic bullet
	if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
		App->objectmanager->CreateObject(BASIC_BULLET, player_pos.x, player_pos.y);

	// Draw Grid ==============================================
	int rows = 100, columms = 100, tile_width = 100, tile_height = 50;
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
				//DrawIsometricQuad(i, j, 1, 1);
			}
		}
	}

	// Draw Player Pos ========================================
	fPoint player_draw_pos = MapToWorldF(player_pos.x, player_pos.y, tile_width, tile_height);
	App->render->DrawCircle(player_draw_pos.x, player_draw_pos.y, 3, 0, 255, 0, 255, true);
	//DrawIsometricQuad(player_pos.x - .5f, player_pos.y - .5f, 1, 1);
	DrawIsometricBox(player_pos.x - .5f, player_pos.y - .5f, 1, 1, 100);
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
