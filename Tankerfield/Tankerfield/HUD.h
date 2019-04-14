#ifndef __HUD_H_
#define __HUD_H_

#include "M_UI.h"
#include <list>
#include "Rect.h"

class Obj_Tank;
class UI_Image;
class UI_Label;
class UI_Bar;
class Object;

class Arrow
{
public:

	Arrow(Object* point,  Player_GUI * player_gui);

	void Update();

public:

	UI_Image*     image;
	Animation*    anim = nullptr;
	Object*       target = nullptr;
	fPoint        pos_test = { 3.f , 3.f };

private:

	Player_GUI  * player_gui = nullptr;
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

	void AddPointer( Object* object);

	~Player_GUI();

	void Update();

public:
	
	TYPE type = TYPE::SINGLE_PLAYER;

	Obj_Tank* target = nullptr;

	Arrow * arrow = nullptr;

	// HUD Elements =========================

	UI_Image* weapon_frame = nullptr;

	UI_Image* item_frame = nullptr;

	UI_Image* ammo_image = nullptr;

	UI_Bar* ammo_bar = nullptr;

	UI_Bar* life_bar = nullptr;

	fRect viewport;

};

#endif // __HUD_H_
