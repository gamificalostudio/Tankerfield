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
	image_def.image_animation = app->ui->arrow_anim;
	arrow_image = app->ui->CreateImage({ 0.f, 0.f}, image_def);
	arrow_image->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);

	// Set Image =================================================
}

bool UI_InGameElement::Update(float dt)
{
	UpdateArrow();

	return true;
}

void UI_InGameElement::UpdateArrow()
{
	fPoint vector = player_gui->target->pos_map - object->pos_map;
	arrow_image->sprite_section = arrow_image->image_animation->GetFrame(atan2(vector.y, vector.x) * RADTODEG);
	vector.Normalize();
	arrow_image->position = this->position - app->map->MapToScreenF(vector * 1.5f);
}

UI_IG_Weapon::UI_IG_Weapon(const fPoint position, const UI_IG_WeaponDef definition, UI_Listener * listener) : UI_InGameElement(position, definition, listener), weapon_level(definition.weapon_level)
{
	//definition.player_gui->target
	UI_ImageDef img_def;
	img_def.sprite_section = { 120, 195, 50, 65 };
	
	fPoint screen_pos = app->map->MapToScreenF( object->pos_map - player_gui->target->pos_map) + fPoint( player_gui->viewport.GetLeft() + player_gui->viewport.w * 0.5f, player_gui->viewport.GetTop() + player_gui->viewport.h * 0.5f);
	weapon_frame = app->ui->CreateImage(screen_pos, img_def);
	weapon_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);

}

bool UI_IG_Weapon::Update(float dt)
{
	if (SDL_HasIntersection(&weapon_frame->GetDrawRect(), &(SDL_Rect)player_gui->viewport))
	{
		fPoint screen_pos = app->map->MapToScreenF(object->pos_map - player_gui->target->pos_map) + fPoint(player_gui->viewport.GetLeft() + player_gui->viewport.w * 0.5f, player_gui->viewport.GetTop() + player_gui->viewport.h * 0.5f);
		weapon_frame->SetPos(screen_pos);
	}

	UpdateArrow();
	return true;
}

