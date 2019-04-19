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

#include "Point.h"

UI_InGameElement::UI_InGameElement(const fPoint position, const UI_InGameElementDef definition) : UI_Element(position, definition, nullptr), pointed_obj(definition.pointed_obj), is_arrow_actived(definition.is_arrow_actived)
{
	// Set Arrow =================================================

	if (definition.is_arrow_actived == true)
	{
		switch (definition.arrow_color)
		{
		case ARROW_COLOR::GREEN:
			arrow_animation = &app->ui->green_arrow_anim;
			break;
		case ARROW_COLOR::BLUE:
			arrow_animation = &app->ui->blue_arrow_anim;
			break;
		case ARROW_COLOR::PINK:
			arrow_animation = &app->ui->pink_arrow_anim;
			break;
		case ARROW_COLOR::ORANGE:
			arrow_animation = &app->ui->orange_arrow_anim;
			break;
		}
	}

	
}

bool UI_InGameElement::Update(float dt)
{
	if (pointed_obj != nullptr)
	{
		SetPos(pointed_obj->pos_map);
	}
	return true;
}

bool UI_InGameElement::Draw()
{
	if (pointed_obj == nullptr || is_arrow_actived == false)
	{
		return true;
	}

	fPoint screen_pos = app->map->MapToCamera(pointed_obj->pos_map, app->ui->current_camera);
	SDL_Point screen_point;

	screen_point.x = (int)screen_pos.x;
	screen_point.y = (int)screen_pos.y;

	if (SDL_PointInRect( &screen_point, &app->ui->current_gui->viewport_with_margin) == false)
	{
		fPoint vector = app->ui->current_gui->player->pos_map - pointed_obj->pos_map;
		sprite_section = arrow_animation->GetFrame(atan2(vector.y, vector.x) * RADTODEG);
		vector.Normalize();
		screen_pos = app->map->MapToCamera(app->ui->current_gui->player->pos_map - vector * 2.f, app->ui->current_camera);
		app->render->BlitUI( app->ui->GetAtlas(), screen_pos.x - (float)sprite_section.w * 0.5f, screen_pos.y - (float)sprite_section.h * 0.5f, &sprite_section ,app->ui->current_camera);
	}
}

UI_IG_Weapon::UI_IG_Weapon(const fPoint position, const UI_InGameElementDef definition) : UI_InGameElement(position, definition)
{
	UI_ImageDef img_def;

	// Add frame ====================================================

	img_def.sprite_section = { 330, 160, 50, 70 };
	img_def.is_in_game = true;

	weapon_frame = app->ui->CreateImage(position, img_def);
	weapon_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
	weapon_frame->SetParent(this);

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

	weapon_icon = app->ui->CreateImage(position + app->map->ScreenToMapF(0.f, -29.f), img_def);
	weapon_icon->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
	weapon_icon->SetParent(weapon_frame);

	level_indicator = app->ui->CreateImage(position + app->map->ScreenToMapF(-32.f,- 64.f), img_def);
	level_indicator->SetParent(weapon_frame);
	UpdateLevel();


	weapon_frame	->SetFX(UI_Fade_FX::FX_TYPE::FADE, 2.F, 0.F, 255.F);
	weapon_icon		->SetFX(UI_Fade_FX::FX_TYPE::FADE, 2.F, 0.F, 255.F);
	level_indicator	->SetFX(UI_Fade_FX::FX_TYPE::FADE, 2.F, 0.F, 255.F);
}

bool UI_IG_Weapon::Draw()
{
	UpdateLevel();
	return true;
}

void UI_IG_Weapon::UpdateLevel()
{
	Obj_PickUp* pick_up = (Obj_PickUp*) pointed_obj;
	int diference = pick_up->level_of_weapon - app->ui->current_gui->player->GetWeaponInfo().level_weapon;

	level_indicator->SetState(ELEMENT_STATE::VISIBLE);

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
		level_indicator->SetState(ELEMENT_STATE::HIDDEN);
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

void UI_IG_Weapon::Destroy()
{
	to_destroy = true;

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
	item_frame = app->ui->CreateImage( position, img_def);
	item_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::BOTTOM);
	item_frame->SetParent(this);

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

	item_icon = app->ui->CreateImage( position + app->map->ScreenToMapF(0.f, - 42.f), img_def);
	item_icon->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	item_icon->SetParent(item_frame);
	
	item_icon->SetFX(UI_Fade_FX::FX_TYPE::FADE, 2.F, 0.F, 255.F);
	item_frame->SetFX(UI_Fade_FX::FX_TYPE::FADE, 2.F, 0.F, 255.F);
}

void UI_IG_Item::Destroy()
{
	to_destroy = true;

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
}

void UI_IG_Helper::AddButtonHelper( const M_UI::GAMEPAD_BUTTON button_type, const fPoint offset)
{
	UI_ImageDef def(app->ui->button_sprites[(int)button_type]);
	def.is_in_game = true;

	UI_Image*  ui_helper = app->ui->CreateImage(position + app->map->ScreenToMapF(offset.x, offset.y), def);
	ui_helper->single_camera = this->single_camera;
	ui_helper->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	ui_helper->SetParent(this);
	helper_elements.push_back(ui_helper);
}

void UI_IG_Helper::AddTextHelper(const String text, const fPoint offset)
{
	UI_LabelDef def(text, app->ui->font_open_sants_bold_12);
	def.is_in_game = true;

	UI_Label* ui_helper = app->ui->CreateLabel(position + app->map->ScreenToMapF(offset.x, offset.y), def);
	ui_helper->single_camera = this->single_camera;
	ui_helper->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	ui_helper->SetParent(this);

	helper_elements.push_back(ui_helper);
}

void UI_IG_Helper::Destroy()
{
	to_destroy = true;

	for (std::vector < UI_Element*>::iterator iter = helper_elements.begin() ;  iter !=helper_elements.end(); ++iter)
	{
		(*iter)->Destroy();
	}
}
