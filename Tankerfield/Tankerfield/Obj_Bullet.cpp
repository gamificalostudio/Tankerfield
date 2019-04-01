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
	LoadRects(bullet_node.child("animations").child("rotate"), rects);
	tex = app->tex->Load(bullet_node.child("tex").attribute("path").as_string());

	return true;
}

bool Obj_Bullet::Update(float dt)
{
	pos.x += speed * direction.x * dt;
	pos.y += speed * direction.y * dt;

	return true;
}

bool Obj_Bullet::PostUpdate()
{
	if (bullet_life_ms_timer.ReadMs() >= bullet_life_ms)
	{
		to_remove = true;
	}

	float width = 0.5f;
	float height = 0.5f;

	app->render->DrawIsometricQuad(
		pos.x - width * 0.5f,
		pos.y - height * 0.5f,
		width,
		height);

	fPoint screen_pos = app->map->MapToWorldF(pos.x, pos.y);
	uint ind = GetRotatedIndex(rects_num, angle, ROTATION_DIR::COUNTER_CLOCKWISE, 135);
	app->render->Blit(tex, screen_pos.x, screen_pos.y, &rects[ind]);

	return true;
}
