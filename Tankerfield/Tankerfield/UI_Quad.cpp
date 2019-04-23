#include "UI_Quad.h"
#include "App.h"
#include "M_Render.h"

UI_Quad::UI_Quad(const fPoint position, const UI_QuadDef def, UI_Listener * listener) : UI_Element(position, def, listener), color(def.color)
{
}

bool UI_Quad::Draw()
{
	SDL_Rect draw_rect = GetDrawRect();
	if (is_in_game == true)
	{
		is_in_game = true;
	}
	app->render->DrawQuadUI(draw_rect, app->ui->current_camera, { color.r, color.g, color.b,  (Uint8)(color.a * (alpha / 255.F)) });

	return true;
}
