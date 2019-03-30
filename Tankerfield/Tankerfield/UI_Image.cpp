#include "UI_Image.h"
#include "M_Render.h"
#include "M_UI.h"
#include "App.h"

Image::Image(fPoint position, Image_Definition definition, Gui_Listener* listener) : UI_Object(position, definition, listener) , draw_rect(definition.draw_rect)
{

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

