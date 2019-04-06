#include "Obj_Bullet.h"
#include "M_Render.h"
#include "App.h"
#include "M_Map.h"

SDL_Texture * Obj_Bullet::tex = nullptr;
int Obj_Bullet::rects_num = 64;
SDL_Rect * Obj_Bullet::rects = new SDL_Rect[rects_num];

Obj_Bullet::Obj_Bullet(fPoint pos) : Object(pos)
{
	bullet_life_ms_timer.Start();
}

Obj_Bullet::~Obj_Bullet()
{
}

bool Obj_Bullet::Start()
{
	pugi::xml_node bullet_node = app->config.child("object").child("basic_bullet");
	LoadRects(bullet_node.child("animations").child("rotate"), rects);//TODO: Optimize. Rects are loaded every time a bullet is created
	if (tex == nullptr)
	{
		tex = app->tex->Load(bullet_node.child("tex").attribute("path").as_string());
	}
	SetRect(0, 0, 67, 42);
	SetPivot(0, 0);
	size = iPoint(frame.w, frame.h);
	return true;
}

bool Obj_Bullet::Update(float dt)
{
	pos_map.x += speed * direction.x * dt;
	pos_map.y += speed * direction.y * dt;

	return true;
}

bool Obj_Bullet::PostUpdate(float dt)
{
	if (bullet_life_ms_timer.ReadMs() >= bullet_life_ms)
	{
		to_remove = true;
	}

	//Collider reference
	//float width = 0.5f;
	//float height = 0.5f;
	//app->render->DrawIsometricQuad(
	//	pos_map.x - width * 0.5f,
	//	pos_map.y - height * 0.5f,
	//	width,
	//	height);

	fPoint screen_pos = app->map->MapToScreenF(pos_map);
	uint ind = GetRotatedIndex(rects_num, angle, ROTATION_DIR::COUNTER_CLOCKWISE, 315);
	app->render->Blit(
		tex,
		screen_pos.x - 35,//TODO: Change when we have the new spritesheet with the bullet rotated from its center
		screen_pos.y - 14,
		&rects[ind]);

	return true;
}
