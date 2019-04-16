#ifndef __ANIMATION_BANK_H__
#define __ANIMATION_BANK_H__

#include <map>
#include <string>

#include "Animation.h"
#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"
#include "Module.h"

class M_AnimationBank : public Module
{
public:
	bool CleanUp() override;

	Frames* LoadFrames(pugi::xml_node node);

private:

	std::map<std::string, Frames*> frames_map;
};

#endif