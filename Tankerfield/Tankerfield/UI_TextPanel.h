#ifndef __UI_TEXT_PANEL_H__
#define __UI_TEXT_PANEL_H__

#include "Module.h"
#include "UI_Element.h"
#include "Point.h"
#include "SDL_ttf/include/SDL_ttf.h"
#include <vector>
#include <string>

using namespace std;
typedef string String;

struct UI_TextPanelDef: public UI_ElementDefinition
{
	String  		      text;
	_TTF_Font*			  font = nullptr;
	int                   line_spacing = 10;
	int                   width = 0;
	SDL_Color             color = { 255,255,255,255 };
};

class UI_TextPanel : public UI_Element
{
public:

	UI_TextPanel(const fPoint position, UI_TextPanelDef definition, UI_Listener* listener);

	// Destructor
	~UI_TextPanel();

	// Set the text
	void SetText(String text);

	// Draw label
	bool Draw();

private:

	SDL_Texture*           text_texture = nullptr;
	UI_TextPanelDef   definition;
	vector<iRect>       rects;
	uint                   lines = 0;
	int                    font_height = 0;
	int                    font_width = 0;
};


#endif // __TEXT_PANEL_H__
