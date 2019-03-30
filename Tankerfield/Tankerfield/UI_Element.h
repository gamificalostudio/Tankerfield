#ifndef __UI_ELEMENT_H__
#define __UI_ELEMENT_H__

#include "Point.h"
#include "Animation.h"
#include "Module.h"
#include <list>
#include <string>

using namespace std;
typedef string String;

#define DEFAULT_MARGIN_SECTION 10

struct SDL_Texture;
class UI_Listener;

enum class HoverState
{
	On,
	Out,
	Repeat,
	None
};

enum class ObjectState
{
	visible,
	hidden,
	locked
};

enum class PivotPos
{
	center,
	center_left,
	center_right,
	top_center,
	top_left,
	top_right,
	bottom_center,
	bottom_left,
	bottom_right
};

struct UI_ElementDefinition
{
	PivotPos   pivot = PivotPos::center;
	SDL_Rect   section = { 0,0,0,0 };
	fPoint     draw_offset = { 0.f, 0.f };

};

class UI_Element
{
public:

	UI_Element(const fPoint position, UI_ElementDefinition definition, UI_Listener *listener);
	
	virtual ~UI_Element();

	// Virtual methods ================================
	virtual bool PreUpdate() { return true; }

	virtual bool Update(float dt) { return true; }

	virtual bool Draw() { return true; };

	// Common methods =================================

	void SetPosition(const fPoint position);

	void SetState(const ObjectState state);

	bool SetParent(UI_Element* parent);

	void SetPivot(const PivotPos new_pivot);

	fPoint GetPosition() const;

	list<UI_Element*>* GetSons(); 

	UI_Element* GetParent();

	void IsDraggable(const bool is_draggable);

	bool UpdateRelativePosition();

protected:

	// Vars ==============================================
	String                name;
	fPoint                position = { 0.f, 0.f };
	fPoint                relative_position = { 0.f, 0.f };
	PivotPos              pivot = PivotPos::center;
	SDL_Rect			  section = {0, 0, 0, 0};
	fPoint                section_offset = { 0.f, 0.f };
	fPoint                draw_offset = { 0.f, 0.f };
	UI_Listener        * listener = nullptr;

	// Properties ========================================

	ObjectState			  state = ObjectState::visible;
	HoverState			  hover_state = HoverState::None;
	bool			      is_draggable = false;
	bool				  is_interactive = true;

	// Hierarchy =========================================

	UI_Element           * parent_object = nullptr;
	list<UI_Element*>      object_sons;

	friend class M_UI;
};

#endif // __Object_H__
