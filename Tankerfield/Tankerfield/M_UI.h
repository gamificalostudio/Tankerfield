#ifndef __M_UI_H__
#define __M_UI_H__

#include <string>
#include <list>

#include "SDL_ttf/include/SDL_ttf.h"

#include "Module.h"
#include "Obj_Tank.h"
#include "Animation.h"
#include "Point.h"

#define CURSOR_WIDTH 2
#define BTW_FOCUS_TIME 150
#define UI_DEAD_ZONE 5000

using namespace std;
typedef string String;

class M_UI;
class Camera;
enum class ELEMENT_STATE;

class UI_Element;
class UI_Listener;
class UI_Image;
class UI_Bar;
class UI_Quad;
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
struct UI_QuadDef;
struct UI_LabelDef;
struct UI_ImageDef;
struct UI_SliderDef;
struct UI_CheckboxDef;
struct UI_TextPanelDef;
struct UI_InGameElementDef;

enum class FocusState
{
	ENTER,
	EXIT,
	REPEAT,
	NONE
};

enum class FOCUS_AXIS
{
	X,
	Y,
	BOTH
};

enum class UI_INPUT_TYPE
{
	NO_TYPE,
	CONTROLLER,
	MOUSE,
	KEYBOARD
};

enum class CONTROLLER_DIR
{
	NO_DIR,
	UP,
	DOWN,
	RIGHT,
	LEFT
};

enum class CONTROLLER_BUTTON : int
{
	NONE = -1,
	A,
	B,
	Y,
	X,
	L,
	LT,
	LB,
	R,
	RT,
	RB,
	MAX
};

enum class ICON_SIZE : int
{
	NONE = -1,
	SMALL,
	BIG,
	MAX
};

enum class ICON_TYPE : int
{
	NONE = -1,
	WEAPON_BASIC,
	WEAPON_DOUBLE_MISSILE,
	WEAPON_HEALING_SHOT,
	WEAPON_LASER,
	WEAPON_FLAMETHROWER,
	ITEM_HEALTH_BAG,
	ITEM_HAPPY_HOUR,
	ITEM_INSTANT_HELP,
	MAX
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

class UI_Fade_FX
{
public:
	enum class FX_TYPE
	{
		NO_TYPE,
		INTERMITTENT,
		FADE_ON,
		FADE_OUT
	};

	UI_Fade_FX(const FX_TYPE type, float seconds, UI_Element* element, float loops = -1, float init_value = -1.f, float target_value = -1.f);
	bool Update(float dt);
	void Destroy();

private:

	UI_Element*		element = nullptr;
	FX_TYPE         type = FX_TYPE::NO_TYPE;
	float			ax = 0.0f;
	float			ratetime = 0.f;
	float			target_value = 255;
	float			init_value = 0;
	bool			finished = false;
	float           loops_count = 0;
	float           max_loops = -1.f;

	friend M_UI;
};

class M_UI : public Module
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

	bool Reset();

	Player_GUI* AddPlayerGUI(Obj_Tank* player);

	void AddInteractiveElement(UI_Element * element);

	SDL_Texture* GetAtlas() const;

	FocusState GetClickState() const;

	// Creation functions ---------------------------------------------------------

	UI_Element	 * CreateElement(const fPoint position, const UI_ElementDef definition, UI_Listener* listener = nullptr);

	UI_Quad      * CreateQuad(const fPoint position, const UI_QuadDef definition, UI_Listener* listener = nullptr);

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

	UI_Element*  GetSelectedElement();

	UI_Element* GetScreen();

	UI_INPUT_TYPE GetInputType() const;

	void SetStateToBranch(const ELEMENT_STATE state, UI_Element* branch_root);


private:

	void UpdateElements(float dt);

	//void FocusMouse();

	//void FocusController();

	UI_Element* GetNearestElement(UI_Element * element, CONTROLLER_DIR dir);

	bool SelectClickedObject();

	void DrawUI(UI_Element* object); // Recursive function to draw gui as a tree

	void UpdateGuiPositions(UI_Element* object, fPoint cumulated_position);

	void AddFX( UI_Fade_FX::FX_TYPE type, const float seconds, UI_Element * element, const float loops ,const float init_value, const float target_value);

private:

	bool debug = false;

	// Atlas Texture ---------------------------------------

	SDL_Texture* atlas = nullptr;

	// Elements ---------------------------------------------

	list<UI_Element*> elements_list;

	list<UI_Element*> ig_elements_list;

	list<UI_Element*> interactive_elements;

	list<Player_GUI*> players_guis;

	list<UI_Fade_FX*> active_fxs;

	UI_Element* main_ui_element = nullptr;

	UI_Element* main_in_game_element = nullptr;

	UI_Element* selected_element = nullptr;

	FocusState click_state = FocusState::NONE;

	UI_INPUT_TYPE input_type = UI_INPUT_TYPE::MOUSE;

	Timer btw_focus_timer;

public:

	Player_GUI* current_gui = nullptr;

	Camera*     current_camera = nullptr;

	fPoint		mouse_position;

	fPoint		mouse_offset;

	FOCUS_AXIS  able_axis = FOCUS_AXIS::BOTH;

	// Assets --------------------------------------------

	SDL_Rect button_sprites[(int)CONTROLLER_BUTTON::MAX];

	SDL_Rect icon_sprites[(int)ICON_SIZE::MAX][(int)ICON_TYPE::MAX];

	Animation	green_arrow_anim;

	Animation   pink_arrow_anim;

	Animation   blue_arrow_anim;

	Animation   orange_arrow_anim;

	friend UI_Element;
};



#endif // __MODULE_UI_H__