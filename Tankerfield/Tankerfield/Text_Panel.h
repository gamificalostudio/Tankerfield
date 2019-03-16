#ifndef __TEXT_PANEL_H__
#define __TEXT_PANEL_H__

#include "j1Module.h"
#include "UI_Object.h"
#include "p2Point.h"
#include "SDL_ttf/include/SDL_ttf.h"
#include <vector>
#include <string>

using namespace std;
typedef string String;

struct TextPanel_Definition
{
	String  		      text;
	_TTF_Font*			  font = nullptr;
	int                   line_spacing = 10;
	int                   width = 0;
	SDL_Color             color = { 255,255,255,255 };
};

class TextPanel : public UI_Object
{
public:

	TextPanel(const iPoint position, TextPanel_Definition definition, Gui_Listener* listener);

	// Destructor
	~TextPanel();

	// Set the text
	void SetText(String text);

	// Draw label
	bool Draw();

private:

	SDL_Texture*           text_texture = nullptr;
	TextPanel_Definition   definition;
	vector<SDL_Rect>       rects;
	uint                   lines = 0;
	int                    font_height = 0;
	int                    font_width = 0;
};


#endif // __TEXT_PANEL_H__
