#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "SDL/include/SDL.h"
#include "Defs.h"
#include "Point.h"
#include "Obj_Tank.h"
#include <random>

class Camera
{
public:
	Camera();

	//Camera shake
	void AddTrauma(float value);

private:
	//Follow player
	void FollowPlayer(float dt, Obj_Tank * player);

	//Camera shake
	void ResetCamera();//Removes the "shake" on the camera so it can be moved normally
	void ShakeCamera(float dt);
	float GetRandomValue(float min_value, float max_value);
	float GetShakeAmount() const;

public:
	iPoint unaltered_pos	= { 0, 0 };//The real position of the camera. Without adding the screen shake.
	SDL_Rect rect			= { 0, 0, 0, 0 };//The actual camera coordinates in the world
	SDL_Rect screen_section	= { 0, 0, 0, 0 };//The section on the screen it covers (ex. player one gets 0, 0, w/2, h/2)
	bool assigned			= false;
	uint number_player		= 0u;

private:
	//Follow player
	float lerp_factor		= 0.f;
	float aim_distance		= 0.f;

	//Camera shake
	std::random_device random_device;	//Will be used to obtain a seed for the random number engine
	std::mt19937 random_generator;		//Standard mersenne_twister_engine seeded with rd()
	float trauma			= 0.f;
	float max_shake_offset	= 0.f;//The maximum amount the camera can be displaced from its original position (unaltered_pos)
	float trauma_decay		= 0.f;

	friend class Obj_Tank;
};

#endif // !__CAMERA_H__
