#include "Item_HealingArea.h"
#include "App.h"
#include "M_Collision.h"
#include "Obj_Tank.h"

Item_HealingArea::Item_HealingArea(fPoint pos) : Obj_Item(pos)
{
}

bool Item_HealingArea::Update(float dt)
{
	if (Timer_life.ReadSec() >= time_of_life_max)
	{
		to_remove = true;
	}

	return true;
}

bool Item_HealingArea::Use()
{
	pugi::xml_node healing_area_node = app->config.child("items").child("healing_area");

	//--stats of the item
	heal_by_second = healing_area_node.child("heal_by_second").attribute("value").as_float();

	time_of_life_max = healing_area_node.child("time_of_life").attribute("value").as_float();
	Timer_life.Start();

	//-- Collider
	coll_w = healing_area_node.child("collider_width").attribute("value").as_float();
	coll_h = healing_area_node.child("collider_height").attribute("value").as_float();

	pos_map -= {coll_w * 0.5f, coll_h * 0.5f}; //new pos in map with the caster centered

	coll = app->collision->AddCollider(pos_map, coll_w, coll_h, Collider::TAG::HEALING_AREA, 0, this);
	coll->AddRigidBody(Collider::BODY_TYPE::SENSOR);

	return true;
}

void Item_HealingArea::OnTrigger(Collider* collider, float dt)
{
	Obj_Tank* tank = (Obj_Tank*)collider->GetObj();

	tank->SetLife(tank->GetLife() + heal_by_second * dt);
}