

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



Obj_RewardBox::Obj_RewardBox(fPoint pos) : Object(pos)
{

	pugi::xml_node reward_box_node = app->config.child("object").child("reward_box");

	reward_box_dead_sound_string = reward_box_node.child("sound").attribute("value").as_string();
	reward_box_dead_sound_int = app->audio->LoadFx(reward_box_dead_sound_string);

	texture = app->tex->Load(reward_box_node.child("image_path").attribute("value").as_string());
	curr_tex = texture;
	
	shadow_frame = { 56, 0, 30, 18 };
	draw_shadow_offset = {-2 , -4 };
	frame = { 0, 0, 28,34 };
	frame_white = { 28,0,28,34 };
	draw_offset = { -14,-17 };
	curr_frame = &frame;

	coll = app->collision->AddCollider(pos, 0.5f ,0.5f , TAG::REWARD_BOX, BODY_TYPE::STATIC ,0.f, this);//width and height hardcoded
}

Obj_RewardBox::~Obj_RewardBox()
{

}

bool Obj_RewardBox::Update(float dt)
{
	if (is_white)
	{
		if (timer_white.ReadSec() >= max_time_in_white)
		{
			curr_frame = &frame;
			is_white = false;
		}
	}
	return true;
}

void Obj_RewardBox::OnTriggerEnter(Collider * collider)
{
	TakeDamage(collider);
}

void Obj_RewardBox::OnTrigger(Collider * collider)
{
	TakeDamage(collider);
}

bool Obj_RewardBox::Draw(float dt, Camera * camera)
{
	if (curr_frame != nullptr)
		app->render->Blit(
			curr_tex,
			pos_screen.x + draw_offset.x,
			pos_screen.y + draw_offset.y,
			camera,
			curr_frame);

	return true;
}

bool Obj_RewardBox::DrawShadow(Camera * camera, float dt)
{
	app->render->Blit(
		curr_tex,
		pos_screen.x + draw_shadow_offset.x,
		pos_screen.y + draw_shadow_offset.y,
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
			app->pick_manager->CreatePickUp(pos_map, PICKUP_TYPE::WEAPON, ItemType::MAX_ITEMS, WEAPON::MAX_WEAPONS, 1u);
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
			curr_frame = &frame_white;
			timer_white.Start();
		}
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
			curr_frame = &frame_white;
			timer_white.Start();
		}
		Obj_Tank* player = (Obj_Tank*)collider->GetObj();
		Eletro_Shot_Animation* electro_anim = (Eletro_Shot_Animation*)app->objectmanager->CreateObject(ObjectType::ELECTRO_SHOT_ANIMATION, player->pos_map);

		electro_anim->tank = player;
		electro_anim->draw_offset -= (iPoint)app->map->MapToScreenF(player->GetShotDir());
		electro_anim->enemy_pos_screen = pos_screen;
		electro_anim->enemy_pos_map = pos_map;
		electro_anim->hit_no_enemie = false;

	}
}

