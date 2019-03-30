#ifndef __TESLATROOPER_H__
#define __TESLATROOPER_H__

#include "p2Point.h"
#include "Animation.h"
#include "Object.h"


class TeslaTrooper : public Object 
{
public:
	TeslaTrooper(float x, float y);
	~TeslaTrooper();

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

#endif // !__TESLATROOPER_H__

