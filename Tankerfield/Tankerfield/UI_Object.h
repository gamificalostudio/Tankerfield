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
	iPoint GetPosition() const;

	void SetPosition(iPoint position);

	bool UpdateRelativePosition();

	bool SetAnchor(UI_Object* anchor);

	list<UI_Object*>* GetAnchorSons(); 

	UI_Object* GetAnchorParent();

	void IsDraggable(const bool is_draggable);

	void SetState(const ObjectState state);

protected:

	iPoint                position = {0, 0};
	iPoint                relative_position = { 0, 0 };
	SDL_Rect			  section = {0, 0, 0, 0};
	SDL_Texture         * texture = nullptr;
	Gui_Listener        * listener = nullptr;

	// Anchors =========================================
	UI_Object           * anchor_parent = nullptr;
	list<UI_Object*>      anchor_sons;

	// Properties ======================================
	ObjectState state = ObjectState::visible;
	HoverState hover_state = HoverState::None;
	bool is_draggable = false;
	bool is_interactive = true;

	friend class Module_UI;
};

#endif // __Object_H__
