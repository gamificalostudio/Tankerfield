#ifndef __ELECTRO_SHOT_ANIMATION_H__
#define __ELECTRO_SHOT_ANIMATION_H__

#include "Animation.h"
#include "Object.h"

struct SDL_Texture;

class Eletro_Shot_Animation : public Object
{
public:

	Eletro_Shot_Animation(fPoint pos);
	~Eletro_Shot_Animation();

	bool Update(float dt);

	bool Draw(float dt, Camera* camera) override;

public:
	//Obj_Tank * tank = nullptr;
	fPoint player_enemy_distance_point{0.f,0.f};
	fPoint offset_dir_screen{ 0.f,0.f };
	float distance = 0.f;

private:
	Animation anim;
	SDL_Texture* tex;
};

#endif
