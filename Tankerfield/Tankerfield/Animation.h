#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "SDL/include/SDL_rect.h"
#include "Defs.h"
#include <vector>
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

enum ROTATION_DIR {
	CLOCKWISE,
	COUNTER_CLOCKWISE,
	INVALID
};

class Animation
{
public:
	//frames[direction, current_frame]
	std::vector<std::vector <SDL_Rect>> frames;

	float speed				= 1.0f;
	float current_frame		= 0.f;

	int max_dirs			= 0;
	int max_frames			= 0;

	bool loop				= true;
	int loops				= 0;

	float first_dir_angle	= 0.f;//The angle on the first sprite
	ROTATION_DIR rotation	= ROTATION_DIR::COUNTER_CLOCKWISE;

public:
	Animation()
	{
	}

	Animation(pugi::xml_node const & node)
	{
		LoadAnimation(node);
	}

	void NextFrame(float dt)
	{
		current_frame += speed * dt;
		if (current_frame >= max_frames)
		{
			current_frame = (loop) ? 0.0f : max_frames - 1;
			loops++;
		}
	}

	SDL_Rect & GetFrame(float angle)
	{
		uint ind = GetRotatedIndex(angle);
		return frames[ind][(uint)current_frame];
	}

	bool LoadAnimation(pugi::xml_node const & node)
	{
		//Load animation variables
		speed = node.attribute("speed").as_float();
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
			frames.push_back(std::vector<SDL_Rect>());
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
				frames[dir_num].push_back(new_frame);
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
		frames.resize(directions);
		for (uint i = 0u; i < directions; ++i)
		{
			frames[i].resize(frames_per_direction);
		}
		max_frames = frames_per_direction - 1;
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

private:

	//angle should be in degrees
	uint GetRotatedIndex(float angle)
	{
		//Avoid all the calculations if it only has one frame
		if (max_dirs == 1)
		{
			return 0;
		}

		//Account for the spritesheet not starting at the 0 degree rotation
		angle -= first_dir_angle;

		angle = ClampRotation(angle);
		float ind = round((angle * max_dirs) / 360.f);

		//If it's the last frame, start over again
		if (ind == max_dirs)
		{
			ind = 0.f;
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