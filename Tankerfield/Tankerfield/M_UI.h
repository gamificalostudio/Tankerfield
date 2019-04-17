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
class Player_GUI;
class Camera;

class UI_Listener;
class UI_Image;
class UI_Bar;
class UI_Button;
class UI_Slider;
class UI_Label;
class UI_Checkbox;
class UI_TextPanel;
class UI_InGameElement;
class UI_IG_Weapon;
class UI_IG_Item;
class UI_IG_Helper;

struct UI_ElementDef;
struct UI_ButtonDef;
struct UI_BarDef;
struct UI_LabelDef;
struct UI_ImageDef;
struct UI_SliderDef;
struct UI_CheckboxDef;
struct UI_TextPanelDef;
struct UI_InGameElementDef;

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


class M_UI : public Module
{
public:

	enum class GAMEPAD_BUTTON: int
	{
		NONE = -1,
		A,
		B,
		Y,
		X,
		LT,
		LB,
		RT,
		RB,
		MAX
	};

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

	UI_Element	 * CreateElement(const fPoint position, const UI_ElementDef definition, UI_Listener* listener = nullptr);

	UI_Label	 * CreateLabel(const fPoint position, const  UI_LabelDef definition, UI_Listener* listener = nullptr);

	UI_Image	 * CreateImage(const fPoint position, const UI_ImageDef definition, UI_Listener* listener = nullptr);

	UI_Button	 * CreateButton(const fPoint position, const UI_ButtonDef definition, UI_Listener* listener = nullptr);

	UI_Slider	 * CreateSlider(const fPoint position, const UI_SliderDef definition, UI_Listener* listener = nullptr);

	UI_Checkbox  * CreateCheckbox(const fPoint position, const UI_CheckboxDef definition, UI_Listener* listener = nullptr);

	UI_TextPanel * CreateTextPanel(const fPoint position, const UI_TextPanelDef definition, UI_Listener* listener = nullptr);

	UI_Bar       * CreateBar(const fPoint position, const UI_BarDef definition, UI_Listener* listener = nullptr);

	UI_InGameElement * CreateInGameElement(const fPoint position, const UI_InGameElementDef definition);

	UI_IG_Weapon*   CreateInGameWeapon(const fPoint position, const UI_InGameElementDef definition);

	UI_IG_Item*   CreateInGameItem(const fPoint position, const UI_InGameElementDef definition);

	UI_IG_Helper*  CreateInGameHelper(const fPoint position, const UI_InGameElementDef definition);

	// Object functions ----------------------------------------------------------

	UI_Element*  GetClickedObject();

	UI_Element* GetScreen();

	void SetStateToBranch(const ELEMENT_STATE state, UI_Element* branch_root);

private:

	bool SelectClickedObject();

	void DrawUI(UI_Element* object); // Recursive function to draw gui as a tree

	void UpdateGuiPositions(UI_Element* object, fPoint cumulated_position);

private:

	bool debug = false;

	// Atlas Texture ---------------------------------------

	SDL_Texture* atlas = nullptr;

	// Elements ---------------------------------------------

	list<UI_Element*> elements_list;

	list<UI_Element*> ig_elements_list;

	list<Player_GUI*> players_guis;

	UI_Element* main_ui_element = nullptr;

	UI_Element* main_in_game_element = nullptr;

	UI_Element* selected_element = nullptr;

	ClickState click_state = ClickState::NONE;

public:
	// Mouse ----------------------------------------------

	Player_GUI* current_gui = nullptr;

	Player_GUI  *player_1_gui = nullptr;

	Player_GUI  *player_2_gui = nullptr;

	Player_GUI  *player_3_gui = nullptr;

	Player_GUI  *player_4_gui = nullptr;

	Camera*     current_camera = nullptr;

	fPoint		mouse_position;

	fPoint		mouse_offset;

	// Assets --------------------------------------------
	SDL_Rect button_sprite[(int)GAMEPAD_BUTTON::MAX];

	_TTF_Font*  font_open_sants_bold_12 = nullptr;

	Animation	arrow_anim;

};


#endif // __MODULE_UI_H__