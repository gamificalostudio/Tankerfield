#ifndef __UI_ELEMENT_H__
#define __UI_ELEMENT_H__

#include "Point.h"
#include "Animation.h"
#include "Module.h"
#include <list>
#include <string>
#include "Rect.h"

using namespace std;
typedef string String;

#define DEFAULT_MARGIN_SECTION 10

struct SDL_Texture;
class UI_Listener;

class Pivot
{
public:

	Pivot()
	{};

	enum class POS_X
	{
		LEFT,
		CENTER,
		RIGHT
	} pos_x = POS_X::LEFT;

	enum class POS_Y
	{
		UP,
		CENTER,
		DOWN
	} pos_y = POS_Y::UP;

};

enum class HoverState
{
	ENTER,
	EXIT,
	REPEAT,
	NONE
};

enum class ELEMENT_STATE
{
	VISIBLE,
	HIDDEN,
	LOCKED
};

struct UI_ElementDefinition
{
	Pivot      pivot;
	fPoint     section_offset = { 0.f, 0.f };
	float      section_width = 0.f;
	float      section_height= 0.f;
	SDL_Rect   sprite_section = { 0, 0, 0, 0};

};

class UI_Element
{
public:

	UI_Element(const fPoint position, const UI_ElementDefinition definition, UI_Listener *listener);
	
	virtual ~UI_Element();

	// Virtual methods ================================
	virtual bool PreUpdate() { return true; }

	virtual bool Update(float dt) { return true; }

	virtual bool Draw();

	// Common methods =================================

	bool SetParent(UI_Element* parent);

	void SetPivot(const Pivot::POS_X x, const Pivot::POS_Y y);

	fRect GetSection();

	SDL_Rect GetDrawRect();

	list<UI_Element*>* GetSons(); 

	UI_Element* GetParent();

	bool UpdateRelativePosition();

public:

	String                name;
	fPoint                position = { 0.f, 0.f };
	SDL_Rect              sprite_section = { 0, 0, 0, 0};
	float                 section_width = 0.f;
	float                 section_height = 0.f;
	fPoint                section_offset = { 0.f, 0.f };
	ELEMENT_STATE		  state = ELEMENT_STATE::VISIBLE;
	bool			      is_draggable = false;
	bool				  is_interactive = false;

protected:

	// Vars ==============================================
	
	fPoint                relative_position = { 0.f, 0.f };
	Pivot                 pivot;
	UI_Listener         * listener = nullptr;

	// Properties ========================================

	HoverState			  hover_state = HoverState::NONE;

	// Hierarchy =========================================

	UI_Element           * parent_object = nullptr;
	list<UI_Element*>      object_sons;

	friend class M_UI;
};

#endif // __Object_H__
