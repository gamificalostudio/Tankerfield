#include "Bullet_Basic.h"
#include "App.h"




Bullet_Basic::Bullet_Basic()
{

}

Bullet_Basic::Bullet_Basic(int x, int y) : Obj_Bullet()
{
	//Load XML var ============
	pos.x = x;
	pos.y = y;
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
	Obj_Bullet::PostUpdate();

	return true;
}

bool Bullet_Basic::CleanUp()
{
	return true;
}

