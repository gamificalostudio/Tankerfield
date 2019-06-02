#include <cmath>

#include "App.h"
#include "M_RewardZoneManager.h"
#include "M_ObjManager.h"
#include "Obj_TeslaTrooper.h"
#include "M_Scene.h"

/* Reward Zone */

RewardZone::RewardZone()
{

}

RewardZone::~RewardZone()
{

}

fPoint RewardZone::GetPos() const
{
	return this->position;
}

uint RewardZone::GetSize() const
{
	return this->size;
}

uint RewardZone::GetNumberOfUnits() const
{
	return this->number_of_units;
}

void RewardZone::SetPos(fPoint pos)
{
	this->position = pos;
}

void RewardZone::SetSize(const uint& size)
{
	this->size = size;
}

void RewardZone::SetNumberOfUnits(const uint& nou)
{
	this->number_of_units = nou;
}


/* Module RewardZoneManager */

M_RewardZoneManager::M_RewardZoneManager()
{

}

M_RewardZoneManager::~M_RewardZoneManager()
{

}

bool M_RewardZoneManager::Awake(pugi::xml_node& config)
{
	return true;
}

bool M_RewardZoneManager::Start()
{
	return true;
}

bool M_RewardZoneManager::PreUpdate()
{
	return true;

}

bool M_RewardZoneManager::Update(float dt)
{
	return true;

}

bool M_RewardZoneManager::PostUpdate(float dt)
{
	accumulated_time += dt;
	if (accumulated_time >= 2.0f)
	{
		do_check = true;
		accumulated_time = 0.0f;
	}

	if (do_check)
	{
		
	}

	return true;
}

bool M_RewardZoneManager::CleanUp()
{
	for (std::list<RewardZone*>::const_reverse_iterator item = this->reward_zones.rbegin();
		item != reward_zones.rend(); item++)
	{
		delete *item;
	}

	return true;
}

RewardZone* M_RewardZoneManager::CreateRewardZone(fPoint map_center_pos, uint size)
{
	RewardZone* temp_rz = DBG_NEW RewardZone();
	temp_rz->SetPos(map_center_pos);
	temp_rz->SetSize(size);
	// The parameter is the tesla troopers plus the additional reward zone unit
	temp_rz->SetNumberOfUnits(pow(size, 2) + 1);

	/* Generate reward zone */
	Object* temp_obj_01 = (Reward_Zone*)app->objectmanager->CreateObject(ObjectType::REWARD_ZONE, fPoint(map_center_pos));;
	temp_rz->enemy_list.push_back(temp_obj_01);

	/* Generate enemies */
	for (int i = map_center_pos.x - size * 0.5f; i <= map_center_pos.x + size * 0.5f; i++)
	{
		for (int j = map_center_pos.y - size * 0.5f; j <= map_center_pos.y + size * 0.5f; j++)
		{
			Object* temp_obj_02 = app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER, fPoint(i, j));
			Obj_TeslaTrooper * enemy = (Obj_TeslaTrooper*)temp_obj_02;
			enemy->SetStats(app->scene->round);
			temp_rz->enemy_list.push_back(temp_obj_02);
		}
	}
	
	reward_zones.push_back(temp_rz);

	return temp_rz;
}

void M_RewardZoneManager::DeleteRewardZones()
{
	for (std::list<RewardZone*>::const_reverse_iterator item = this->reward_zones.rbegin();
		item != reward_zones.rend(); item++)
	{
		delete *item;
	}
}

std::list<RewardZone*> M_RewardZoneManager::GetRewardZones() const
{
	return this->reward_zones;
}