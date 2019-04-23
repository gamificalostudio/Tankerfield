#include "Brofiler/Brofiler.h"
#include "Player_GUI.h"
#include "App.h"
#include "Defs.h"
#include "Log.h"
#include <typeinfo>

#include "M_Window.h"
#include "M_UI.h"
#include "M_Render.h"
#include "M_Fonts.h"

#include "Object.h"
#include "M_ObjManager.h"
#include "Obj_Tank.h"
#include "WeaponInfo.h"

#include "UI_Image.h"
#include "UI_InGameElement.h"
#include "UI_Label.h"
#include "UI_Bar.h"
#include "Camera.h"

Player_GUI::Player_GUI(const GUI_TYPE type, Obj_Tank * player_object) : type(type), player(player_object)
{
	viewport.create(player_object->camera_player->screen_section.x, player_object->camera_player->screen_section.y, player_object->camera_player->screen_section.w, player_object->camera_player->screen_section.h);
	viewport_with_margin = { (int)(viewport.GetLeft() + margin.x * 0.5f) ,  (int)(viewport.GetTop() + +margin.y * 0.5f) , (int)(viewport.w - margin.x) ,(int)(viewport.h - margin.y) };
	margin = { 30.f, 30.f };

	// In Game Elements =====================================================

	UI_InGameElementDef arrow_def;
	arrow_def.is_arrow_actived = true;
	arrow_def.pointed_obj = player_object;

	switch (type)
	{
	case GUI_TYPE::PLAYER_1:
		arrow_def.arrow_color = ARROW_COLOR::GREEN;
		break;
	case GUI_TYPE::PLAYER_2:
		arrow_def.arrow_color = ARROW_COLOR::BLUE;
		break;
	case GUI_TYPE::PLAYER_3:
		arrow_def.arrow_color = ARROW_COLOR::PINK;
		break;
	case GUI_TYPE::PLAYER_4:
		arrow_def.arrow_color = ARROW_COLOR::ORANGE;
		break;
	}

	player_arrow = app->ui->CreateInGameElement( fPoint(0.f, 0.f), arrow_def );

	// HUD  Elements ========================================================

	UI_ImageDef image_def;

	image_def.sprite_section = { 80, 10, 65, 65 };        

	if (type == GUI_TYPE::PLAYER_1 || type == GUI_TYPE::PLAYER_2)
	{
		item_frame = app->ui->CreateImage({ viewport.GetLeft() + margin.x + 30.f, viewport.GetTop() + margin.y + 30.f }, image_def);
		item_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	}
	else if (type == GUI_TYPE::PLAYER_3 || type == GUI_TYPE::PLAYER_4)
	{
		item_frame = app->ui->CreateImage({ viewport.GetLeft() + margin.x + 30.f, viewport.GetBottom() - margin.y - 30.f }, image_def);
		item_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	}
	image_def.sprite_section = { 0, 0, 0, 0 };

	if (type == GUI_TYPE::PLAYER_1 || type == GUI_TYPE::PLAYER_2)
	{
		item_icon = app->ui->CreateImage({ viewport.GetLeft() + margin.x + 30.f, viewport.GetTop() + margin.y + 30.f }, image_def);
		item_icon->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	}
	else if (type == GUI_TYPE::PLAYER_3 || type == GUI_TYPE::PLAYER_4)
	{
		item_icon = app->ui->CreateImage({ viewport.GetLeft() + margin.x + 30.f, viewport.GetBottom() - margin.y - 30.f }, image_def);
		item_icon->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	}

	image_def.sprite_section = { 10, 10, 60, 60 };

	if (type == GUI_TYPE::PLAYER_1 || type == GUI_TYPE::PLAYER_2)
	{
		weapon_frame = app->ui->CreateImage({ viewport.GetRight() - margin.x  ,viewport.GetTop() + margin.y }, image_def);
		weapon_frame->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::TOP);
	}
	else if (type == GUI_TYPE::PLAYER_3 || type == GUI_TYPE::PLAYER_4)
	{
		weapon_frame = app->ui->CreateImage({ viewport.GetRight() - margin.x ,viewport.GetBottom() - margin.y }, image_def);
		weapon_frame->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);
	}
	image_def.sprite_section = { 0, 0, 0, 0 };

	if (type == GUI_TYPE::PLAYER_1 || type == GUI_TYPE::PLAYER_2)
	{
		weapon_icon = app->ui->CreateImage({ viewport.GetRight() - margin.x - 8.f,viewport.GetTop() + margin.y + 8.f }, image_def);
		weapon_icon->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::TOP);
	}
	else if (type == GUI_TYPE::PLAYER_3 || type == GUI_TYPE::PLAYER_4)
	{
		weapon_icon = app->ui->CreateImage({ viewport.GetRight() - margin.x - 8.f ,viewport.GetBottom() - margin.y - 8.f }, image_def);
		weapon_icon->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);
	}

	SetWeaponIcon(WEAPON::BASIC);

	UI_BarDef life_bar_def(UI_Bar::DIR::UP, 1.f, { 0, 160, 0, 255 }, { 80, 80, 80, 255 });
	life_bar_def.section_width = 20.f;
	life_bar_def.section_height = 234.f;

	switch (type)
	{
	case GUI_TYPE::PLAYER_1:
		life_bar_def.direction = UI_Bar::DIR::UP;
		life_bar = app->ui->CreateBar({ viewport.GetLeft() + 10.f, viewport.GetBottom() - 21.f }, life_bar_def);
		life_bar->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::BOTTOM);
		break;
	case GUI_TYPE::PLAYER_2:
		life_bar_def.direction = UI_Bar::DIR::UP;
		life_bar = app->ui->CreateBar({ viewport.GetRight() - 10.f, viewport.GetBottom() - 21.f }, life_bar_def);
		life_bar->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);
		break;
	case GUI_TYPE::PLAYER_3:
		life_bar_def.direction = UI_Bar::DIR::DOWN;
		life_bar = app->ui->CreateBar({ viewport.GetLeft() + 10.f, viewport.GetTop() + 21.f }, life_bar_def);
		life_bar->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::TOP);
		break;
	case GUI_TYPE::PLAYER_4:
		life_bar_def.direction = UI_Bar::DIR::DOWN;
		life_bar = app->ui->CreateBar({ viewport.GetRight() - 10.f, viewport.GetTop() + 21.f }, life_bar_def);
		life_bar->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::TOP);
		break;
	default:
		break;
	}

	UI_BarDef charged_shot_bar_def(UI_Bar::DIR::DOWN, 0.f, { 180, 160, 0, 150 }, { 80, 80, 80, 0 });
	charged_shot_bar_def.section_width = 44.f;
	charged_shot_bar_def.section_height = 44.f;
	
	if (type == GUI_TYPE::PLAYER_1 || type == GUI_TYPE::PLAYER_2)
	{           
		charged_shot_bar_def.direction = UI_Bar::DIR::DOWN;
		charged_shot_bar = app->ui->CreateBar({ viewport.GetRight() - margin.x - 8.f, viewport.GetTop() + margin.y + 8.f }, charged_shot_bar_def);
		charged_shot_bar->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::TOP);
	}
	else if (type == GUI_TYPE::PLAYER_3 || type == GUI_TYPE::PLAYER_4)
	{
		charged_shot_bar_def.direction = UI_Bar::DIR::UP;
		charged_shot_bar = app->ui->CreateBar({ viewport.GetRight() - margin.x - 8.f, viewport.GetBottom() - margin.y - 8.f }, charged_shot_bar_def);
		charged_shot_bar->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);
	}

	if (type == GUI_TYPE::PLAYER_1 || type == GUI_TYPE::PLAYER_2)
	{
		item_helper = app->ui->CreateImage(weapon_frame->position + fPoint(-65.f, 55.f), UI_ImageDef({ app->ui->button_sprites[(int)CONTROLLER_BUTTON::RT] }));
		item_helper->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	}
	else if (type == GUI_TYPE::PLAYER_3 || type == GUI_TYPE::PLAYER_4)
	{
		item_helper = app->ui->CreateImage(weapon_frame->position + fPoint(-65.f, -55.f), UI_ImageDef({ app->ui->button_sprites[(int)CONTROLLER_BUTTON::RT] }));
		item_helper->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	}

	if (type == GUI_TYPE::PLAYER_1 || type == GUI_TYPE::PLAYER_2)
	{
		weapon_helper = app->ui->CreateImage(item_frame->position + fPoint(28.F, 28.F), UI_ImageDef({ app->ui->button_sprites[(int)CONTROLLER_BUTTON::LB] }));
		weapon_helper->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	}
	else if (type == GUI_TYPE::PLAYER_3 || type == GUI_TYPE::PLAYER_4)
	{
		weapon_helper = app->ui->CreateImage(item_frame->position + fPoint(28.F, -28.F), UI_ImageDef({ app->ui->button_sprites[(int)CONTROLLER_BUTTON::LB] }));
		weapon_helper->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	}


	Fade_GUI(true);
}

void Player_GUI::Fade_GUI(bool fade_on)
{
	UI_Fade_FX::FX_TYPE type;

	if (fade_on)
	{
		type = UI_Fade_FX::FX_TYPE::FADE_ON;
	}
	else
	{
		type = UI_Fade_FX::FX_TYPE::FADE_OUT;
	}

	weapon_frame		->SetFX(type, 3.f);
	weapon_helper		->SetFX(type, 3.f);
	weapon_icon			->SetFX(type, 3.f);
	item_frame			->SetFX(type, 3.f);
	item_helper			->SetFX(type, 3.f);
	item_icon			->SetFX(type, 3.f);
	charged_shot_bar	->SetFX(type, 3.f);
	life_bar			->SetFX(type, 3.f);
	
}


void Player_GUI::Update(float dt)
{
	// Update charged shot value ========================================

}

void Player_GUI::SetLifeBar(float life)
{
	life_bar->value = life / (float)player->GetMaxLife();
	life_bar->color_1 = { 255u - (Uint8)(215.f* life_bar->value),  (Uint8)(215.f* life_bar->value) , 0u , 255u };
}

void Player_GUI::SetChargedShotBar(float percent)
{
	if (percent > 1.f)
	{
		percent = 1.f;
	}

	charged_shot_bar->value = percent;
}

void Player_GUI::SetWeaponIcon(WEAPON weapon_type)
{

	switch (weapon_type)
	{
	case WEAPON::DOUBLE_MISSILE:
		weapon_icon->sprite_section = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_DOUBLE_MISSILE];
		break;
	case WEAPON::HEALING_SHOT:
		weapon_icon->sprite_section = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_HEALING_SHOT];
		break;
	case WEAPON::FLAMETHROWER:
		weapon_icon->sprite_section = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_FLAMETHROWER];
		break;
	case WEAPON::LASER_SHOT:
		weapon_icon->sprite_section = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_LASER];
		break;
	case WEAPON::BASIC:
		weapon_icon->sprite_section = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_BASIC];
		break;
	}
	weapon_icon->SetFX(UI_Fade_FX::FX_TYPE::INTERMITTENT, 1, 3.5F);

}

void Player_GUI::SetItemIcon( ObjectType type)
{
	item_icon->SetState(ELEMENT_STATE::VISIBLE);

	switch (type)
	{
	case ObjectType::HEALTH_BAG:
		item_icon->sprite_section = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::ITEM_HEALTH_BAG];
		break;
	case ObjectType::HAPPY_HOUR_ITEM:
		item_icon->sprite_section = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::ITEM_HAPPY_HOUR];
		break;
	case ObjectType::NO_TYPE:
		item_icon->SetState(ELEMENT_STATE::HIDDEN);
		break;
	}
	item_icon->SetFX(UI_Fade_FX::FX_TYPE::INTERMITTENT, 1, 3.5F);

}


void Player_GUI::ClearHelpers()
{
	for (std::vector<UI_Element*>::iterator iter = helper_elements.begin(); iter != helper_elements.end(); ++iter)
	{
		(*iter)->Destroy();
	}

	helper_elements.clear();
}

void Player_GUI::SetHelper()
{

}

void Player_GUI::AddButtonHelper( const CONTROLLER_BUTTON button_type)
{
	app->ui->CreateImage({ 0.f, 0.f }, UI_ImageDef(app->ui->button_sprites[(int)button_type]));

}

void Player_GUI::AddTextHelper(const std::string text)
{
	app->ui->CreateLabel({ 0.f, 0.f }, UI_LabelDef(text, app->font->font_open_sants_bold_12));
}

Player_GUI::~Player_GUI()
{
	if (app->on_clean_up == false)
	{
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

		if (item_frame != nullptr)
		{
			item_frame->Destroy();
			item_frame = nullptr;
		}

		if (item_icon != nullptr)
		{
			item_icon->Destroy();
			item_icon = nullptr;
		}
		if (life_bar != nullptr)
		{
			life_bar->Destroy();
			life_bar = nullptr;
		}
		if (weapon_helper != nullptr)
		{
			weapon_helper->Destroy();
			weapon_helper = nullptr;
		}
		if (item_helper != nullptr)
		{
			item_helper->Destroy();
			item_helper = nullptr;
		}
	}
}


//
//if (type == TYPE::PLAYER_1 || type == TYPE::PLAYER_2)
//{
//	image_def.sprite_section = { 10, 70, 50, 20 };
//	ammo_image = app->ui->CreateImage({ viewport.GetRight() - 24.f - margin.x , viewport.GetTop() + 50.f + margin.y }, image_def);
//	ammo_image->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::TOP);
//}
//else if (type == TYPE::PLAYER_3 || type == TYPE::PLAYER_4)
//{
//	image_def.sprite_section = { 10, 100, 50, 20 };
//	ammo_image = app->ui->CreateImage({ viewport.GetRight() - 24.f - margin.x , viewport.GetBottom() - 50.f - margin.y }, image_def);
//	ammo_image->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);
//}