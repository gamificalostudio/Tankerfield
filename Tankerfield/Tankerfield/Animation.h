#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "Rect.h"
#include "Defs.h"

class Animation
{
public:
	bool loop = true;
	float speed = 1.0f;
	iRect * frames = nullptr;

public:
	float current_frame=0;
	int last_frame = 0;
	int loops = 0;

public:
	Animation(uint frame_num)
	{
		frames = new iRect[frame_num];
	}

	void PushBack(const iRect& rect)
	{
		frames[last_frame++] = rect;
	}

	iRect& GetCurrentFrame(float dt)
	{
		current_frame += speed*dt;
		if (current_frame >= last_frame)
		{
			current_frame = (loop) ? 0.0f : last_frame - 1;
			loops++;
		}
		return frames[(int)current_frame];
	}
	
	//This overloaded GetCurrentFrame is used for Animation arrays where the frame number must concide
	iRect& GetCurrentFrame(float dt, float & new_current_frame)
	{
		new_current_frame += speed*dt;
		if (new_current_frame >= last_frame)
		{
			new_current_frame = (loop) ? 0.0f : last_frame - 1;
			loops++;
		}
		return frames[(int)new_current_frame];
	}

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
};

#endif // __ANIMATION_H__