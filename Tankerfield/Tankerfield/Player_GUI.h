#ifndef __PLAYER_GUI_H_
#define __PLAYER_GUI_H_

#include "M_UI.h"
#include <list>
#include <vector>
#include "Rect.h"

class Obj_Tank;
class UI_Image;
class UI_Label;
class UI_Bar;
class Object;
class Helper;
class Text_Helper;
class Button_Helper;
class UI_PickUpElement;

enum class WEAPON;
enum class ObjectType;

class Player_GUI: public UI_Listener
{
public:

	enum class TYPE
	{
		SINGLE_PLAYER,
		PLAYER_1,
		PLAYER_2,
		PLAYER_3,
		PLAYER_4
	};

	Player_GUI(const Player_GUI::TYPE type, Obj_Tank* target);

	void Update(float dt);

	void ClearHelpers();

	void SetHelper();

	void AddButtonHelper(const Button_Helper helper);

	void AddTextHelper(const Text_Helper helper);

	void SetLifeBar( float life);

	void SetChargedShotBar(float percent);

	void SetWeaponIcon(WEAPON weapon_type);

	void SetItemIcon(ObjectType type);

	~Player_GUI();

public:
	
	TYPE type = TYPE::SINGLE_PLAYER;

	Obj_Tank* player = nullptr;

	// HUD Elements =========================

	UI_Image* weapon_frame = nullptr;

	UI_Image* weapon_icon = nullptr;

	UI_Image* item_frame = nullptr;

	UI_Image* item_icon = nullptr;

	UI_Bar* charged_shot_bar = nullptr;

	UI_Bar* life_bar = nullptr;

	fRect viewport;

	fPoint margin = { 80.f, 80.f };

	SDL_Rect viewport_with_margin = { 0,0,0,0 };

private:

	std::vector<Helper*> helpers_vector;

	std::vector<UI_Element*> helper_elements;

	friend M_UI;
};

class Helper
{
public:

	enum class HELPER_TYPE
	{
		BUTTON,
		TEXT,
		NONE
	};

	Helper(fPoint offset = { 0.f,0.f }) : offset(offset) {}

	fPoint offset = {0,0};

	HELPER_TYPE type = HELPER_TYPE::NONE;

};

class Button_Helper: public Helper
{
public:

	Button_Helper(const M_UI::GAMEPAD_BUTTON button_type ,const fPoint offset = { 0.f,0.f }): Helper(offset) , button_type(button_type)
	{
		type = HELPER_TYPE::BUTTON;
	}

	M_UI::GAMEPAD_BUTTON button_type = M_UI::GAMEPAD_BUTTON::NONE;
};

class Text_Helper : public Helper
{
public:

	Text_Helper( const String text, const fPoint offset = { 0.f,0.f }) : Helper(offset)
	{
		this->text.assign(text);
		type = HELPER_TYPE::TEXT;
	}

	String text;
};

#endif // __PLAYER_GUI_H_
