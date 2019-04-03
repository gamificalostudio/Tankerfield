#ifndef __OBJ_TESLATROOPER_H__
#define __OBJ_TESLATROOPER_H__

#include "Point.h"
#include "Animation.h"
#include "Object.h"


class Obj_TeslaTrooper : public Object 
{
public:
	Obj_TeslaTrooper(fPoint pos);
	~Obj_TeslaTrooper();

public:
	bool PreUpdate() override;
	bool Update(float dt) override;
	bool PostUpdate() override;
	bool CleanUp() { return true; };
	bool Awake(pugi::xml_node&) { return true; };

private:
	int life = 100;
	int damage = 100;
	float speed = 1.0f;

	bool death = false;
private:
	Animation* walking = nullptr;
	float angle = 0.0f;
	float new_current_frame = 0.0f;


};

// TODO REMOVE IT 
class Sensor : public Object
{
	Sensor();

	bool Start()
	{

		return true;
	}

	void OnTriggerEnter(Collider* c1)
	{
		LOG("ENTER");
	}
	void OnTrigger(Collider* c1)
	{
		LOG("BOOOOOOOOOOOOOOOI");
	}
	void OnTriggerExit(Collider* c1)
	{
		LOG("EXIT");
	}
};
#endif // !__TESLATROOPER_H__

