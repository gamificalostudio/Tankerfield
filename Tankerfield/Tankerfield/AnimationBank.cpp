#include "AnimationBank.h"

Frames * M_AnimationBank::LoadFrames(pugi::xml_node node)
{
	Frames * frames = nullptr;
	std::string anim_name = node.text().as_string();
	std::map<std::string, Frames*>::iterator iter = frames_map.find(anim_name);
	if (iter != frames_map.end())//If the frame has been loaded
	{
		frames = iter->second;
	}
	else
	{
		frames = frames_map[anim_name] = new Frames(node);
	}

	return frames;
}

bool M_AnimationBank::CleanUp()
{
	for (std::map<std::string, Frames*>::iterator iter = frames_map.begin(); iter != frames_map.end();)
	{
		delete (*iter).second;
		iter = frames_map.erase(iter);
	}
	frames_map.clear();

	return true;
}