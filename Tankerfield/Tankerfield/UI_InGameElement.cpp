#include "UI_InGameElement.h"
#include "App.h"
#include "M_UI.h"
#include "M_Map.h"
#include "Player_GUI.h"

#include "Obj_Tank.h"
#include "Object.h"

#include "UI_Image.h"


UI_InGameElement::UI_InGameElement(const fPoint position, const UI_InGameElementDef definition, UI_Listener* listener) : UI_Element(position, definition, listener), object(definition.object), player_gui(definition.player_gui)
{

	this->position = { player_gui->viewport.GetLeft() + player_gui->viewport.w * 0.5f, player_gui->viewport.GetTop() + player_gui->viewport.h * 0.5f };

	// Set Arrow =================================================

	UI_ImageDef image_def;
	image_def.image_animation = &app->ui->arrow_anim;
	arrow_image = app->ui->CreateImage({ 0.f, 0.f}, image_def);
	arrow_image->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);

	// Set Image =================================================
}

bool UI_InGameElement::Update(float dt)
{
	fPoint vector = player_gui->target->pos_map - object->pos_map;
	arrow_image->sprite_section = arrow_image->image_animation->GetFrame(atan2(vector.y, vector.x) * RADTODEG);
	vector.Normalize();
	arrow_image->position = this->position - app->map->MapToScreenF(vector * 1.5f);

	return true;
}