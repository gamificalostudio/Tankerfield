#ifndef __HUD_H_
#define __HUD_H_

#include "M_UI.h"

class Obj_Tank;
class UI_Image;
class UI_Label;
class UI_Bar;


struct Pickable_Weapon
{
	UI_Image* weapon_frame;
	UI_Image* weapon_icon;
	int       weapon_level;
};

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
	~Player_GUI();

public:
	
	TYPE type = TYPE::SINGLE_PLAYER;

	Obj_Tank* target = nullptr;

	UI_Image* weapon_frame = nullptr;
	UI_Image* item_frame = nullptr;
	UI_Image* ammo_image = nullptr;
	UI_Bar* ammo_bar = nullptr;
	UI_Bar* life_bar = nullptr;


};

#endif // __HUD_H_
