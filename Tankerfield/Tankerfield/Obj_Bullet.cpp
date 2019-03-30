#include "Obj_Bullet.h"
#include "M_UITest.h"
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
	return false;
}

bool Obj_Bullet::PostUpdate()
{
	if (bullet_life_ms_timer.ReadMs() >= bullet_life_ms)
	{
		to_remove = true;
	}

	app->ui_test->DrawIsometricQuad(pos.x, pos.y, 0.75f, 0.5f);
	return false;
}
