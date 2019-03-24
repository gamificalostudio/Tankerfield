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
#include "ObjectManager.h"
#include "Object.h"
#include "Label.h"
#include <typeinfo>
#include "Module_Collision.h"

float tile_width = 100.f;
float tile_height = 50.f;


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
	player.position = { 2,2 };
	player.coll = App->collision->AddCollider({ 2.f, 2.f }, 1.f, 1.f, Collider::TAG::PLAYER, this, &player);
	player.coll->SetType(Collider::TYPE::DYNAMIC);
	wall = App->collision->AddCollider({ 4.f, 4.f }, 1.f, 1.f, Collider::TAG::WALL, this);
	return true;
}

bool UI_Test::PreUpdate()
{

	return true;
}

fPoint  UI_Test::MapToWorldF(float x, float y)
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
	point_1 = MapToWorldF(x, y);
	// top_right
	point_2 = MapToWorldF(x + w, y);
	// bot_right
	point_3 = MapToWorldF(x + w, y + h);
	// bot_left
	point_4 = MapToWorldF(x, y + h);

	App->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 0, 0, 255, true);
	App->render->DrawLine(point_2.x, point_2.y, point_3.x, point_3.y, 255, 0, 0, 255, true);
	App->render->DrawLine(point_3.x, point_3.y, point_4.x, point_4.y, 255, 0, 0, 255, true);
	App->render->DrawLine(point_4.x, point_4.y, point_1.x, point_1.y, 255, 0, 0, 255, true);
}

void UI_Test::DrawIsometricBox(float x, float y, float w, float h, float p)
{
	fPoint point_1, point_2, point_3, point_4;

	// top_left 
	point_1 = MapToWorldF(x, y);
	// top_right
	point_2 = MapToWorldF(x + w, y);
	// bot_right
	point_3 = MapToWorldF(x + w, y + h);
	// bot_left
	point_4 = MapToWorldF(x, y + h);

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
	player.velocity = { 0,0 };

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += floor(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= floor(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += floor(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= floor(200.0f * dt);
	
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		player.position.y -= 1.5f * dt;
		player.velocity.y = -1;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		player.position.y += 1.5f * dt;
		player.velocity.y = 1.f;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		player.position.x -= 1.5f * dt;
		player.velocity.x = -1;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		player.position.x += 1.5f * dt;
		player.velocity.x = 1;
	}

	// Draw Grid ==============================================

	int rows = 100, columms = 100, tile_width = 100, tile_height = 50;
	fPoint point_1, point_2;


	for (int i = 0; i <= rows; ++i)
	{
		point_1 = MapToWorldF( 0 , i);
		point_2 = MapToWorldF(columms, i);
		App->render->DrawLine(point_1.x , point_1.y, point_2.x, point_2.y, 255, 255, 255, 255, true);
	}

	for (int i = 0; i <= columms; ++i)
	{
		point_1 = MapToWorldF(i, 0);
		point_2 = MapToWorldF(i, rows);
		App->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 255, 255, 255, true);
	}

	// Draw Player Pos ========================================
	fPoint player_draw_pos = MapToWorldF(player.position.x, player.position.y);
	App->render->DrawCircle(player_draw_pos.x, player_draw_pos.y, 3, 0, 255, 0, 255, true);
	player.coll->SetPos(player.position.x, player.position.y);

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

void UI_Test::OnCollision(Collider * c1, Collider * c2)
{
	if (c1 == player.coll && c2 == wall)
	{
		LOG("BOIIIIIIIIIIIIIIII");
	}


}
