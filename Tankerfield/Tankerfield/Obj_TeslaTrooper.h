#ifndef __OBJ_TESLATROOPER_H__
#define __OBJ_TESLATROOPER_H__

#include "Point.h"
#include "Animation.h"
#include "Object.h"

struct SDL_Texture;
class Timer;

class Obj_TeslaTrooper : public Object 
{
public:
	Obj_TeslaTrooper(fPoint pos);
	~Obj_TeslaTrooper();

public:
	bool Start() override;
	bool PreUpdate() override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() { return true; };
	bool Awake(pugi::xml_node&) { return true; };

private:
	int life = 100;
	int damage = 100;
	float speed = 1.0f;

	bool death = false;

	Animation* walking = nullptr;
	float angle = 0.0f;
	float new_current_frame = 0.0f;
	static SDL_Texture* tex;
	Timer timer;

	Object* target = nullptr;
	std::vector<iPoint> path;
};

#endif // !__TESLATROOPER_H__

