#ifndef __M_RewardZoneManager_H__
#define __M_RewardZoneManager_H__

#include <list>

#include "Module.h"
#include "Point.h"

class RewardZone
{
public:
	RewardZone();
	~RewardZone();

private:

};

class M_RewardZoneManager : public Module
{
public:
	M_RewardZoneManager();
	~M_RewardZoneManager();

	bool Awake(pugi::xml_node& config) override;

	bool Start() override;

	bool PreUpdate() override;

	bool Update(float dt) override;

	bool PostUpdate(float dt) override;

	bool CleanUp() override;

	RewardZone* CreateRewardZone(iPoint map_center_pos, uint size);

	void DeleteRewardZones();

	std::list<RewardZone*> GetRewardZones() const;

private:
	std::list<RewardZone*> reward_zones;
};

#endif /* __M_RewardZoneManager_H__ */
