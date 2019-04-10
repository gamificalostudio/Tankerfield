#include "UI_Image.h"
#include "M_Render.h"
#include "M_UI.h"
#include "App.h"

UI_Image::UI_Image(fPoint position, UI_ImageDef definition, UI_Listener* listener) : UI_Element(position, definition, listener) , draw_rect(definition.draw_rect)
{

}


UI_Image::~UI_Image()
{

}

bool UI_Image::Draw()
{
	// TODO 2: Add animation.h methods

	//SDL_Rect anim_rect = animation.GetLastFrame();

	//section.w = anim_rect.w;
	//section.h = anim_rect.h;
	//App->render->Blit( texture ,  position.x - section.w*0.5f , position.y  - section.h *0.5f , &anim_rect, false, 0.0f);

	return true;
}

