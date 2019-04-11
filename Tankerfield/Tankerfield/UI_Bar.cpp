#include "UI_Bar.h"
#include "M_Render.h"
#include "M_UI.h"
#include "App.h"

UI_Bar::UI_Bar(fPoint position, UI_BarDef definition, UI_Listener* listener) : UI_Element(position, definition, listener)
, direction(definition.direction), color_1(definition.color_1), color_2(definition.color_2)
{
}


UI_Bar::~UI_Bar()
{

}

bool UI_Bar::Draw()
{
	fRect back_rect = GetSection();
	SDL_Rect     percent_rect;

	switch (direction)
	{
	case UI_Bar::DIR::RIGHT:
		percent_rect = { (int)back_rect.GetLeft(), (int)back_rect.GetTop(), (int)(section_width * value),(int) section_height };
		break;
	case UI_Bar::DIR::LEFT:
		percent_rect = { (int) (back_rect.GetRight() - section_width * value) , (int)back_rect.GetTop(), (int)(section_width * value),(int)section_height };
		break;
	case UI_Bar::DIR::DOWN:
		percent_rect = { (int)(back_rect.GetLeft()) , (int)back_rect.GetTop(), (int) section_width, (int)(section_height * value)};
		break;
	case UI_Bar::DIR::UP:
		percent_rect = { (int)(back_rect.GetLeft()) , (int)(back_rect.GetBottom() - section_height * value) , (int)section_width, (int)(section_height * value) };
		break;
	}

	// Draw back rect
	app->render->DrawQuad((SDL_Rect)back_rect, color_2.r, color_2.g, color_2.b, color_2.a, true, false);
	// Draw percent rect
	app->render->DrawQuad(percent_rect, color_1.r, color_1.g, color_1.b, color_1.a, true, false);

	return true;
}

