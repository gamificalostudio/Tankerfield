#ifndef __UI_Object_H__
#define __UI_Object_H__

#include "p2Point.h"
#include "Animation.h"
#include "j1Module.h"
#include <list>
using namespace std;

#define DEFAULT_MARGIN_SECTION 10

struct SDL_Texture;
class Gui_Listener;

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

class UI_Object
{
public:

	UI_Object(const iPoint position , Gui_Listener *listener);
	
	virtual ~UI_Object();

	// Virtual methods ================================
	virtual bool PreUpdate() { return true; }

	virtual bool Update(float dt) { return true; }

	virtual bool Draw() { return true; };

	// Common methods =================================

	void SetPosition(iPoint position);

	void SetState(const ObjectState state);

	bool SetParent(UI_Object* parent);

	iPoint GetPosition() const;

	list<UI_Object*>* GetSons(); 

	UI_Object* GetParent();

	void IsDraggable(const bool is_draggable);

	bool UpdateRelativePosition();

protected:

	// Vars ==============================================

	iPoint                position = {0, 0};
	iPoint                relative_position = { 0, 0 };
	SDL_Rect			  section = {0, 0, 0, 0};
	Gui_Listener        * listener = nullptr;

	// Properties ========================================

	ObjectState state = ObjectState::visible;
	HoverState hover_state = HoverState::None;
	bool is_draggable = false;
	bool is_interactive = true;

	// Hierarchy =========================================

	UI_Object           * parent_object = nullptr;
	list<UI_Object*>      object_sons;

	friend class Module_UI;
};

#endif // __Object_H__
