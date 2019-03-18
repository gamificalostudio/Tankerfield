#ifndef _IMAGE_H__
#define _IMAGE_H__

#include "p2Point.h"
#include "Animation.h"
#include "UI_Object.h"

class Image: public UI_Object
{

public:
	Image(const iPoint position, const SDL_Rect draw_rect, Gui_Listener* listener);

	virtual ~Image();

	bool Draw();

private:

	SDL_Rect draw_rect;
};

#endif // _IMAGE_H__

