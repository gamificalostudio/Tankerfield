#include "UI_Quad.h"
#include "App.h"
#include "M_Render.h"

UI_Quad::UI_Quad(const fPoint position, const UI_QuadDef def, UI_Listener * listener) : UI_Element(position, def, listener), color(def.color)
{
}

bool UI_Quad::Draw()
{
	app->render->DrawQuad(sprite_section, color.r, color.g, color.b, color.a * (alpha / 255.F), true, false);
	return true;
}