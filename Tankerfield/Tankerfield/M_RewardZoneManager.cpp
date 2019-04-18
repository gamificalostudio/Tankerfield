#include "App.h"
#include "M_RewardZoneManager.h"
#include "M_ObjManager.h"
#include "Obj_TeslaTrooper.h"

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

void RewardZone::SetPos(fPoint pos)
{
	this->position = pos;
}

void RewardZone::SetSize(const uint& size)
{
	this->size = size;
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
	return true;

}

bool M_RewardZoneManager::CleanUp()
{
	return true;

}

RewardZone* M_RewardZoneManager::CreateRewardZone(fPoint map_center_pos, uint size)
{
	RewardZone* temp_rz = new RewardZone();
	temp_rz->SetPos(map_center_pos);
	temp_rz->SetSize(size);

	/*for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			fPoint map_pos = map_center_pos;
			app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER, map_pos);
		}
	}*/
	app->objectmanager->CreateObject(ObjectType::TESLA_TROOPER, map_center_pos);
	reward_zones.push_back(temp_rz);

	return temp_rz;
}

void M_RewardZoneManager::DeleteRewardZones()
{

}

std::list<RewardZone*> M_RewardZoneManager::GetRewardZones() const
{
	return this->reward_zones;
}