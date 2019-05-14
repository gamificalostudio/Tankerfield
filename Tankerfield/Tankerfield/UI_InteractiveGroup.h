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

class UI_InteractiveGroup : public UI_Element
{
public:

	UI_InteractiveGroup(const fPoint position, const UI_InteractiveGroupDef definition, UI_Listener *listener);

	~UI_InteractiveGroup();

	bool Update(float dt);

public:

	void SetElement( UI_Element* element, const iPoint position);

	void SetController(Controller ** controller);

	UI_Element * GetElement(iPoint position);

private:

	iPoint GetNearestElement(const iPoint current_focus, const CONTROLLER_DIR dir);

	iPoint GetFirstAvailableElement();

private:

	uint columns = 0u;
	uint rows = 0u;

	iPoint current_focus = { 0,0 };
	UI_Image*  focus_image = nullptr;
	Controller ** current_controller = nullptr;
	UI_Element** group_elements = nullptr;
};


#endif //__UI_INTERACTIVE_GROUP_H__