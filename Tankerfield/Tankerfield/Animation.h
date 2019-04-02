#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "SDL/include/SDL_rect.h"
#include "Defs.h"
#include <vector>

class Animation
{
public:
	bool loop = true;
	float speed = 1.0f;
	std::vector<std::vector <SDL_Rect*>> frames;
	//First position is the direction that the object is facing
	//Second position is the frame

public:
	float current_frame=0;
	int last_frame = 0;
	int loops = 0;

public:
	Animation(uint directions, uint frames_per_direction)
	{

	}

	SDL_Rect& GetCurrentFrame(float angle, float dt)
	{
		current_frame += speed * dt;
		if (current_frame >= last_frame)
		{
			current_frame = (loop) ? 0.0f : last_frame - 1;
			loops++;
		}
		return frames[(int)current_frame].;
	}

	//Used before loading rects
	//Resizes the std::2Dvector frames so that it doesn't need to change size when rects are loaded00
	void Resize()
	{
		frames.resize(directions);
		for (uint i = 0u; i < directions; ++i)
		{
			frames[i].resize[frames_per_direction];
		}
		last_frame = frames_per_direction - 1;
	}
	
	//This overloaded GetCurrentFrame is used for Animation arrays where the frame number must concide
	SDL_Rect& GetCurrentFrame(float dt, float & new_current_frame)
	{
		new_current_frame += speed*dt;
		if (new_current_frame >= last_frame)
		{
			new_current_frame = (loop) ? 0.0f : last_frame - 1;
			loops++;
		}
		return frames[(int)new_current_frame];
	}//What's most eacy to use? Create a method to allocate size? Create a method to get the nimber of xml lines?

	bool Finished() const
	{
		return loops > 0;
	}

	void Reset()
	{
		current_frame = 0;
		loops = 0;
	}

	void Start()
	{
		speed = 0.08f;
	}

	void Stop()
	{
		speed = 0.0f;
	}

private:
	//angle should be in degrees
	uint GetRotatedIndex(uint rect_num, float angle, ROTATION_DIR rot_dir, float fist_rect_dir)
	{
		//Account for the spritesheet not starting at the 0 degree rotation
		angle -= fist_rect_dir;
		angle = ClampRotation(angle);
		float ind = (angle * rect_num) / 360;
		if (fmod(ind, 1) >= 0.5f)
		{
			ceil(ind);
		}
		else
		{
			floor(ind);
		}
		//If it's the last frame, start over again
		if (ind == rect_num)
		{
			ind = 0;
		}
		return (uint)ind;
	}

	float ClampRotation(float angle)
	{
		if (angle > 360)
		{
			angle = fmod(angle, 360);
		}
		else if (angle < -360)
		{
			angle = fmod(angle, -360);
		}
		if (angle < 0)
		{
			angle += 360;
		}
		return angle;
	}

};

#endif // __ANIMATION_H__