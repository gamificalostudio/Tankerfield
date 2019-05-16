#ifndef __UI_ELEMENT_H__
#define __UI_ELEMENT_H__

#include "Point.h"
#include "Animation.h"
#include "Module.h"
#include <list>
#include <string>
#include "Rect.h"
#include "M_UI.h"

using namespace std;
typedef string String;

#define DEFAULT_MARGIN_SECTION 10

struct SDL_Texture;
class UI_Listener;
class Camera;

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
		TOP,
		CENTER,
		BOTTOM
	} pos_y = POS_Y::TOP;

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

struct UI_ElementDef
{
	UI_ElementDef(){}
	UI_ElementDef(const SDL_Rect sprite_section): sprite_section(sprite_section){}

	Pivot      pivot;
	fPoint     section_offset = { 0.f, 0.f };
	float      section_width = 0.f;
	float      section_height= 0.f;
	fPoint     screen_offset = { 0.f, 0.f };
	SDL_Rect   sprite_section = { 0, 0, 0, 0};
	bool       is_in_game = false;
	Camera*    single_camera = nullptr;
	Camera*    not_in_camera = nullptr;
};

class UI_Element
{
public:

	UI_Element(const fPoint position, const UI_ElementDef definition, UI_Listener *listener);
	
	virtual ~UI_Element();

	// Virtual methods ================================
	virtual bool PreUpdate() { return true; }

	virtual bool Update(float dt) { return true; }

	virtual bool PostUpdate() { return true; }

	virtual bool Draw();

	virtual void Destroy();

	// Common methods =================================

	void  SetPos( const fPoint pos);

	void SetParent(UI_Element* parent);

	void SetState(ELEMENT_STATE new_state);

	void SetStateToBranch(ELEMENT_STATE new_state);

	//ELEMENT_STATE GetState();

	void SetPivot(const Pivot::POS_X x, const Pivot::POS_Y y);

	void SetFX(UI_Fade_FX::FX_TYPE type, float seconds, float loops = -1.F, float init_value = -1.F, float target_value = -1.F);

	void FinishFX();

	fRect GetSection();

	SDL_Rect GetDrawRect();

	list<UI_Element*>* GetSons(); 

	UI_Element* GetParent();

	bool UpdateRelativePosition();

public:
	fPoint                position = { 0.f, 0.f };
	SDL_Rect              sprite_section = { 0, 0, 0, 0};
	float                 section_width = 0.f;
	float                 section_height = 0.f;
	fPoint                section_offset = { 0.f, 0.f };
	fPoint                screen_offset = { 0.f, 0.f };
	bool			      is_draggable = false;
	bool                  is_in_game = false;
	Camera*               single_camera = nullptr;
	Camera*               not_in_camera = nullptr;
	float                 alpha = 255.f;

protected:

	// Vars =============================================

	fPoint                relative_position = { 0.f, 0.f };
	Pivot                 pivot;
	bool                  to_destroy = false;
	UI_Listener         * listener = nullptr;
	bool				  is_interactive = false;

	// Properties ========================================

	ELEMENT_STATE		  state = ELEMENT_STATE::VISIBLE;
	HoverState			  hover_state = HoverState::NONE;
	UI_Fade_FX*           element_fx = nullptr;

	// Hierarchy =========================================

	UI_Element           * parent_element = nullptr;
	list<UI_Element*>      element_sons;

	friend class M_UI;
};

#endif // __Object_H__
