#ifndef __UI_INTERACTIVE_GROUP_H__
#define __UI_INTERACTIVE_GROUP_H__

#include "UI_Element.h"

class UI_InteractiveGroup : public UI_Element
{
public:

	UI_InteractiveGroup(const fPoint position, const UI_ElementDef definition, UI_Listener *listener);

	~UI_InteractiveGroup();

	bool PreUpdate() { return true; }

	bool Update(float dt) { return true; }

	bool PostUpdate() { return true; }

public:

	void AddElement(UI_Element* element);

private:

	UI_Element * GetNearestElement(UI_Element * element, CONTROLLER_DIR dir);

private:

	std::list< UI_Element*> group_elements;

};


#endif //__UI_INTERACTIVE_GROUP_H__