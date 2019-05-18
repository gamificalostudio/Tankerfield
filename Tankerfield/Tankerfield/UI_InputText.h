#include "UI_Element.h"
#include <string>

class UI_Label;
class UI_Quad;

struct UI_InputTextDef : public UI_ElementDef
{
	int max_characters = 0;
	std::string  default_text = "Enter your text";
	TTF_Font* font = nullptr;
	SDL_Color default_text_color = { 255, 255, 255,255 };
	SDL_Color editable_text_color = { 255, 255, 255,255 };;
};

class UI_InputText : public UI_Element
{
public:

	UI_InputText(const fPoint position, const UI_InputTextDef definition, UI_Listener* listener);

	bool Update(float dt);

	std::string GetText();

	void AddChar(char text);

	void AddText(std::string text);

	void DeleteLastChar();

private:

	void SetCursorPos();

private:

	std::string input_text;
	TTF_Font* font = nullptr;

	UI_Quad* cursor = nullptr;
	UI_Label* default_label = nullptr;
	UI_Label* editable_label = nullptr;
	int max_characters = 0;
	int default_text = 0;
	SDL_Color default_text_color = { 255, 255, 255,255 };
	SDL_Color editable_text_color = { 255, 255, 255,255 };;
};

