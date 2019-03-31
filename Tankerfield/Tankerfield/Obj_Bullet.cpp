#include "Obj_Bullet.h"
#include "M_Render.h"
#include "App.h"

Obj_Bullet::Obj_Bullet()
{
	bullet_life_ms_timer.Start();
}

Obj_Bullet::~Obj_Bullet()
{
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

	return true;
}
