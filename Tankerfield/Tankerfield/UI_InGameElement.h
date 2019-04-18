#ifndef __UI_IN_GAME_ELEMENT_H_
#define  __UI_IN_GAME_ELEMENT_H_

#include "UI_Element.h"

class Object;
class Obj_PickUp;
class Player_GUI;
class UI_Elment;
class UI_Image;
class Button_Helper;
class Text_Helper;

struct UI_InGameElementDef : public UI_ElementDef
{
	fPoint        map_pos = { 0.f, 0.f };
	Object*       pointed_obj = nullptr;
	bool          add_arrow = true;
};

class UI_InGameElement: public UI_Element
{
public:

	UI_InGameElement(const fPoint position, const UI_InGameElementDef definition);

	virtual bool PostUpdate();

	virtual void Destroy();

	void UpdateArrow();
	
public:

	fPoint         map_pos = { 0.f, 0.f };  // If object is null, use only a point to set its position
	fPoint         object_offset = { 0.f, 0.f };
	Object*        pointed_obj = nullptr;   // Object used to set position of main element

protected:
	
	UI_Element*    main_element = nullptr;  // Element used to check if element is inside viewport margins in order to hide arrow
	UI_Image*      arrow_image = nullptr;   // Arrow element 
};


class UI_IG_Weapon: public UI_InGameElement
{
public:

	UI_IG_Weapon(const fPoint position, const UI_InGameElementDef definition);

	bool PostUpdate();

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

	UI_IG_Item(const fPoint position, const UI_InGameElementDef definition);

	bool PostUpdate();

	void Destroy();

public:

	UI_Image*  item_icon = nullptr;
	UI_Image*  item_frame = nullptr;
};


class UI_IG_Helper : public UI_InGameElement
{
public:

	UI_IG_Helper(const fPoint position, const UI_InGameElementDef definition);

	bool PostUpdate();

	void AddButtonHelper( Button_Helper helper);

	void AddTextHelper(Text_Helper helper);

	void Destroy();

public:

	std::vector<UI_Element*> helper_elements;
};


#endif // !__UI_IN_GAME_ELEMENT_H_

