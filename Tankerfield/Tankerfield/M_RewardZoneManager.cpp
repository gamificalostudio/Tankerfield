#include "M_RewardZoneManager.h"

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

void RewardZone::SetPos(const fPoint& pos)
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

RewardZone* M_RewardZoneManager::CreateRewardZone(iPoint map_center_pos, uint size)
{
	RewardZone* temp_rz = nullptr;
	temp_rz 

	reward_zones.push_back(temp_rz);

	return test;
}

void M_RewardZoneManager::DeleteRewardZones()
{

}

std::list<RewardZone*> M_RewardZoneManager::GetRewardZones() const
{
	return this->reward_zones;
}