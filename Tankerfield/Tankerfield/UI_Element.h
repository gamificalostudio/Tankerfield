#ifndef __UI_ELEMENT_H__
#define __UI_ELEMENT_H__

#include "Point.h"
#include "Animation.h"
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

	enum class X
	{
		LEFT,
		CENTER,
		RIGHT
	} pos_x = X::LEFT;

	enum class Y
	{
		TOP,
		CENTER,
		BOTTOM
	} pos_y = Y::TOP;

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

	void SetMenuState(ELEMENT_STATE new_state);

	ELEMENT_STATE GetState();

	void SetStateToBranch(ELEMENT_STATE new_state);

	void SetPivot(const Pivot::X x, const Pivot::Y y);

	void SetFX(UI_Fade_FX::FX_TYPE type, float seconds, float loops = -1.F, float init_value = -1.F, float target_value = -1.F);

	void SetListener(UI_Listener* new_listener);

	void FinishFX();

	fRect GetSection();

	SDL_Rect GetDrawRect();

	list<UI_Element*>* GetSons(); 

	UI_Element* GetParent();

	bool UpdateRelativePosition();

public:

	// Transform ==========================================
	fPoint                position = { 0.f, 0.f };
	float				  scale_w = 1.f;
	float				  scale_h = 1.f;


	SDL_Rect              sprite_rect = { 0, 0, 0, 0};
	float                 section_width = 0.f;
	float                 section_height = 0.f;

	bool                  is_in_game = false;

	float                 alpha = 255.f;
	SDL_Color             color_mod = { 255, 255, 255, 255 };

	// Cameras options ==================================

	Camera*               single_camera = nullptr;
	Camera*               not_in_camera = nullptr;

protected:

	// Vars =============================================
	bool                  is_focused = false;
	fPoint                section_offset = { 0.f, 0.f };
	fPoint                relative_position = { 0.f, 0.f };
	Pivot                 pivot;
	bool                  to_destroy = false;
	UI_Listener         * listener = nullptr;

	// Properties ========================================

	ELEMENT_STATE		  state = ELEMENT_STATE::VISIBLE;
	UI_Fade_FX*           element_fx = nullptr;

	// Hierarchy =========================================

	UI_Element           * element_parent = nullptr;
	list<UI_Element*>      element_sons;

	friend class M_UI;
	friend class UI_InteractiveGroup;
};

#endif // __Object_H__
