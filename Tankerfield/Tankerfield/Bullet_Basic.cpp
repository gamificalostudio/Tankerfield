#include "Bullet_Basic.h"
#include "App.h"




Bullet_Basic::Bullet_Basic()
{

}

Bullet_Basic::Bullet_Basic(float x, float y) : Obj_Bullet()
{
	//Load XML var ============
	pos.x = x;
	pos.y = y;
}

Bullet_Basic::~Bullet_Basic()
{
}

