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

	bool Update(float dt);

	virtual bool Draw() { return true; };

public:

	Object*       object = nullptr;
	Player_GUI  * player_gui = nullptr;

	// Arrow ===================================

	UI_Image*     arrow_image = nullptr;
};

#endif // !__UI_IN_GAME_ELEMENT_H_

