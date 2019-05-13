#ifndef __UI_INTERACTIVE_GROUP_H__
#define __UI_INTERACTIVE_GROUP_H__

#include "UI_Element.h"
#include "Point.h"

struct Controller;
class UI_Quad;

struct UI_InteractiveGroupDef : public UI_ElementDef
{
	uint columns = 0u;
	uint rows = 0u;
};


struct PlayerFocus
{
	int player_id = 0;
	iPoint    element_pos = {0,0};
	Controller **controller = nullptr;
	UI_Quad*  quad = nullptr;
};

class UI_InteractiveGroup : public UI_Element
{
public:

	UI_InteractiveGroup(const fPoint position, const UI_InteractiveGroupDef definition, UI_Listener *listener);

	~UI_InteractiveGroup();

	bool Update(float dt);

public:
	void AddPlayer( int player_id);

	void SetElement( UI_Element* element, const iPoint position);

private:

	iPoint GetNearestElement(PlayerFocus * players, CONTROLLER_DIR dir);

	bool ElementIsFocused(iPoint element_pos);

	iPoint GetFirstAvailableElement();

	UI_Element * GetElement(iPoint position);

private:

	uint columns = 0u;
	uint rows = 0u;
	UI_Element** group_elements = nullptr;
	std::vector <PlayerFocus*> players;
};


#endif //__UI_INTERACTIVE_GROUP_H__