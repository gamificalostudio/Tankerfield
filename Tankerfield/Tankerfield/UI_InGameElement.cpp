#include "UI_InGameElement.h"
#include "App.h"
#include "M_UI.h"
#include "M_Map.h"
#include "Player_GUI.h"

#include "Obj_Tank.h"
#include "Obj_PickUp.h"
#include "Object.h"

#include "UI_Image.h"
#include "UI_Label.h"

UI_InGameElement::UI_InGameElement(const fPoint position, const UI_InGameElementDef definition): UI_Element(position, definition, nullptr),  map_pos(definition.map_pos), pointed_obj(definition.pointed_obj)
{
	// Set Arrow =================================================

	if (definition.add_arrow == true)
	{
		UI_ImageDef image_def;

		switch (definition.arrow_color)
		{
		case ARROW_COLOR::GREEN:
			image_def.image_animation = &app->ui->green_arrow_anim;
			break;
		case ARROW_COLOR::BLUE:
			image_def.image_animation = &app->ui->blue_arrow_anim;
			break;
		case ARROW_COLOR::PINK:
			image_def.image_animation = &app->ui->pink_arrow_anim;
			break;
		case ARROW_COLOR::ORANGE:
			image_def.image_animation = &app->ui->orange_arrow_anim;
			break;
		}

		image_def.is_in_game = true;
		arrow_image = app->ui->CreateImage({ 0.f, 0.f }, image_def);
		arrow_image->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
		arrow_image->SetParent(this);
	}

	
}

bool UI_InGameElement::PostUpdate()
{
	UpdateArrow();
	return true;
}

void UI_InGameElement::Destroy()
{
	to_destroy = true;

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

	img_def.sprite_section = { 330, 160, 50, 70 };
	img_def.is_in_game = true;

	weapon_frame = app->ui->CreateImage({ 0.f ,0.f }, img_def);
	weapon_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
	weapon_frame->SetParent(this);
	main_element = weapon_frame;

	// Add icon ====================================================

	Obj_PickUp* pick_up_obj = (Obj_PickUp*)pointed_obj;
	
	switch (pick_up_obj->type_of_weapon)
	{
	case WEAPON::DOUBLE_MISSILE:
		img_def.sprite_section = app->ui->icon_sprites[(int)M_UI::ICON_SIZE::SMALL][(int)M_UI::ICON_TYPE::WEAPON_DOUBLE_MISSILE];
		break;
	case WEAPON::FLAMETHROWER:
		img_def.sprite_section = app->ui->icon_sprites[(int)M_UI::ICON_SIZE::SMALL][(int)M_UI::ICON_TYPE::WEAPON_FLAMETHROWER];
		break;
	default:
		img_def.sprite_section = app->ui->icon_sprites[(int)M_UI::ICON_SIZE::SMALL][(int)M_UI::ICON_TYPE::WEAPON_FLAMETHROWER];
		break;
	}

	img_def.sprite_section = { 620, 10, 34, 34 };
	weapon_icon = app->ui->CreateImage(weapon_frame->position - fPoint(0.f, 29.f), img_def);
	weapon_icon->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
	weapon_icon->SetParent(weapon_frame);

	level_indicator = app->ui->CreateImage(weapon_frame->position - fPoint( 32.f, 64.f), img_def);
	level_indicator->SetParent(weapon_frame);
	 
}

void UI_IG_Weapon::UpdateLevel()
{
	int diference = pick_up_weapon_level - player_weapon_level;

	level_indicator->state == ELEMENT_STATE::VISIBLE;

	if (diference > 4)
	{
		diference = 4;
	}
	else if (diference < -4)
	{
		diference = -4;
	}
	else if (diference == 0)
	{
		level_indicator->state == ELEMENT_STATE::HIDDEN;
		return;
	}

	if (diference > 0)
	{
		level_indicator->sprite_section = { 460, 160, 15, 10 * abs(diference)};
	}
	else
	{
		level_indicator->sprite_section = { 460, 205, 15, 10 * abs(diference) };
	}



}

bool UI_IG_Weapon::PostUpdate()
{
	weapon_frame->SetPos(app->map->MapToCamera(pointed_obj->pos_map, app->ui->current_gui->player->camera_player));
	UpdateLevel();
	UpdateArrow();

	return true;
}

void UI_IG_Weapon::Destroy()
{
	to_destroy = true;

	if (arrow_image != nullptr)
	{
		arrow_image->Destroy();
		arrow_image = nullptr;
	}
	if (weapon_frame != nullptr)
	{
		weapon_frame->Destroy();
		weapon_frame = nullptr;
	}
	if (weapon_icon != nullptr)
	{
		weapon_icon->Destroy();
		weapon_icon = nullptr;
	}
	if (level_indicator != nullptr)
	{
		level_indicator->Destroy();
		level_indicator = nullptr;
	}

}

UI_IG_Item::UI_IG_Item(const fPoint position, const UI_InGameElementDef definition) : UI_InGameElement(position, definition)
{
	UI_ImageDef img_def;
	img_def.is_in_game = true;
	// Add frame ====================================================

	img_def.sprite_section = { 390, 160, 55, 70 };
	item_frame = app->ui->CreateImage({ 0.f, 0.f }, img_def);
	item_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
	item_frame->SetParent(this);
	main_element = item_frame;

	// Add icon ====================================================
	Obj_PickUp* pick_up_obj = (Obj_PickUp*)pointed_obj;

	switch (pick_up_obj->type_of_item)
	{
	case ObjectType::HEALTH_BAG:
		img_def.sprite_section = app->ui->icon_sprites[(int)M_UI::ICON_SIZE::SMALL][(int)M_UI::ICON_TYPE::ITEM_HEALTH_BAG];
		break;
	case ObjectType::HAPPY_HOUR_ITEM:
		img_def.sprite_section = app->ui->icon_sprites[(int)M_UI::ICON_SIZE::SMALL][(int)M_UI::ICON_TYPE::ITEM_HAPPY_HOUR];
		break;
	default:
		img_def.sprite_section = app->ui->icon_sprites[(int)M_UI::ICON_SIZE::SMALL][(int)M_UI::ICON_TYPE::ITEM_HEALTH_BAG];
		break;
	}

	item_icon = app->ui->CreateImage({ 0.f, - 42.f }, img_def);
	item_icon->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	item_icon->SetParent(item_frame);
	
}
bool UI_IG_Item::PostUpdate()
{
	item_frame->SetPos(app->map->MapToCamera(pointed_obj->pos_map, app->ui->current_gui->player->camera_player));
	UpdateArrow();
	return true;
}

void UI_IG_Item::Destroy()
{
	to_destroy = true;

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

UI_IG_Helper::UI_IG_Helper(const fPoint position, const UI_InGameElementDef definition): UI_InGameElement(position, definition)
{
	UI_ElementDef def;
	def.is_in_game = true;
	map_pos = position;

	if (pointed_obj != nullptr)
	{
		main_element = app->ui->CreateElement(pointed_obj->pos_map, def, nullptr);
	}
	else
	{
		main_element = app->ui->CreateElement(map_pos, def, nullptr);
	}

	main_element->SetParent(this);

}

bool UI_IG_Helper::PostUpdate()
{
	if (pointed_obj != nullptr)
	{
		main_element->SetPos(app->map->MapToCamera(pointed_obj->pos_map, app->ui->current_gui->player->camera_player));
	}
	else
	{
		main_element->SetPos(app->map->MapToCamera(map_pos, app->ui->current_gui->player->camera_player));
	}
	
	return true;
}

void UI_IG_Helper::AddButtonHelper(Button_Helper helper)
{
	UI_ImageDef def(app->ui->button_sprites[(int)helper.button_type]);
	def.is_in_game = true;

	UI_Image*  ui_helpear = app->ui->CreateImage(main_element->position + helper.offset, def);
	ui_helpear->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	ui_helpear->SetParent(main_element);
	helper_elements.push_back(ui_helpear);
}

void UI_IG_Helper::AddTextHelper(Text_Helper helper)
{
	UI_LabelDef def(helper.text, app->ui->font_open_sants_bold_12);
	def.is_in_game = true;

	UI_Label* ui_helpear = app->ui->CreateLabel(main_element->position + helper.offset, def);
	ui_helpear->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	ui_helpear->SetParent(main_element);
	helper_elements.push_back(ui_helpear);
}

void UI_IG_Helper::Destroy()
{
	to_destroy = true;

	main_element->Destroy();

	for (std::vector < UI_Element*>::iterator iter = helper_elements.begin() ;  iter !=helper_elements.end(); ++iter)
	{
		(*iter)->Destroy();
	}
}
