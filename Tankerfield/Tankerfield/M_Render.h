#ifndef __M_RENDER_H__
#define __M_RENDER_H__

#include "SDL/include/SDL.h"
#include "Point.h"
#include "Module.h"
#include "Camera.h"
#include <vector>

class M_Render : public Module
{
public:

	M_Render();
	
	// Destructor
	~M_Render();

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
	iPoint ScreenToWorld(int x, int y, const Camera* camera) const;

	// Draw & Blit
	void BlitUI(SDL_Texture * texture, int screen_x, int screen_y, const SDL_Rect * section = nullptr, Camera* current_camera = nullptr, const int alpha =  255) const;
	
	void Blit(SDL_Texture* texture, int screen_x, int screen_y, Camera* camera, const SDL_Rect* section = NULL) const;
	void BlitScaled(SDL_Texture* texture, int screen_x, int screen_y, Camera* camera, const SDL_Rect* section = NULL, float custom_scale = 1.f) const;


	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;

	bool DrawQuadUI(const SDL_Rect section, Camera * camera, const SDL_Color color);

	bool DrawIsometricQuad(float x, float y, float w, float h, SDL_Color color = {255,255,255,255}, const Camera* camera = nullptr);

	void DrawIsometricLine(fPoint point_1, fPoint point_2, SDL_Color color, const Camera* camera = nullptr);

	bool DrawLineSplitScreen( int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, const Camera* camera = nullptr) const;
	
	bool DrawCircle(int x1, int y1, int redius, Camera* camera, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool DrawIsoCircle(int x1, int y1, int redius, Camera* camera, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;

	bool IsOnCamera(const int & x, const int & y, const int & w, const int & h, Camera* camera) const;

	// Set background color
	void SetBackgroundColor(SDL_Color color);

public:

	SDL_Renderer*					renderer = nullptr;
	std::vector<Camera*>			cameras;
	std::vector<Camera*>			camera_saves;
	SDL_Rect						viewport;
	SDL_Color						background;

	bool debug = true;
};

#endif // __j1RENDER_H__