#include "UI_Bar.h"
#include "M_Render.h"
#include "M_UI.h"
#include "App.h"

UI_Bar::UI_Bar(fPoint position, UI_BarDef definition, UI_Listener* listener) : UI_Element(position, definition, listener)
, direction(definition.direction), color_1(definition.color_1), color_2(definition.color_2), value(definition.value)
{
	
}


UI_Bar::~UI_Bar()
{

}

bool UI_Bar::Draw()
{
	fRect back_rect = GetSection();
	SDL_Rect     percent_rect;

	// Draw back rect
	app->render->DrawQuad((SDL_Rect)back_rect, color_2.r, color_2.g, color_2.b, color_2.a *  (alpha/ 255.F), true, false);

	if (value > 1.f)
	{
		value = 1.f;
	}
	else if (value <= 0.f)
	{
		value = 0.f;
		return true;
	}

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

	// Draw percent rect
	app->render->DrawQuad(percent_rect, color_1.r, color_1.g, color_1.b, color_1.a * (alpha / 255.F), true, false);

	// Draw sprite 
	SDL_Rect draw_pos = GetDrawRect();
	app->render->BlitUI(app->ui->GetAtlas(), draw_pos.x, draw_pos.y, &sprite_section, app->ui->current_camera, alpha);

	return true;
}

