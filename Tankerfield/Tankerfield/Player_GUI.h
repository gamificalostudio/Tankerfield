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

	void AddButtonHelper(const M_UI::GAMEPAD_BUTTON button_type);

	void AddTextHelper(const String text);

	void SetLifeBar( float life);

	void SetChargedShotBar(float percent);

	void SetWeaponIcon(WEAPON weapon_type);

	void SetItemIcon(ObjectType type);

	void Fade_GUI(bool fade_on);

	~Player_GUI();

public:
	
	TYPE type = TYPE::SINGLE_PLAYER;

	Obj_Tank* player = nullptr;

	fRect viewport;

	fPoint margin = { 80.f, 80.f };

	SDL_Rect viewport_with_margin = { 0,0,0,0 };

private:

	// HUD Elements =========================

	UI_Image* weapon_frame = nullptr;

	UI_Image* weapon_icon = nullptr;

	UI_Image* item_frame = nullptr;

	UI_Image* item_icon = nullptr;

	UI_Bar*   charged_shot_bar = nullptr;

	UI_Bar*   life_bar = nullptr;

	std::vector<UI_Element*> helper_elements;

	// In Game Elements ======================

	UI_InGameElement* player_arrow = nullptr;

	friend M_UI;
};

#endif // __PLAYER_GUI_H_
