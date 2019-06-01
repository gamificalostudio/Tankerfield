#ifndef __PLAYER_GUI_H_
#define __PLAYER_GUI_H_

#include <list>
#include <string>
#include <vector>
#include "Rect.h"

class Obj_Tank;
class UI_Image;
class UI_Label;
class UI_Bar;
class Object;
class UI_PickUpElement;
class UI_Element;
class UI_InGameElement;
class M_UI;

enum class CONTROLLER_BUTTON;
enum class WEAPON;
enum class ObjectType;
enum class ItemType;

class Player_GUI
{
public:

	Player_GUI(Obj_Tank* target);

	void Update(float dt);

	void ClearHelpers();

	void SetHelper();

	void SetLifeBar( float life);

	void SetChargedShotBar(float percent);

	void SetWeaponIcon(WEAPON weapon_type);

	void SetItemIcon(ItemType type);

	void SetArrowColor(const SDL_Color color);

	void Fade_GUI(bool fade_on);

	void DamageFlash();

	void HealingFlash();

public:
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

	UI_Image* item_helper = nullptr;

	UI_Image*  weapon_helper = nullptr;

	std::vector<UI_Element*> helper_elements;

	// In Game Elements ======================

	UI_InGameElement* player_arrow = nullptr;

	friend M_UI;
};

#endif // __PLAYER_GUI_H_
