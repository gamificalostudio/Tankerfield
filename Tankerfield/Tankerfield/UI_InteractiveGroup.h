#ifndef __UI_INTERACTIVE_GROUP_H__
#define __UI_INTERACTIVE_GROUP_H__

#include "UI_Element.h"
#include "Point.h"
#include "M_UI.h"

class UI_Quad;

struct UI_InteractiveGroupDef : public UI_ElementDef
{
	// -1 : All conected controllers can navigate   
    // 0 to 3 : Only one controller can navigate 
	int  focused_controller = -1;     

	// Image that can indicate the focused element. It is optional 
	UI_Image* focus_indicator = nullptr;
	

public:
	// -1 : All conected controllers can navigate   
    // 0 to 3 : Only one controller can navigate 
	UI_InteractiveGroupDef(int focused_controller, UI_Image* focus_indicator = nullptr): focus_indicator(focus_indicator)
	{};

};

class UI_InteractiveGroup : public UI_Element , public UI_Listener
{
public:

	UI_InteractiveGroup(const fPoint position, const UI_InteractiveGroupDef definition, UI_Listener *listener);

	~UI_InteractiveGroup();

	bool Update(float dt);

	void Destroy();

public:
	// -1 : All conected controllers can navigate  
	// 0 to 3 : Only one controller can navigate 
	void SetControllers(int controller_id);

	void SetFocusImage(UI_Element * element);

	UI_Element * GetFocusedElement();

	std::list<UI_Element*> * GetGroupList();

	void AddElement( UI_Element* element);

	void Active();

	void Desactive();

private:

	UI_Element * GetNearestElement(INPUT_DIR input_dir);

	UI_Element * GetFistAvaliableElement();

	void MouseNavigation();

	void MouseSelection();

	void ControllersNavigation();

	void ControllerSelection();

private:

	bool is_active = false;

	bool to_active = false;

	bool to_desactive = false;

	// Focus info =================================

	// Current focused element in interactive group
	UI_Element* focused_element = nullptr;

	// Image that can indicate the focused element. It is optional 
	UI_Image*  focus_indicator = nullptr;

	// -1 : All conected controllers can navigate  
	// 0 to 3 : Only one controller can navigate 
	int focused_controller = -1;

	// Current state of focus. Used in UI Listeners methods 
	FocusState focus_state = FocusState::NONE;

	// Elements ====================================

	list<UI_Element*> group_elements_list;

	friend M_UI;
};


#endif //__UI_INTERACTIVE_GROUP_H__