#ifndef __M_UI_H__
#define __M_UI_H__

#include <string>
#include <list>

#include "SDL_ttf/include/SDL_ttf.h"

#include "Module.h"
#include "Animation.h"
#include "Point.h"


#define CURSOR_WIDTH 2

using namespace std;
typedef string String;

enum class ELEMENT_STATE;
class UI_Element;

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
	ENTER,
	EXIT,
	REPEAT,
	NONE
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


class M_UI : public Module , public UI_Listener
{
public:

	M_UI();

	virtual ~M_UI();

	bool Awake(pugi::xml_node& node) override;

	bool Start() override;

	bool PreUpdate() override;

	bool Update(float dt) override;

	bool PostUpdate(float dt) override;

	bool CleanUp() override;

	SDL_Texture* GetAtlas() const;

	ClickState GetClickState() const;

	// Creation functions ---------------------------------------------------------

	UI_Element* CreateObject(const fPoint position, UI_ElementDefinition definition, UI_Listener* listener = nullptr);

	UI_Label* CreateLabel(const fPoint position, const String text, UI_LabelDef definition, UI_Listener* listener = nullptr);

	UI_Image* CreateImage(const fPoint position, UI_ImageDef definition, UI_Listener* listener = nullptr);

	UI_Button* CreateButton(const fPoint position, UI_ButtonDef definition, UI_Listener* listener = nullptr);

	UI_Slider* CreateSlider(const fPoint position, UI_SliderDef definition, UI_Listener* listener = nullptr);

	UI_Checkbox* CreateCheckbox(const fPoint position, UI_CheckboxDef definition, UI_Listener* listener = nullptr);

	UI_TextPanel * CreateTextPanel(const fPoint position, UI_TextPanelDef definition, UI_Listener* listener = nullptr);

	// Object functions ----------------------------------------------------------

	UI_Element*  GetClickedObject();

	UI_Element* GetScreen();

	bool DeleteObject(UI_Element* object);

	void SetStateToBranch(const ELEMENT_STATE state, UI_Element* branch_root);

private:

	bool SelectClickedObject();

	void DrawUI(UI_Element* object); // Recursive function to draw gui as a tree

	void UpdateGuiPositions(UI_Element* object, fPoint cumulated_position);

private:

	bool debug = false;

	// Atlas Texture ---------------------------------------

	SDL_Texture* atlas = nullptr;

	// Objects ---------------------------------------------

	list<UI_Element*> objects_list;

	UI_Element* main_object = nullptr;

	UI_Element* selected_object = nullptr;

	ClickState click_state = ClickState::NONE;

public:
	// Mouse ----------------------------------------------

	fPoint				mouse_position;

	fPoint				mouse_offset;

};

#endif // __MODULE_UI_H__