#ifndef __M_RENDER_H__
#define __M_RENDER_H__

#include "SDL/include/SDL.h"
#include "Point.h"
#include "Module.h"

class M_Render : public Module
{
public:

	M_Render();

	// Destructor
	virtual ~M_Render();

	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	// Called before the first frame
	bool Start() override;

	// Called each loop iteration
	bool PreUpdate() override;
	bool PostUpdate(float dt) override;

	// Called before quitting
	bool CleanUp() override;

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Utils
	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();
	iPoint ScreenToWorld(int x, int y) const;

	// Draw & Blit
	bool Blit(SDL_Texture* texture, int screen_x, int screen_y, const SDL_Rect* section = NULL, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;
	bool DrawIsometricQuad(float x, float y, float w, float h, SDL_Color color = {255,255,255,255});
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;

	bool IsOnCamera(const int & x, const int & y, const int & w, const int & h) const;

	void BlitInScreen2(SDL_Rect& rect);
	void BlitInScreen3(SDL_Rect& rect);
	void BlitInScreen4(SDL_Rect& rect);

	// Set background color
	void SetBackgroundColor(SDL_Color color);

public:

	SDL_Renderer*   renderer;
	SDL_Rect		camera;
	SDL_Rect		viewport;
	SDL_Color		background;

	bool debug = true;
};

#endif // __j1RENDER_H__