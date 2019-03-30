#ifndef _IMAGE_H__
#define _IMAGE_H__

#include "p2Point.h"
#include "Animation.h"
#include "UI_Element.h"

struct UI_ImageDef : public UI_ElementDefinition
{
	SDL_Rect draw_rect;
	bool is_animated = false;
	Animation image_animation;
};

class UI_Image: public UI_Element
{

public:
	UI_Image(const fPoint position, const UI_ImageDef draw_rect, UI_Listener* listener);

	virtual ~UI_Image();

	bool Draw();

private:

	SDL_Rect draw_rect;
};

#endif // _IMAGE_H__

