#include "Bullet_Basic.h"
#include "App.h"
#include "M_UITest.h"
#include "M_Input.h"
#include "M_Render.h"




Bullet_Basic::Bullet_Basic()
{

}

Bullet_Basic::Bullet_Basic(int x, int y)
{
	//Load XML var ============
	pugi::xml_node basic_bullet_node = app->config.child("object").child("basic_bullet");

	speed = basic_bullet_node.child("speed").attribute("value").as_float();
	bullet_life_ms = basic_bullet_node.child("life").attribute("value").as_float();
}

Bullet_Basic::~Bullet_Basic()
{
}

bool Bullet_Basic::Awake(pugi::xml_node &)
{
	return true;
}

bool Bullet_Basic::Start()
{
	return true;
}

bool Bullet_Basic::PreUpdate()
{
	return true;
}

bool Bullet_Basic::Update(float dt)
{
	pos.x += speed * direction.x * dt;
	pos.y += speed * direction.y * dt;
	return true;
}

bool Bullet_Basic::PostUpdate()
{
	app->ui_test->DrawIsometricQuad(pos.x, pos.y, 0.75f, 0.5f);
	return true;
}

bool Bullet_Basic::CleanUp()
{
	return true;
}

