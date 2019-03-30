#ifndef __MODULE_UI_H__
#define __MODULE_UI_H__

#include "Module.h"
#include "SDL_ttf/include/SDL_ttf.h"
#include "Animation.h"
#include "p2Point.h"
#include <string>
#include <list>

#define CURSOR_WIDTH 2

using namespace std;
typedef string String;

class UI_Element;
enum class ObjectState;

class UI_Listener;
class UI_Image;
class UI_Button;
class UI_Slider;
class UI_Label;
class UI_Checkbox;
class UI_TextPanel;

struct UI_ElementDefinition;
struct UI_ButtonDef;
struct UI_LabelDef;
struct UI_ImageDef;
struct UI_SliderDef;
struct UI_CheckboxDef;
struct UI_TextPanelDef;

enum class ClickState
{
	On,
	Out,
	Repeat,
	None
};

class M_UI : public Module
{
public:

	M_UI();

	virtual ~M_UI();

	bool Awake(pugi::xml_node& node);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	SDL_Texture* GetAtlas() const;

	ClickState GetClickState() const;

	// Creation functions ---------------------------------------------------------
	UI_Element* CreateObject(const fPoint position, UI_ElementDefinition definition, UI_Listener* listener = nullptr);

	UI_Label* CreateLabel(const fPoint position, const String text, UI_LabelDef definition, UI_Listener* listener = nullptr);

	UI_Image* CreateImage(const fPoint position, UI_ImageDef definition, UI_Listener* listener = nullptr);

	UI_Button* CreateButton(const fPoint position, UI_ButtonDef definition, UI_Listener* listener = nullptr);

	UI_Slider* CreateSlider(const fPoint position, UI_SliderDef definition, UI_Listener* listener = nullptr);

	UI_Checkbox* CreateCheckbox(const fPoint position, UI_CheckboxDef definition, UI_Listener* listener = nullptr);

	UI_TextPanel * CreateTextPanel(const fPoint position, UI_TextPanelDef definition, UI_Listener* listener);

	// Object functions ----------------------------------------------------------
	UI_Element*  GetClickedObject();

	UI_Element* GetScreen();

	bool DeleteObject(UI_Element* object);

	void SetStateToBranch(const ObjectState state, UI_Element* branch_root);

	// Slider functions ----------------------------------------------------------
	fPoint GetMouseOffset() const;

	void SetCursorOffset(const fPoint offset);

private:

	bool SelectClickedObject();

	void DrawUI(UI_Element* object); // Recursive function to draw gui as a tree

	void UpdateGuiPositions(UI_Element* object, fPoint cumulated_position);

private:

	bool debug = false;

	// Atlas Texture ---------------------------------------
	SDL_Texture* atlas;

	// Objects ---------------------------------------------
	list<UI_Element*> objects_list;

	UI_Element* main_object = nullptr;

	UI_Element* selected_object = nullptr;

	ClickState click_state = ClickState::None;

public:
	// Mouse ----------------------------------------------

	fPoint				mouse_position;

	fPoint				mouse_offset;

};


class UI_Listener
{
public:
	UI_Listener()
	{}

	virtual bool OnHover(UI_Element* object) { return true; }

	virtual bool RepeatHover(UI_Element* object) { return true; }

	virtual bool OutHover(UI_Element* object) { return true; }

	virtual bool OnClick(UI_Element* object) { return true; }

	virtual bool RepeatClick(UI_Element* object) { return true; }

	virtual bool OutClick(UI_Element* object) { return true; }
};



#endif // __MODULE_UI_H__