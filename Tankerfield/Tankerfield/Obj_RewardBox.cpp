

#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"

#include "Obj_RewardBox.h"
#include "App.h"
#include "M_Render.h"
#include "M_Collision.h"
#include "M_PickManager.h"
#include "M_Map.h"
#include "M_Audio.h"
#include "Obj_ElectroShotAnimation.h"
#include "M_Scene.h"
#include "Obj_SpawnPoint.h"



Obj_RewardBox::Obj_RewardBox(fPoint pos) : Object(pos)
{

	pugi::xml_node reward_box_node = app->config.child("object").child("reward_box");

	reward_box_dead_sound_string = reward_box_node.child("crash_sound").attribute("value").as_string();
	reward_box_dead_sound_int = app->audio->LoadFx(reward_box_dead_sound_string, 100);

	reward_box_hit_sound_string = reward_box_node.child("hit_sound").attribute("value").as_string();
	reward_box_hit_sound_int = app->audio->LoadFx(reward_box_hit_sound_string, 100);

	texture = app->tex->Load(reward_box_node.child("image_path").attribute("value").as_string());
	curr_tex = texture;
	
	frame = default_frame = {  0, 0, 45, 34 };
	frame_white   = { 45, 0, 45, 34 };
	shadow_frame  = { 90, 0, 45, 34 };

	draw_offset   = { 14, 27 };

	float coll_w = 0.75f;
	float coll_h = 0.75f;
	coll = app->collision->AddCollider(pos, coll_w ,coll_h , TAG::REWARD_BOX, BODY_TYPE::STATIC ,0.f, this);
	coll->SetObjOffset({ -coll_w * 0.75f, -coll_h * 0.75f });
}

Obj_RewardBox::~Obj_RewardBox()
{

}

bool Obj_RewardBox::Update(float dt)
{
	if (is_white
		&& timer_white.ReadSec() >= max_time_in_white)
	{
		frame = default_frame;
		is_white = false;
	}
	return true;
}

void Obj_RewardBox::OnTriggerEnter(Collider * collider, float dt)
{
	TakeDamage(collider);
}

void Obj_RewardBox::OnTrigger(Collider * collider, float dt)
{
	TakeDamage(collider);
}

bool Obj_RewardBox::DrawShadow(Camera * camera, float dt)
{
	app->render->Blit(
		curr_tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&shadow_frame);

	return true;
}


void Obj_RewardBox::Dead()
{
	uint probability = rand() % 100;
	if (type == PICKUP_TYPE::NO_TYPE)
	{
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
			app->pick_manager->CreatePickUp(pos_map, PICKUP_TYPE::WEAPON, ItemType::MAX_ITEMS, WEAPON::MAX_WEAPONS, app->scene->round + 1u);
		}
		if (my_spawn_point != nullptr)
		{
			my_spawn_point->occupied = false;
		}
	}
	else
	{
		app->pick_manager->CreatePickUp(pos_map, type);
	}
	to_remove = true;

	app->audio->PlayFx(reward_box_dead_sound_int);
}

void Obj_RewardBox::SetTypeBox(PICKUP_TYPE type)
{
	this->type = type;
}

void Obj_RewardBox::TakeDamage(Collider* collider)
{
	if (collider->GetTag() == TAG::BULLET || collider->GetTag() == TAG::FRIENDLY_BULLET || collider->GetTag() == TAG::BULLET_LASER || collider->GetTag() == TAG::BULLET_OIL || collider->GetTag() == TAG::FLAMETHROWER)
	{
		++hits_taken;
		if (hits_taken >= max_hits)
		{
			Dead();
		}
		else
		{
			is_white = true;
			frame = frame_white;
			timer_white.Start();
		}

		app->audio->PlayFx(reward_box_hit_sound_int);
	}

	else if (collider->GetTag() == TAG::ELECTRO_SHOT)
	{
		++hits_taken;
		if (hits_taken > max_hits)
		{
			Dead();
		}
		else
		{
			is_white = true;
			frame = frame_white;
			timer_white.Start();
		}
		Obj_Tank* player = (Obj_Tank*)collider->GetObj();
		Eletro_Shot_Animation* electro_anim = (Eletro_Shot_Animation*)app->objectmanager->CreateObject(ObjectType::ELECTRO_SHOT_ANIMATION, player->pos_map);

		electro_anim->tank = player;
		electro_anim->draw_offset -= (iPoint)app->map->MapToScreenF(player->GetShotDir());
		electro_anim->enemy_pos_screen = pos_screen;
		electro_anim->enemy_pos_map = pos_map;
		electro_anim->hit_no_enemie = false;

		app->audio->PlayFx(reward_box_hit_sound_int);
	}
}

