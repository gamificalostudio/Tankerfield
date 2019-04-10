#include "Defs.h"
#include "Log.h"
#include "UI_Label.h"
#include "M_Render.h"
#include "M_Fonts.h"
#include "M_Textures.h"
#include "App.h"

UI_Label::UI_Label(const fPoint position, const String text, UI_LabelDef definition, UI_Listener* listener): UI_Element(position, definition, listener), font(definition.font), color(definition.color)
{
	SetText(text);
}

UI_Label::~UI_Label()
{
	if (label_texture != nullptr)
	{
		app->tex->UnLoad(label_texture);
	}
}

void UI_Label::SetText(String text)
{
	if (label_texture != nullptr)
	{
		app->tex->UnLoad(label_texture);
		label_texture = nullptr;
	}

	app->font->CalcSize( text.c_str() , section.w, section.h, font);
	section.x = section.y = 0;
	label_texture = app->font->Print( text.c_str(), color, font);
}

bool UI_Label::Draw()
{
	app->render->Blit(label_texture, position.x- section.w * 0.5f, position.y - section.h * 0.5f , &section, false, 0.0f);

	return false;
}






