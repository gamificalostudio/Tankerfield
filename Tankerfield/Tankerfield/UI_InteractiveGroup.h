#ifndef __UI_INTERACTIVE_GROUP_H__
#define __UI_INTERACTIVE_GROUP_H__

#include "UI_Element.h"
#include "Point.h"

struct Controller;

struct UI_InteractiveGroupDef : public UI_ElementDef
{
	uint columns = 0u;
	uint rows = 0u;
};


struct PlayerFocus
{
	iPoint    focused_element = {0,0};
	Obj_Tank* player = nullptr;
};

class UI_InteractiveGroup : public UI_Element
{
public:

	UI_InteractiveGroup(const fPoint position, const UI_InteractiveGroupDef definition, UI_Listener *listener);

	~UI_InteractiveGroup();

	bool Update(float dt);

public:

	void SetElement( UI_Element* element, const iPoint position);

private:

	UI_Element * GetNearestElement(PlayerFocus * player_focus, CONTROLLER_DIR dir);

	bool ElementIsFocused( UI_Element* element);

	UI_Element * GetElement(iPoint position);

private:

	uint columns = 0u;
	uint rows = 0u;
	std::vector <UI_Element*>  group_elements;
	std::vector <PlayerFocus*> player_focus;

};


#endif //__UI_INTERACTIVE_GROUP_H__