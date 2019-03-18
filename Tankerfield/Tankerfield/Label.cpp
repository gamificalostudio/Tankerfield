#include "p2Defs.h"
#include "p2Log.h"
#include "Label.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1Textures.h"
#include "j1App.h"

Label::Label(const fPoint position, const String text, Label_Definition definition, Gui_Listener* listener): UI_Object(position, definition, listener), font(definition.font), color(definition.color)
{
	SetText(text);

}

Label::~Label()
{
	if (label_texture != nullptr)
	{
		App->tex->UnLoad(label_texture);
	}
}

void Label::SetText(String text)
{
	if (label_texture != nullptr)
	{
		App->tex->UnLoad(label_texture);
		label_texture = nullptr;
	}

	App->font->CalcSize( text.c_str() , section.w, section.h, font);
	section.x = section.y = 0;
	label_texture = App->font->Print( text.c_str(), color, font);
}

bool Label::Draw()
{
	App->render->Blit(label_texture, position.x- section.w * 0.5f, position.y - section.h * 0.5f , &section, false, 0.0f);

	return false;
}






