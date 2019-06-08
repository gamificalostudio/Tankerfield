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
	void BlitUI(SDL_Texture * texture, int screen_x, int screen_y, const SDL_Rect * section = nullptr, Camera* current_camera = nullptr, const int alpha = 255, float scale_w = 1, float scale_h = 1) const;

	void BlitCustomUI(SDL_Texture * texture, const SDL_Rect * sprite_section, const SDL_Rect * draw_section, Camera * camera, const int alpha) const;

	void Blit(SDL_Texture* texture, int screen_x, int screen_y, Camera* camera, const SDL_Rect* section = NULL) const;
	void BlitScaled(SDL_Texture* texture, int screen_x, int screen_y, Camera* camera, const SDL_Rect* section = NULL, float scale_w = 1.f, float scale_h = 1.f) const;
	void BlitScaledAndRotated(SDL_Texture* texture, const int screen_x, const int screen_y, Camera* current_camera, const SDL_Rect* section, float scale_w = 1.f, float scale_h = 1.f, SDL_Point pivot= {0,0}, float angle = 0.f) const;
	void BlitAlphaAndScale(SDL_Texture * texture, int screen_x, int screen_y, const SDL_Rect * section = nullptr, Camera* current_camera = nullptr, const int alpha = 255, float scale_w = 1, float scale_h = 1) const;

	bool BlitParticle(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, const SDL_Rect* rectSize = NULL, const SDL_Color & color = { 0u, 0u, 0u, 0u }, const SDL_BlendMode & blend_mode = SDL_BLENDMODE_NONE, float speed = 1.0f, double angle = 0) const;

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

	Camera* CreateCamera(Obj_Tank * tank);
	Camera * CreateCamera(iPoint init_pos, SDL_Rect section);
	void DestroyCamera(Camera * camera);

public:

	SDL_Renderer*					renderer = nullptr;
	std::vector<Camera*>			cameras;
	std::vector<Camera*>			camera_saves;
	SDL_Rect						viewport;
	SDL_Color						background;

	bool debug = true;
};

#endif // __j1RENDER_H__