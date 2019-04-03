#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_Window.h"
#include "M_Render.h"
#include "M_Map.h"
#include "Brofiler/Brofiler.h"
#include "SDL/include/SDL_rect.h"

M_Render::M_Render() : Module()
{
	name.assign("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
M_Render::~M_Render()
{}

// Called before render is available
bool M_Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if (config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(app->win->window, -1, flags);

	if (renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = app->win->screen_surface->w;
		camera.h = app->win->screen_surface->h;
		camera.pos.x = 0;
		camera.pos.y = 0;
	}

	return ret;
}

// Called before the first frame
bool M_Render::Start()
{
	LOG("render start");
	// back background
	SDL_Rect viewport_rect;
	viewport_rect.x = viewport.pos.x;
	viewport_rect.y = viewport.pos.y;
	viewport_rect.w = viewport.w;
	viewport_rect.h = viewport.h;
	SDL_RenderGetViewport(renderer, &viewport_rect);
	return true;
}

// Called each loop iteration
bool M_Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool M_Render::PostUpdate(float dt)
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool M_Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool M_Render::Load(pugi::xml_node& data)
{
	camera.pos.x = data.child("camera").attribute("x").as_int();
	camera.pos.y = data.child("camera").attribute("y").as_int();

	return true;
}

// Save Game State
bool M_Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.pos.x;
	cam.append_attribute("y") = camera.pos.y;

	return true;
}

void M_Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void M_Render::SetViewPort(const iRect& rect)
{
	SDL_RenderSetViewport(renderer, &(SDL_Rect)rect);
}

void M_Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &(SDL_Rect)viewport);
}

iPoint M_Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = app->win->GetScale();

	ret.x = (x - camera.pos.x / scale);
	ret.y = (y - camera.pos.y / scale);

	return ret;
}

// Blit to screen
bool M_Render::Blit(SDL_Texture* texture, int screen_x, int screen_y, const SDL_Rect* section, float speed, double angle, int pivot_x, int pivot_y) const
{
	BROFILER_CATEGORY("M_RenderBlit", Profiler::Color::DarkBlue)
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(-camera.pos.x * speed) + screen_x * scale;
	rect.y = (int)(-camera.pos.y * speed) + screen_y * scale;

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to main_object. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool M_Render::DrawQuad(const iRect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if (use_camera)
	{
		rec.x = (int)(-camera.pos.x + rect.pos.x * scale);
		rec.y = (int)(-camera.pos.y + rect.pos.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if (result != 0)
	{
		LOG("Cannot draw quad to main_object. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool M_Render::DrawIsometricQuad(float x, float y, float w, float h, SDL_Color color)
{
	fPoint point_1, point_2, point_3, point_4;

	// top_left 
	point_1 = app->map->MapToScreenF({x, y});
	// top_right
	point_2 = app->map->MapToScreenF({x + w, y});
	// bot_right
	point_3 = app->map->MapToScreenF({x + w, y + h});
	// bot_left
	point_4 = app->map->MapToScreenF({x, y + h});

	app->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, color.r, color.g, color.b, color.a, true);
	app->render->DrawLine(point_2.x, point_2.y, point_3.x, point_3.y, color.r, color.g, color.b, color.a, true);
	app->render->DrawLine(point_3.x, point_3.y, point_4.x, point_4.y, color.r, color.g, color.b, color.a, true);
	app->render->DrawLine(point_4.x, point_4.y, point_1.x, point_1.y, color.r, color.g, color.b, color.a, true);

	return true;
}

bool M_Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if (use_camera)
		result = SDL_RenderDrawLine(renderer, -camera.x + x1 * scale, -camera.y + y1 * scale, -camera.x + x2 * scale, -camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if (result != 0)
	{
		LOG("Cannot draw quad to main_object. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool M_Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	if (use_camera)
	{
		for (uint i = 0; i < 360; ++i)
		{
			points[i].x = (int)(-camera.x + x + radius * cos(i * factor));
			points[i].y = (int)(-camera.y + y + radius * sin(i * factor));
		}
	}
	else
	{
		for (uint i = 0; i < 360; ++i)
		{
			points[i].x = (int)(x + radius * cos(i * factor));
			points[i].y = (int)(y + radius * sin(i * factor));
		}
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if (result != 0)
	{
		LOG("Cannot draw quad to main_object. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool M_Render::IsOnCamera(const int & x, const int & y, const int & w, const int & h) const
{
	int scale = app->win->GetScale();

	iRect r = { x*scale,y*scale,w*scale,h*scale };
	iRect cam = { camera.x,camera.y,camera.w,camera.h };

	return SDL_HasIntersection(&r, &cam);
}