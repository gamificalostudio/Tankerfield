#include "M_AnimationBank.h"

Frames * M_AnimationBank::LoadFrames(pugi::xml_node node)
{
	Frames * frames = nullptr;

	std::string final_name = node.parent().parent().name() + std::string("_") + node.name();

	std::map<std::string, Frames*>::iterator iter = frames_map.find(final_name);
	if (iter != frames_map.end())//If the frame has been loaded
	{
		frames = iter->second;
	}
	else
	{
		frames = frames_map[final_name] = DBG_NEW Frames(node);
	}

	return frames;
}

bool M_AnimationBank::UnloadFrames(Frames * frames)
{
	for (std::map<std::string, Frames *>::iterator iter = frames_map.begin(); iter != frames_map.end(); ++iter)
	{
		if ((iter->second) == frames)
		{
			delete(frames);
			frames = nullptr;
			frames_map.erase(iter);
			return true;
		}
	}
}

bool M_AnimationBank::CleanUp()
{
	for (std::map<std::string, Frames*>::iterator iter = frames_map.begin(); iter != frames_map.end();)
	{
		delete iter->second;
		iter->second = nullptr;
		iter = frames_map.erase(iter);
	}
	frames_map.clear();

	return true;
}