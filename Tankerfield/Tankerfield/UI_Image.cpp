#include "UI_Image.h"
#include "M_Render.h"
#include "M_UI.h"
#include "App.h"

UI_Image::UI_Image(fPoint position, UI_ImageDef definition, UI_Listener* listener) : UI_Element(position, definition, listener), image_animation(definition.image_animation)
{

}


UI_Image::~UI_Image()
{

}

bool UI_Image::Draw()
{
	SDL_Rect draw_rect;

	if (SDL_RectEmpty(&custom_draw_rect))
	{
		draw_rect = GetDrawRect();
	}
	else
	{
		draw_rect = custom_draw_rect;
	}

	SDL_SetTextureColorMod(GetTexture(), color_mod.r, color_mod.g, color_mod.b);
	app->render->BlitCustomUI(GetTexture(), &sprite_rect, &draw_rect, app->ui->current_camera, (int)alpha);
	SDL_SetTextureColorMod(GetTexture(), 255, 255, 255);

	return true;
}

void UI_Image::SetTexture(SDL_Texture * texture)
{
	this->texture = texture;
}

void UI_Image::SetDrawRect(SDL_Rect draw_rect)
{
	custom_draw_rect = draw_rect;
}

SDL_Texture * UI_Image::GetTexture()
{
	if (texture != nullptr)
	{
		return texture;
	}
	else
	{
		return app->ui->GetAtlas();
	}
}

