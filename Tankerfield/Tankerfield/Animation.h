#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "SDL/include/SDL_rect.h"
#include "Defs.h"
#include <vector>
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"
#include "MathUtils.h"

enum ROTATION_DIR {
	CLOCKWISE,
	COUNTER_CLOCKWISE,
	INVALID
};

//Variables which all of the repeated animations share
class Frames
{

public:
	Frames(pugi::xml_node anim_node)
	{
		LoadAnimation(anim_node);
	}

	bool LoadAnimation(pugi::xml_node const & node)
	{
		//Load animation variables
		speed = node.attribute("speed").as_float();
		loop = node.attribute("loop").as_bool(true);
		std::string rotation_string = node.attribute("rotation").as_string();
		if (rotation_string == "clockwise")
		{
			rotation = ROTATION_DIR::CLOCKWISE;
		}
		else if (rotation_string == "counter-clockwise")
		{
			rotation = ROTATION_DIR::COUNTER_CLOCKWISE;
		}
		first_dir_angle = node.attribute("first_dir_angle").as_float();;

		//Load animation frames
		uint dir_num = 0u;
		for (pugi::xml_node dir_iter = node.child("dir"); dir_iter; dir_iter = dir_iter.next_sibling("dir"))
		{
			rects.push_back(std::vector<SDL_Rect>());
			uint frame_num = 0u;
			for (pugi::xml_node frame_iter = dir_iter.child("frame"); frame_iter; frame_iter = frame_iter.next_sibling("frame"))
			{
				SDL_Rect new_frame =
				{
					frame_iter.attribute("x").as_int(),
					frame_iter.attribute("y").as_int(),
					frame_iter.attribute("w").as_int(),
					frame_iter.attribute("h").as_int()
				};
				rects[dir_num].push_back(new_frame);
				frame_num++;
				if (frame_num > max_frames) { max_frames = frame_num; }
			}
			dir_num++;
			if (dir_num > max_dirs) { max_dirs = dir_num; }
		}
		return true;
	}

	//TODO: Assert if there is a different number of frames in each direction

	//Used before loading rects
	//Resizes the std::2Dvector frames so that it doesn't need to change size when rects are loaded00
	void Resize(uint directions, uint frames_per_direction)
	{
		rects.resize(directions);
		for (uint i = 0u; i < directions; ++i)
		{
			rects[i].resize(frames_per_direction);
		}
		max_frames = frames_per_direction - 1;
	}

private:
	//	 direction,  current_frame
	std::vector<std::vector <SDL_Rect>> rects;
	int max_dirs			= 0;
	int max_frames			= 0;
	bool loop				= true;
	float first_dir_angle	= 0.f;//The angle on the first sprite
	ROTATION_DIR rotation	= ROTATION_DIR::COUNTER_CLOCKWISE;
	float speed				= 1.0f;

	friend class Animation;
};

//Variables unique to each of the repeated animation
class Animation
{
public:
	Frames * frames			= nullptr;
	float current_frame		= 0.f;
	int loops				= 0;



public:
	void NextFrame(float dt)
	{
		current_frame += frames->speed * dt;
		if (current_frame >= frames->max_frames)
		{
			current_frame = (frames->loop) ? 0.0f : frames->max_frames - 1;
			loops++;
		}
	}

	SDL_Rect & GetFrame(float angle)
	{
		uint ind = GetRotatedIndex(angle);
		return frames->rects[ind][(uint)current_frame];
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
		frames->speed = 0.08f;
	}

	void Stop()
	{
		frames->speed = 0.0f;
	}

private:

	//angle should be in degrees
	uint GetRotatedIndex(float angle)
	{
		//Avoid all the calculations if it only has one frame
		if (frames->max_dirs == 1)
		{
			return 0;
		}

		//Account for the spritesheet not starting at the 0 degree rotation
		angle -= frames->first_dir_angle;

		angle = ClampRotation(angle);
		float ind = round((angle * frames->max_dirs) / 360.f);

		if (frames->rotation == ROTATION_DIR::CLOCKWISE) {
			ind = frames->max_dirs - ind;
		}

		//If it's the last frame, start over again
		if (ind == frames->max_dirs)
		{
			ind = 0.f;
		}
		return (uint)ind;
	}
};

#endif // __ANIMATION_H__