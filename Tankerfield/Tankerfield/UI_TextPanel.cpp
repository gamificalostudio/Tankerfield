#include "Defs.h"
#include "Log.h"
#include "UI_TextPanel.h"
#include "M_Render.h"
#include "M_Fonts.h"
#include "M_Textures.h"
#include "App.h"

UI_TextPanel::UI_TextPanel(const fPoint position, UI_TextPanelDef definition, UI_Listener * listener): UI_Element(position, definition,listener)
{
	this->definition = definition;
	SetText(definition.text);
}

UI_TextPanel::~UI_TextPanel()
{
	if (text_texture != nullptr)
	{
		app->tex->UnLoad(text_texture);
	}
}

void UI_TextPanel::SetText(String text)
{
	
}

bool UI_TextPanel::Draw()
{

	return true;
}
