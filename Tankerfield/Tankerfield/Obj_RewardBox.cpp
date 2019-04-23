

#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"

#include "Obj_RewardBox.h"
#include "App.h"
#include "M_Render.h"
#include "M_Collision.h"
#include "M_PickManager.h"
#include "M_Map.h"
#include "M_Audio.h"



Obj_RewardBox::Obj_RewardBox(fPoint pos) : Object(pos)
{

	pugi::xml_node reward_box_node = app->config.child("object").child("reward_box");

	reward_box_dead_sound_string = reward_box_node.child("sound").attribute("value").as_string();
	reward_box_dead_sound_int = app->audio->LoadFx(reward_box_dead_sound_string);

	texture = app->tex->Load(reward_box_node.child("image_path").attribute("value").as_string());
	curr_tex = texture;
	frame = { 14, 21, 28,34 };
	draw_offset = { 14,20 };

	shadow_frame = { 89, 37, 30, 18 };
	draw_shadow_offset = { 0, 5 };

	life = 1;

	coll = app->collision->AddCollider(pos, 0.5f ,0.5f , Collider::TAG::REWARD_BOX, 0.f, this);//width and height hardcoded
	coll->AddRigidBody(Collider::BODY_TYPE::STATIC);
}

Obj_RewardBox::~Obj_RewardBox()
{
}



void Obj_RewardBox::OnTrigger(Collider * collider)
{
	if (collider->GetTag() == Collider::TAG::BULLET || collider->GetTag() == Collider::TAG::FRIENDLY_BULLET || collider->GetTag() == Collider::TAG::BULLET_LASER)
	{
		GetDamage(collider->damage);
	}

}

//bool Obj_RewardBox::Draw(float dt, Camera * camera)
//{
//	app->render->DrawIsometricQuad(pos_map.x, pos_map.y, 2, 2, { 255,255,255,255 }, camera);
//
//	return true;
//}

bool Obj_RewardBox::DrawShadow(Camera * camera)
{
	app->render->Blit(
		texture,
		pos_screen.x,
		pos_screen.y-draw_shadow_offset.y,
		camera,
		&shadow_frame
	);

	return true;
}


void Obj_RewardBox::GetDamage(float damage)
{
	if (life - damage <= 0 && life != 0)
	{
		life = 0;
		Dead();
	}
	else
	{
		life -= damage;
	}
}

void Obj_RewardBox::Dead()
{
	uint probability = rand() % 100;

	if (probability < 15)
	{
		app->pick_manager->CreatePickUp(pos_map, PICKUP_TYPE::ITEM);
	}

	else if (probability < 25)
	{
		fPoint offset{ 0.5f,0 };
		app->pick_manager->CreatePickUp(pos_map - offset, PICKUP_TYPE::ITEM);
		app->pick_manager->CreatePickUp(pos_map + offset, PICKUP_TYPE::ITEM);
	}

	else if (probability < 75)
	{
		app->pick_manager->CreatePickUp(pos_map, PICKUP_TYPE::WEAPON);
	}

	else if (probability < 100)
	{
		app->pick_manager->CreatePickUp(pos_map, PICKUP_TYPE::WEAPON, 1);
	}

	if (my_spawn_point != nullptr)
	{
		my_spawn_point->occupied = false;
	}
	to_remove = true;

	app->audio->PlayFx(reward_box_dead_sound_int);
}

