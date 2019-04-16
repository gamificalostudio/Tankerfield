#include "UI_InGameElement.h"
#include "App.h"
#include "M_UI.h"
#include "M_Map.h"
#include "Player_GUI.h"

#include "Obj_Tank.h"
#include "Obj_Item.h"
#include "Object.h"

#include "UI_Image.h"
//#include <algorithm>

UI_InGameElement::UI_InGameElement(const fPoint position, const UI_InGameElementDef definition, UI_Listener* listener) : UI_Element(position, definition, listener), object(definition.object), player_gui(definition.player_gui)
{
	this->position = { player_gui->viewport.GetLeft() + player_gui->viewport.w * 0.5f, player_gui->viewport.GetTop() + player_gui->viewport.h * 0.5f };

	// Set Arrow =================================================

	UI_ImageDef image_def;
	image_def.image_animation = app->ui->arrow_anim;
	arrow_image = app->ui->CreateImage({ 0.f, 0.f}, image_def);
	arrow_image->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	arrow_image->camera = player_gui->player->camera_player;

	// Set Image =================================================
}

bool UI_InGameElement::Update(float dt)
{
	UpdateArrow();
	return true;
}

void UI_InGameElement::UpdateArrow()
{
	fPoint vector = player_gui->player->pos_map - object->pos_map;
	arrow_image->sprite_section = arrow_image->image_animation->GetFrame(atan2(vector.y, vector.x) * RADTODEG);
	vector.Normalize();
	arrow_image->SetPos(app->map->MapToCamera(player_gui->player->pos_map - vector * 1.5f, player_gui->player->camera_player));
}


UI_IG_Weapon::UI_IG_Weapon(const fPoint position, const UI_IG_WeaponDef definition, UI_Listener * listener) : UI_InGameElement(position, definition, listener), last_player_level(definition.last_player_level)
{
	last_player_level = 3;
	current_player_level = 4;
	weapon_level = 6;
	type = WEAPON::DOUBLE_MISSILE;

	UI_ImageDef img_def;
	img_def.sprite_section = { 330, 160, 50, 65 };
	
	fPoint camera_pos = app->map->MapToCamera(object->pos_map, player_gui->player->camera_player);
	weapon_frame = app->ui->CreateImage(camera_pos, img_def);
	weapon_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
	weapon_frame->camera = player_gui->player->camera_player;

	switch (type)
	{
	case WEAPON::DOUBLE_MISSILE:
		img_def.sprite_section = { 620, 10, 34, 34 };
		break;
	case WEAPON::FLAMETHROWER:
		img_def.sprite_section = { 580, 10, 34, 34 };
		break;
	case WEAPON::BASIC:
		img_def.sprite_section = { 330, 10, 34, 34 };
		break;
	}

	weapon_icon = app->ui->CreateImage(camera_pos - fPoint(0.f, 24.f), img_def);
	weapon_icon->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
	weapon_icon->camera = player_gui->player->camera_player;
	weapon_icon->SetParent(weapon_frame);
	 
}

void UI_IG_Weapon::UpdateLevel()
{
	int diference = weapon_level - current_player_level;

	if (last_player_level != current_player_level && diference <= 4 && diference >= - 4)
	{
		UI_ImageDef img_def;

		if (diference > 0)
		{
			img_def.sprite_section = { 460, 160, 15, 9 };
		}
		else
		{
			img_def.sprite_section = { 460, 170, 15, 9 };
		}


		for (std::list <UI_Image*>::iterator iter = level_indicators.begin(); iter != level_indicators.end(); ++iter)
		{
			(*iter)->to_destroy = true;
		}

		level_indicators.clear();

		for (int i = 0; i < abs( diference) ; ++i)
		{
			UI_Image* aux = app->ui->CreateImage(weapon_frame->position - fPoint(34.f , 59.f -9.f * i), img_def);
			aux->SetParent(weapon_frame);
			aux->camera = player_gui->player->camera_player;
			level_indicators.push_back(aux);
		}

		last_player_level = current_player_level;
	}
	
}

bool UI_IG_Weapon::Update(float dt)
{
	weapon_frame->SetPos(app->map->MapToCamera(object->pos_map, player_gui->player->camera_player));
	UpdateLevel();

	if (SDL_HasIntersection(&(SDL_Rect)player_gui->viewport, &weapon_frame->GetDrawRect()))
	{
		weapon_frame->state = ELEMENT_STATE::VISIBLE;
	}
	else
	{
		weapon_frame->state = ELEMENT_STATE::HIDDEN;
	}

	if (SDL_HasIntersection( &player_gui->viewport_with_margin , &weapon_frame->GetDrawRect()))
	{
		arrow_image->state = ELEMENT_STATE::HIDDEN;
	}
	else
	{
		arrow_image->state = ELEMENT_STATE::VISIBLE;
		UpdateArrow();
	}

	return true;
}

UI_IG_Item::UI_IG_Item(const fPoint position, const UI_InGameElementDef definition, UI_Listener * listener) : UI_InGameElement(position, definition, listener)
{
	UI_ImageDef img_def;

	fPoint camera_pos = app->map->MapToCamera(object->pos_map, player_gui->player->camera_player);

	img_def.sprite_section = { 385, 160, 70, 80 };
	item_frame = app->ui->CreateImage(camera_pos, img_def);
	item_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
	item_frame->camera = player_gui->player->camera_player;

	switch (object->type)
	{
	case ObjectType::HEALTH_BAG:
		img_def.sprite_section = { 490,110,45,45 };
		break;
	default:
		break;
	}

	item_icon = app->ui->CreateImage(camera_pos + fPoint(0.f, 26.f), img_def);
	item_icon->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
	item_icon->SetParent(item_frame);
	item_icon->camera = player_gui->player->camera_player;
	
}	

bool UI_IG_Item::Update(float dt)
{
	if (SDL_HasIntersection(&player_gui->viewport_with_margin, &item_frame->GetDrawRect()))
	{
		arrow_image->state = ELEMENT_STATE::HIDDEN;
	}
	else
	{
		arrow_image->state = ELEMENT_STATE::VISIBLE;
		UpdateArrow();
	}

	return true;
}
