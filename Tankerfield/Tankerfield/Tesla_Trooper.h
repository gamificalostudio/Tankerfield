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
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate() { return true; };
	bool CleanUp() { return true; };
	bool Awake(pugi::xml_node&) { return true; };

private:
	
	Animation idle;
};

#endif // !__TESLATROOPER_H__

