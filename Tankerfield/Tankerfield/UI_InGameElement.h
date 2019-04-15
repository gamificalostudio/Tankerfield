#ifndef __UI_IN_GAME_ELEMENT_H_
#define  __UI_IN_GAME_ELEMENT_H_

#include "UI_Element.h"

class Object;
class Player_GUI;

class UI_Image;

struct UI_InGameElementDef: public UI_ElementDef
{
	Object*       object = nullptr;
	Player_GUI  * player_gui = nullptr;
};

class UI_InGameElement: public UI_Element
{
public:

	UI_InGameElement(const fPoint position, const UI_InGameElementDef definition, UI_Listener* listener);

	virtual bool Update(float dt);

	void UpdateArrow();

public:

	Object*        object = nullptr;
	Player_GUI   * player_gui = nullptr;		    
	UI_Image*      arrow_image = nullptr;

};

struct UI_IG_WeaponDef : public UI_InGameElementDef
{
	uint weapon_level    = 0u;
	uint weapon_type     = 0u;
};


class UI_IG_Weapon: public UI_InGameElement
{
public:

	UI_IG_Weapon(const fPoint position, const UI_IG_WeaponDef definition, UI_Listener* listener);

	bool Update(float dt);

public: 

	uint                     weapon_level = 0u;
	UI_Image*                weapon_icon = nullptr;
	UI_Image*                weapon_frame = nullptr;
	std::list<UI_Image*>     level_indicators;


};


#endif // !__UI_IN_GAME_ELEMENT_H_

