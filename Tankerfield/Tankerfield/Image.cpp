#include "Image.h"
#include "j1Render.h"
#include "Module_UI.h"
#include "j1App.h"

Image::Image(iPoint position, SDL_Rect draw_rect, Gui_Listener* listener) : UI_Object(position, listener)
{
	//this->animation.PushBack({ draw_rect });
}


Image::~Image()
{

}

bool Image::Draw()
{
	// TODO 2: Add animation.h methods

	//SDL_Rect anim_rect = animation.GetLastFrame();

	//section.w = anim_rect.w;
	//section.h = anim_rect.h;
	//App->render->Blit( texture ,  position.x - section.w*0.5f , position.y  - section.h *0.5f , &anim_rect, false, 0.0f);

	return true;
}

