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
	M_AnimationBank();

	bool Awake(pugi::xml_node& config);

	bool CleanUp() override;

	Frames* LoadFrames(pugi::xml_node node);

	bool UnloadFrames(Frames * frames);

	//XML document
	pugi::xml_node		animations_xml_node;

private:
	pugi::xml_document animations_xml_doc;
	std::map<std::string, Frames*> frames_map;
};

#endif