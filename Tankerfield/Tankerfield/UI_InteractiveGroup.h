#ifndef __UI_INTERACTIVE_GROUP_H__
#define __UI_INTERACTIVE_GROUP_H__

#include "UI_Element.h"
#include "Point.h"
#include "M_UI.h"

class UI_Quad;

struct UI_InteractiveGroupDef : public UI_ElementDef
{
	uint columns = 0u;
	uint rows = 0u;
	UI_Image*  focus_indicator = nullptr;
};

class UI_InteractiveGroup : public UI_Element , public UI_Listener
{
public:

	UI_InteractiveGroup(const fPoint position, const UI_InteractiveGroupDef definition, UI_Listener *listener);

	bool Update(float dt);

	void Destroy();

	bool UI_OnHoverEnter(UI_Element* object);

	bool HandleControllerINavigation(int controller);

	bool HandleKeyboardNavigation();

	bool UI_OnHoverRepeat(UI_Element* object);

	bool UI_OnHoverExit(UI_Element * object);

public:

	void SetFocusImage(iPoint point);

	void SetElement( UI_Element* element, const iPoint position);

	UI_Element * GetElement(iPoint position);

	UI_Element * GetFocusedElement();

	iPoint GetPos(UI_Element* element);

	void SetNearestElement(const INPUT_DIR dir);

private:

	iPoint GetFirstAvailableElement();

private:

	uint columns = 0u;
	uint rows = 0u;

	iPoint current_focus_pos = { 0,0 };

	UI_Image*  focus_indicator = nullptr;

	UI_Element** group_elements = nullptr;
	list<UI_Element*> group_elements_list;

	friend M_UI;
};


#endif //__UI_INTERACTIVE_GROUP_H__