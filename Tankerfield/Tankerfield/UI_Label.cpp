#include "Defs.h"
#include "Log.h"
#include "UI_Label.h"
#include "M_Render.h"
#include "M_Fonts.h"
#include "M_Textures.h"
#include "M_Map.h"
#include "App.h"

UI_Label::UI_Label(const fPoint position, const UI_LabelDef definition, UI_Listener* listener): UI_Element(position, definition, listener), font(definition.font)
{
	color_mod = definition.color;
	SetText(definition.text);
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
	if (this->text == text)
	{
		return;
	}

	if (label_texture != nullptr)
	{
		app->tex->UnLoad(label_texture, true);
		label_texture = nullptr;
	}
	 
	if (text.empty())
	{
		LOG("Label text is null");
		return;
	}

	
	this->text = text;
	app->font->CalcSize( text.c_str() , sprite_rect.w, sprite_rect.h, font);
	label_texture = app->font->Print(text.c_str(), {255, 255, 255, 255}, font);
}

bool UI_Label::Draw()
{
	SDL_Rect draw_rect = GetDrawRect();

	if (label_texture != nullptr)
	{
		SDL_SetTextureColorMod(label_texture, color_mod.r, color_mod.g, color_mod.b);
		app->render->BlitUI(label_texture, draw_rect.x, draw_rect.y, &sprite_rect, app->ui->current_camera, (int)alpha);
		SDL_SetTextureColorMod(label_texture, 255, 255, 255);
	}

	return true;
}





