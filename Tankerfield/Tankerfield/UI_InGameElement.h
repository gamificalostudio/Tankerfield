#ifndef __UI_IN_GAME_ELEMENT_H_
#define  __UI_IN_GAME_ELEMENT_H_

#include "UI_Element.h"
#include "M_UI.h"

class Object;
class Obj_PickUp;
class Player_GUI;
class UI_Elment;
class UI_Image;
class Camera;

enum class ARROW_COLOR
{
	NONE,
	ORANGE,
	GREEN,
	BLUE,
	PINK
};

struct UI_InGameElementDef : public UI_ElementDef
{
	Object*         pointed_obj = nullptr;
	bool            is_arrow_actived = false;
	SDL_Color       arrow_color = {255, 255, 255, 255};
};

class UI_InGameElement: public UI_Element
{
public:

	UI_InGameElement(const fPoint map_pos, const UI_InGameElementDef definition);

	bool Update(float dt);

	virtual bool Draw();
	
public:

	Object*        pointed_obj = nullptr;   // Object used to set position of main element

protected:

	bool           is_arrow_actived = false;
};


class UI_IG_Weapon: public UI_InGameElement
{
public:

	UI_IG_Weapon(const fPoint map_pos, const UI_InGameElementDef definition);

	bool Draw();

	void Destroy();

	void UpdateLevel();

public: 

	int pick_up_weapon_level = 4;
	int player_weapon_level = 1;

	UI_Image*             weapon_icon = nullptr;
	UI_Image*             weapon_frame = nullptr;
	UI_Image*             level_indicator = nullptr;
};

class UI_IG_Item : public UI_InGameElement
{
public:

	UI_IG_Item(const fPoint map_pos, const UI_InGameElementDef definition);

	void Destroy();

public:

	UI_Image*  item_icon = nullptr;
	UI_Image*  item_frame = nullptr;
};


class UI_IG_Helper : public UI_InGameElement
{
public:

	UI_IG_Helper(const fPoint map_pos, const UI_InGameElementDef definition);

	void AddButtonHelper(const CONTROLLER_BUTTON button_type, const fPoint offset);

	void AddTextHelper(const String text, const  fPoint offset);

	void Destroy();

public:

	std::vector<UI_Element*> helper_elements;

};


#endif // !__UI_IN_GAME_ELEMENT_H_

