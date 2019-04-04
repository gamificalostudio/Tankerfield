#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_Input.h"
#include "M_Window.h"
#include "M_Render.h"
#include "M_Map.h"
#include "Brofiler/Brofiler.h"

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
		camera.w = app->win->screen_surface->w/2;
		camera.h = app->win->screen_surface->h/2;
		camera.x = 0;
		camera.y = 0;
	}

	return ret;
}

// Called before the first frame
bool M_Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
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
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		debug = !debug;
		if (!debug)
		{
			camera.w = app->win->screen_surface->w;
			camera.h = app->win->screen_surface->h;
		}
		else
		{
			camera.w *= 0.5f;
			camera.h *= 0.5f;
		}
	}

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
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();

	return true;
}

// Save Game State
bool M_Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void M_Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void M_Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void M_Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

iPoint M_Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = app->win->GetScale();

	ret.x = (x - camera.x / scale);
	ret.y = (y - camera.y / scale);

	return ret;
}

// Blit to screen
bool M_Render::Blit(SDL_Texture* texture, int screen_x, int screen_y, const SDL_Rect* section, float speed, double angle, int pivot_x, int pivot_y) const
{
	//BROFILER_CATEGORY("M_RenderBlit", Profiler::Color::DarkBlue)
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(-camera.x * speed) + screen_x * scale;
	rect.y = (int)(-camera.y * speed) + screen_y * scale;

	SDL_Rect sect(*section);
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

	//Don't blit if the sprite is out of the screen
	uint width, height = 0;
	app->win->GetWindowSize(width, height);
	SDL_Rect cam;
	cam.x = 0;
	cam.y = 0;
	cam.w = camera.w;
	cam.h = camera.h;

	if (SDL_HasIntersection(&rect, &cam))
	{
		SDL_Point* p = NULL;
		SDL_Point pivot;

		if (pivot_x != INT_MAX && pivot_y != INT_MAX)
		{
			pivot.x = pivot_x;
			pivot.y = pivot_y;
			p = &pivot;
		}
		if (rect.x + rect.w >= cam.w)
		{
	
			sect.w = cam.w - rect.x;
			rect.w = cam.w - rect.x;
		}
		if (rect.y + rect.h >= cam.h)
		{
			sect.h = cam.h - rect.y;
			rect.h = cam.h - rect.y;
		}
		if (rect.x < 0)
		{
			float d = - rect.x;
			rect.x = 0;
			sect.x = d;
			sect.w -= d;
			rect.w -= d;
			
			//rect.x = 0;
		}
		if (rect.y < 0)
		{
			float d = -rect.y;
			rect.y = 0;
			sect.y = d;
			sect.h -= d;
			rect.h -= d;
		}
		for (uint i = 1; i <= 4; ++i)
		{
			SDL_Rect rect_cam(rect);
			if (debug)
			{
				switch (i)
				{
				case 1:

					break;
				case 2:
					rect_cam.x += cam.w;
					break;
				case 3:
					rect_cam.y += cam.h;
					break;
				case 4:
					rect_cam.x += cam.w;
					rect_cam.y += cam.h;
					break;
				}
			}
			//rect_cam.x += 100;
			//sect.x += 100;
			DrawLine(camera.x + camera.w, 0, camera.x+camera.w, 2000,0,0,0);
			DrawLine(0, camera.y + camera.h, 2000, camera.y + camera.h, 0, 0, 0);
			if (SDL_RenderCopyEx(renderer, texture, &sect, &rect_cam, angle, p, SDL_FLIP_NONE) != 0)
			{
				LOG("Cannot blit to main_object. SDL_RenderCopy error: %s", SDL_GetError());
				ret = false;
			}
		}
		
	}
	else {
		ret = false;
	}

	return ret;
}

bool M_Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if (use_camera)
	{
		rec.x = (int)(-camera.x + rect.x * scale);
		rec.y = (int)(-camera.y + rect.y * scale);
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

void M_Render::BlitInScreen2(SDL_Rect& rect)
{
	rect.x += rect.w;
}

void M_Render::BlitInScreen3(SDL_Rect& rect)
{
	rect.y += rect.h;
}

void M_Render::BlitInScreen4(SDL_Rect& rect)
{
	rect.x += rect.w;
	rect.y += rect.h;
}