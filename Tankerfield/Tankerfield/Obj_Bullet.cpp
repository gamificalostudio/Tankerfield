#include "Obj_Bullet.h"
#include "M_Render.h"
#include "App.h"
#include "M_Map.h"
#include "M_Collision.h"

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

	coll = app->collision->AddCollider(pos_map, .5f, .5f, Collider::TAG::BULLET, this);

	return true;
}

bool Obj_Bullet::Update(float dt)
{
	pos_map += direction * speed * dt;
	
	coll->AddRigidBody(Collider::BODY_TYPE::SENSOR);

	return true;
}

bool Obj_Bullet::PostUpdate()
{
	if (bullet_life_ms_timer.ReadMs() >= bullet_life_ms)
	{
		to_remove = true;
	}

	fPoint screen_pos = app->map->MapToScreenF(pos_map);
	uint ind = GetRotatedIndex(rects_num, angle, ROTATION_DIR::COUNTER_CLOCKWISE, 315);
	app->render->Blit(
		tex,
		screen_pos.x - 35,//TODO: Change when we have the new spritesheet with the bullet rotated from its center
		screen_pos.y - 14,
		&rects[ind]);

	return true;
}

void Obj_Bullet::OnTrigger(Collider * collider_1)
{
	to_remove = true;
}
