#ifndef _OBJ_SPAWNPOINT_H_
#define _OBJ_SPAWNPOINT_H_
#include "Object.h"
#include "Animation.h"
#include "Timer.h"

struct SDL_Texture;
class Obj_SpawnPoint: public Object
{
public:
	Obj_SpawnPoint(fPoint pos);
	
	bool Start() override;
	bool Update(float dt) override;
	fPoint pos;
	bool occupied = false;
private:
	Animation portal;
	SDL_Texture* tex_portal= nullptr;
	Timer show_portal_timer;
};
#endif // _OBJ_SPAWNPOINT_H_

