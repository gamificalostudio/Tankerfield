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
	player_1.position = { 2,2 };
	player_1.coll = App->collision->AddCollider({ 2.f, 2.f }, 1.f, 1.f, Collider::TAG::PLAYER, this, &player_1);
	player_1.coll->SetType(Collider::TYPE::DYNAMIC);

	player_2.position = { 6,6 };
	player_2.coll = App->collision->AddCollider({ 4.f, 5.f }, 1.f, 1.f, Collider::TAG::PLAYER, this, &player_2);
	player_2.coll->SetType(Collider::TYPE::DYNAMIC);

	player_3.position = { 6,6 };
	player_3.coll = App->collision->AddCollider({ 3.f, 6.f }, 1.f, 1.f, Collider::TAG::PLAYER, this, &player_3);
	player_3.coll->SetType(Collider::TYPE::DYNAMIC);

	player_4.position = { 6,6 };
	player_4.coll = App->collision->AddCollider({ 6.f, 6.f }, 1.f, 1.f, Collider::TAG::PLAYER, this, &player_4);
	player_4.coll->SetType(Collider::TYPE::DYNAMIC);



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
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += floor(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= floor(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += floor(200.0f * dt);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= floor(200.0f * dt);

	float speed_xy = 4.f;
	
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		player_1.position.y -= speed_xy * dt;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		player_1.position.y += speed_xy * dt;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		player_1.position.x -= speed_xy * dt;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		player_1.position.x += speed_xy * dt;
	}

	if (App->input->GetKey(SDL_SCANCODE_U) == KEY_REPEAT)
	{
		player_2.position.y -= speed_xy * dt;
	}
	if (App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
	{
		player_2.position.y += speed_xy * dt;
	}
	if (App->input->GetKey(SDL_SCANCODE_H) == KEY_REPEAT)
	{
		player_2.position.x -= speed_xy * dt;
	}
	if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT)
	{
		player_2.position.x += speed_xy * dt;
	}



	if (App->input->GetKey(SDL_SCANCODE_KP_8) == KEY_REPEAT)
	{
		player_3.position.y -= speed_xy * dt;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_5) == KEY_REPEAT)
	{
		player_3.position.y += speed_xy * dt;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_REPEAT)
	{
		player_3.position.x -= speed_xy * dt;
	}
	if (App->input->GetKey(SDL_SCANCODE_KP_6) == KEY_REPEAT)
	{
		player_3.position.x += speed_xy * dt;
	}

	player_1.coll->SetPos(player_1.position.x, player_1.position.y);
	player_2.coll->SetPos(player_2.position.x, player_2.position.y);
	player_3.coll->SetPos(player_3.position.x, player_3.position.y);

	return true;
}

bool UI_Test::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;


	// Draw Grid ==============================================

	int rows = 100, columms = 100, tile_width = 100, tile_height = 50;
	fPoint point_1, point_2;


	for (int i = 0; i <= rows; ++i)
	{
		point_1 = MapToWorldF(0, i);
		point_2 = MapToWorldF(columms, i);
		App->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 255, 255, 255, true);
	}

	for (int i = 0; i <= columms; ++i)
	{
		point_1 = MapToWorldF(i, 0);
		point_2 = MapToWorldF(i, rows);
		App->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 255, 255, 255, true);
	}

	// Draw Player 1 ========================================
	fPoint player_draw_pos = MapToWorldF(player_1.position.x, player_1.position.y);
	App->render->DrawCircle(player_draw_pos.x, player_draw_pos.y, 3, 0, 255, 0, 255, true);

	// Draw Player 2 ========================================
	player_draw_pos = MapToWorldF(player_2.position.x, player_2.position.y);
	App->render->DrawCircle(player_draw_pos.x, player_draw_pos.y, 3, 0, 0, 255, 255, true);

	// Draw Player 3 ========================================
	player_draw_pos = MapToWorldF(player_3.position.x, player_3.position.y);
	App->render->DrawCircle(player_draw_pos.x, player_draw_pos.y, 3, 255, 0, 255, 255, true);

	return ret;
}

bool UI_Test::CleanUp()
{
	LOG("Freeing ui_test");



	return true;
}

void UI_Test::OnCollision(Collider * c1, Collider * c2)
{
	if (c1 == player_1.coll && c2 == wall)
	{
		LOG("BOIIIIIIIIIIIIIIII");
	}


}
