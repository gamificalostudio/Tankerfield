#ifndef __UI_INTERACTIVE_GROUP_H__
#define __UI_INTERACTIVE_GROUP_H__

#include "UI_Element.h"
#include "Point.h"
#include "M_UI.h"

class UI_Quad;

struct UI_InteractiveGroupDef : public UI_ElementDef
{
	UI_Image*  focus_indicator = nullptr;
public:
	UI_InteractiveGroupDef(uint columns, uint rows, UI_Image* focus_indicator = nullptr) :columns(columns), rows(rows), focus_indicator(focus_indicator)
	{};
	UI_InteractiveGroupDef()
	{};
};

class UI_InteractiveGroup : public UI_Element , public UI_Listener
{
public:

	UI_InteractiveGroup(const fPoint position, const UI_InteractiveGroupDef definition, UI_Listener *listener);

	bool Update(float dt);

	void Destroy();

	bool UI_OnHoverEnter(UI_Element* object);

	bool UI_OnHoverRepeat(UI_Element* object);

	bool UI_OnHoverExit(UI_Element * object);

	bool UI_Selected(UI_Element * object);

public:

	void SetFocusImage(UI_Element * element);

	void SetElement( UI_Element* element);

	std::list<UI_Element*>* GetElementsList();

private:

	UI_Image*  focus_indicator = nullptr;
	list<UI_Element*> group_elements_list;

	friend M_UI;
};


#endif //__UI_INTERACTIVE_GROUP_H__