#include "UI_InGameElement.h"
#include "App.h"
#include "M_UI.h"
#include "M_Map.h"
#include "Player_GUI.h"

#include "Obj_Tank.h"
#include "Obj_PickUp.h"
#include "Object.h"

#include "UI_Image.h"

UI_InGameElement::UI_InGameElement(const fPoint position, const UI_InGameElementDef definition): UI_Element(position, definition, nullptr),  map_pos(definition.map_pos), pointed_obj(definition.pointed_obj)
{
	// Set Arrow =================================================

	if (definition.add_arrow == true)
	{
		UI_ImageDef image_def;
		image_def.image_animation = app->ui->arrow_anim;
		arrow_image = app->ui->CreateInGameImage({ 0.f, 0.f }, image_def);
		arrow_image->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	}
}



bool UI_InGameElement::Update(float dt)
{
	UpdateArrow();
	return true;
}

void UI_InGameElement::Destroy()
{
	if (arrow_image != nullptr)
	{
		arrow_image->Destroy();
	}
}

void UI_InGameElement::UpdateArrow()
{
	if (arrow_image == nullptr)
	{
		return;
	}
	
	if (SDL_HasIntersection(&app->ui->current_gui->viewport_with_margin, &main_element->GetDrawRect()))
	{
		arrow_image->state = ELEMENT_STATE::HIDDEN;
	}
	else
	{
		arrow_image->state = ELEMENT_STATE::VISIBLE;

		fPoint vector = app->ui->current_gui->player->pos_map - pointed_obj->pos_map;
		arrow_image->sprite_section = arrow_image->image_animation->GetFrame(atan2(vector.y, vector.x) * RADTODEG);
		vector.Normalize();
		arrow_image->SetPos(app->map->MapToCamera(app->ui->current_gui->player->pos_map - vector * 1.5f, app->ui->current_gui->player->camera_player));
	}
}

UI_IG_Weapon::UI_IG_Weapon(const fPoint position, const UI_InGameElementDef definition) : UI_InGameElement(position, definition)
{
	UI_ImageDef img_def;

	// Add frame ====================================================

	img_def.sprite_section = { 330, 160, 50, 65 };
	
	weapon_frame = app->ui->CreateInGameImage({ 0.f ,0.f }, img_def);
	weapon_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
	main_element = weapon_frame;

	// Add icon ====================================================

	Obj_PickUp* pick_up_obj = (Obj_PickUp*)pointed_obj;
	
	switch (pick_up_obj->type_of_weapon)
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
	default:
		img_def.sprite_section = { 500, 10, 34, 34 };
		break;
	}
	img_def.sprite_section = { 620, 10, 34, 34 };
	weapon_icon = app->ui->CreateInGameImage(weapon_frame->position - fPoint(0.f, 24.f), img_def);
	weapon_icon->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
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
			(*iter)->Destroy();
		}

		level_indicators.clear();

		for (int i = 0; i < abs( diference) ; ++i)
		{
			UI_Image* aux = app->ui->CreateInGameImage(weapon_frame->position - fPoint(34.f , 59.f -9.f * i), img_def);
			aux->SetParent(weapon_frame);
			level_indicators.push_back(aux);
		}

		last_player_level = current_player_level;
	}
	
}

bool UI_IG_Weapon::Update(float dt)
{
	weapon_frame->SetPos(app->map->MapToCamera(pointed_obj->pos_map, app->ui->current_gui->player->camera_player));
	UpdateLevel();
	UpdateArrow();

	return true;
}

void UI_IG_Weapon::Destroy()
{
	if (arrow_image != nullptr)
	{
		arrow_image->Destroy();
	}
	if (weapon_frame != nullptr)
	{
		weapon_frame->Destroy();
	}
	if (weapon_icon != nullptr)
	{
		weapon_icon->Destroy();
	}

	for (std::list <UI_Image*>::iterator iter = level_indicators.begin(); iter != level_indicators.end(); ++iter)
	{
		(*iter)->Destroy();
	}
}

UI_IG_Item::UI_IG_Item(const fPoint position, const UI_InGameElementDef definition) : UI_InGameElement(position, definition)
{
	UI_ImageDef img_def;

	// Add frame ====================================================

	img_def.sprite_section = { 385, 160, 70, 80 };
	item_frame = app->ui->CreateInGameImage({ 0.f, 0.f }, img_def);
	item_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
	main_element = item_frame;

	// Add icon ====================================================

	switch (pointed_obj->type)
	{
	case ObjectType::HEALTH_BAG:
		img_def.sprite_section = { 490,110,45,45 };
		break;
	default:
		img_def.sprite_section = { 490,110,45,45 };
		break;
	}

	item_icon = app->ui->CreateInGameImage({ 0.f, - 26.f }, img_def);
	item_icon->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
	item_icon->SetParent(item_frame);
	
}
bool UI_IG_Item::Update(float dt)
{
	item_frame->SetPos(app->map->MapToCamera(pointed_obj->pos_map, app->ui->current_gui->player->camera_player));
	UpdateArrow();
	return true;
}

void UI_IG_Item::Destroy()
{
	if (arrow_image != nullptr)
	{
		arrow_image->Destroy();
	}
	if (item_frame != nullptr)
	{
		item_frame->Destroy();
	}
	if (item_icon != nullptr)
	{
		item_icon->Destroy();
	}
}

