#include "Defs.h"
#include "Log.h"
#include "UI_Label.h"
#include "M_Render.h"
#include "M_Fonts.h"
#include "M_Textures.h"
#include "M_Map.h"
#include "App.h"

UI_Label::UI_Label(const fPoint position, const UI_LabelDef definition, UI_Listener* listener): UI_Element(position, definition, listener), text(definition.text), font(definition.font), color(definition.color)
{
	SetText(text);
}

void UI_Label::Destroy()
{
	to_destroy = true;

	if (label_texture != nullptr)
	{
		app->tex->UnLoad(label_texture, true);
	}
}

void UI_Label::SetText(String text)
{
	if (label_texture != nullptr)
	{
		app->tex->UnLoad(label_texture, true);
		label_texture = nullptr;
	}
	 
	if (text == "")
	{
		LOG("Label text is null");
		return;
	}

	app->font->CalcSize( text.c_str() , sprite_section.w, sprite_section.h, font);
	label_texture = app->font->Print( text.c_str(), color, font);
}

bool UI_Label::Draw()
{
	SDL_Rect draw_rect = GetDrawRect();

	if (label_texture != nullptr)
	{
		app->render->BlitUI(label_texture, draw_rect.x, draw_rect.y, &sprite_section, app->ui->current_camera, (int)alpha);
	}

	return true;
}





