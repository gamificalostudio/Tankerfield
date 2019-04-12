#ifndef __HUD_H_
#define __HUD_H_

#include "M_UI.h"

class Obj_Tank;
class UI_Image;
class UI_Label;
class UI_Bar;

class HUD: public UI_Listener
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

	HUD( HUD::TYPE type, Obj_Tank* target);
	~HUD();

public:
	
	TYPE type = TYPE::SINGLE_PLAYER;

	Obj_Tank* target = nullptr;

	UI_Image* basic_weapon_frame = nullptr;
	UI_Image* special_weapon_frame = nullptr;
	UI_Image* item_frame = nullptr;
	UI_Image* ammo_image = nullptr;
	UI_Bar* ammo_bar = nullptr;
	UI_Bar* life_bar = nullptr;
};

#endif // __HUD_H_
