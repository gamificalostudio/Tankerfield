#ifndef _IMAGE_H__
#define _IMAGE_H__

#include "p2Point.h"
#include "Animation.h"
#include "UI_Element.h"

struct Image_Definition : public UI_Object_Definition
{
	SDL_Rect draw_rect;
	bool is_animated = false;
	Animation image_animation;
};

class Image: public UI_Object
{

public:
	Image(const fPoint position, const Image_Definition draw_rect, Gui_Listener* listener);

	virtual ~Image();

	bool Draw();

private:

	SDL_Rect draw_rect;
};

#endif // _IMAGE_H__

