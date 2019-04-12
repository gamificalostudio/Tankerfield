#ifndef __M_WINDOW_H__
#define __M_WINDOW_H__

#include "Module.h"
#include "Rect.h"

struct SDL_Window;
struct SDL_Surface;
class M_Window : public Module
{
public:

	M_Window();

	// Destructor
	virtual ~M_Window();

	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	// Called before quitting
	bool CleanUp() override;

	// Changae title
	void SetTitle(const char* new_title);

	// Retrive window size
	void GetWindowSize(uint& width, uint& height) const;

	fRect GetWindowRect();
	// Retrieve window scale
	uint GetScale() const;

public:
	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

private:
	uint		width = 0;
	uint		height = 0;
	uint		scale = 0;
};

#endif // __j1WINDOW_H__