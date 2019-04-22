#ifndef __M_RewardZoneManager_H__
#define __M_RewardZoneManager_H__

#include <list>

#include "Module.h"
#include "Point.h"

class Object;

class RewardZone
{
public:
	RewardZone();
	~RewardZone();

	std::list<Object*> enemy_list;

public:
	fPoint GetPos() const;
	uint GetSize() const;
	uint GetNumberOfUnits() const;

	void SetPos(fPoint pos);
	void SetSize(const uint& size);
	void SetNumberOfUnits(const uint& nou);

private:
	// Position in tile units
	fPoint position = { 0.0f, 0.0f };
	uint size = 0u;
	uint number_of_units = 0u;
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

	RewardZone* CreateRewardZone(fPoint map_center_pos, uint size);

	void DeleteRewardZones();

	std::list<RewardZone*> GetRewardZones() const;

public: // Change to private when fixed Getter error
	std::list<RewardZone*> reward_zones;

	float accumulated_time = 0.0f;
	bool do_check = false;
};

#endif /* __M_RewardZoneManager_H__ */
