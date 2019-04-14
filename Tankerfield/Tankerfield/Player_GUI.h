#ifndef __PLAYER_GUI_H_
#define __PLAYER_GUI_H_

#include "M_UI.h"
#include <list>
#include "Rect.h"

class Obj_Tank;
class UI_Image;
class UI_Label;
class UI_Bar;
class Object;

class UI_InGameElement;

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

	Player_GUI( Player_GUI::TYPE type, Obj_Tank* target);

	UI_InGameElement* AddInGameElement(Object* object, Player_GUI* gui);

	~Player_GUI();

public:
	
	TYPE type = TYPE::SINGLE_PLAYER;

	Obj_Tank* target = nullptr;

	// HUD Elements =========================

	UI_Image* weapon_frame = nullptr;

	UI_Image* item_frame = nullptr;

	UI_Image* ammo_image = nullptr;

	UI_Bar* ammo_bar = nullptr;

	UI_Bar* life_bar = nullptr;

	fRect viewport;

	std::list<UI_InGameElement*> in_game_elements;
};

#endif // __PLAYER_GUI_H_
