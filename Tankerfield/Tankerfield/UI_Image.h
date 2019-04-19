#ifndef __UI_IMAGE_H__
#define __UI_IMAGE_H__

#include "Point.h"
#include "Animation.h"
#include "UI_Element.h"

struct UI_ImageDef : public UI_ElementDef
{
	UI_ImageDef( ){}
	UI_ImageDef(const SDL_Rect sprite_section): UI_ElementDef(sprite_section){}
	bool is_animated = false;
	Animation* image_animation = nullptr;
};

class UI_Image: public UI_Element
{

public:

	UI_Image(const fPoint position, const UI_ImageDef def, UI_Listener* listener);

	virtual ~UI_Image();

public:

	Animation* image_animation = nullptr;
};

#endif // _IMAGE_H__

