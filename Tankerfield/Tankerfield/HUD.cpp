#include "Brofiler/Brofiler.h"
#include "HUD.h"
#include "App.h"
#include "Defs.h"
#include "Log.h"

#include "M_Window.h"
#include "M_UI.h"

#include "UI_Image.h"
#include "UI_Bar.h"


HUD::HUD(HUD::TYPE type, Obj_Tank * target): type(type), target(target)
{
	// Position ======================================

	fRect screen = app->win->GetWindowRect();
	fPoint margin = { 30.f, 30.f };
	fRect viewport;

	switch (type)
	{
	case TYPE::SINGLE_PLAYER:
		viewport.create(0.f, 0.f, screen.w, screen.h);
		break;
	case TYPE::PLAYER_1:
		viewport.create(0.f, 0.f, screen.w * .5f, screen.h * .5f);
		break;
	case TYPE::PLAYER_2:
		viewport.create(screen.w * .5f, 0.f, screen.w * .5f, screen.h * .5f);
		break;
	case TYPE::PLAYER_3:
		viewport.create(0.f, screen.h * .5f, screen.w * .5f, screen.h * .5f);
		break;
	case TYPE::PLAYER_4:
		viewport.create(screen.w * .5f, screen.h * .5f, screen.w * .5f, screen.h * .5f);
		break;
	}

	UI_ImageDef image_def;

	// Individual player ========================================================

	image_def.sprite_section = { 100, 10, 50, 50 };

	if (type == TYPE::PLAYER_1 || type == TYPE::PLAYER_2)
	{
		basic_weapon_frame = app->ui->CreateImage({ viewport.GetLeft() + margin.x , viewport.GetTop() + margin.y }, image_def);
		basic_weapon_frame->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::TOP);
	}
	else if (type == TYPE::PLAYER_3 || type == TYPE::PLAYER_4)
	{
		basic_weapon_frame = app->ui->CreateImage({ viewport.GetLeft() + margin.x , viewport.GetBottom() - margin.y }, image_def);
		basic_weapon_frame->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::BOTTOM);
	}

	image_def.sprite_section = { 100, 70, 70, 70 };

	if (type == TYPE::PLAYER_1 || type == TYPE::PLAYER_2)
	{
		item_frame = app->ui->CreateImage({ viewport.GetLeft() + margin.x + 25.f ,viewport.GetTop() + margin.y + 90.f }, image_def);
		item_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	}
	else if (type == TYPE::PLAYER_3 || type == TYPE::PLAYER_4)
	{
		item_frame = app->ui->CreateImage({ viewport.GetLeft() + margin.x + 25.f ,viewport.GetBottom() - margin.y - 90.f }, image_def);
		item_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	}

	if (type == TYPE::PLAYER_1 || type == TYPE::PLAYER_2)
	{
		image_def.sprite_section = { 10, 70, 50, 20 };
		ammo_image = app->ui->CreateImage({ viewport.GetRight() - 24.f - margin.x , viewport.GetTop() + 50.f + margin.y }, image_def);
		ammo_image->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::TOP);
	}
	else if (type == TYPE::PLAYER_3 || type == TYPE::PLAYER_4)
	{
		image_def.sprite_section = { 10, 100, 50, 20 };
		ammo_image = app->ui->CreateImage({ viewport.GetRight() - 24.f - margin.x , viewport.GetBottom() - 50.f - margin.y }, image_def);
		ammo_image->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);
	}

	image_def.sprite_section = { 10, 10, 50, 50 };

	if (type == TYPE::PLAYER_1 || type == TYPE::PLAYER_2)
	{
		special_weapon_frame = app->ui->CreateImage({ viewport.GetRight() - 24.f - margin.x ,viewport.GetTop() + margin.y }, image_def);
		special_weapon_frame->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::TOP);
	}
	else if (type == TYPE::PLAYER_3 || type == TYPE::PLAYER_4)
	{
		special_weapon_frame = app->ui->CreateImage({ viewport.GetRight() - 24.f - margin.x ,viewport.GetBottom() - margin.y }, image_def);
		special_weapon_frame->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);
	}

	UI_BarDef ammo_bar_def(UI_Bar::DIR::DOWN, 0.8f, { 180, 160, 0, 255 }, { 80, 80, 80, 255 });
	ammo_bar_def.section_width = 12.f;
	ammo_bar_def.section_height = 128.f;
	ammo_bar_def.section_offset = { 6.f ,6.f };
	ammo_bar_def.sprite_section = { 70, 10, 24, 140 };

	if (type == TYPE::PLAYER_1 || type == TYPE::PLAYER_2)
	{
		ammo_bar_def.direction = UI_Bar::DIR::DOWN;
		ammo_bar = app->ui->CreateBar({ viewport.GetRight() - margin.x , viewport.GetTop() + margin.y }, ammo_bar_def);
		ammo_bar->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::TOP);
	}
	else if (type == TYPE::PLAYER_3 || type == TYPE::PLAYER_4)
	{
		ammo_bar_def.direction = UI_Bar::DIR::UP;
		ammo_bar = app->ui->CreateBar({ viewport.GetRight() - margin.x , viewport.GetBottom() - margin.y }, ammo_bar_def);
		ammo_bar->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);
	}

	UI_BarDef life_bar_def(UI_Bar::DIR::UP, 0.8f, { 0, 160, 0, 255 }, { 80, 80, 80, 255 });
	life_bar_def.section_width = 20.f;
	life_bar_def.section_height = 234.f;

	switch (type)
	{
	case HUD::TYPE::PLAYER_1:
		life_bar_def.direction = UI_Bar::DIR::UP;
		life_bar = app->ui->CreateBar({ viewport.GetLeft() + 10.f, viewport.GetBottom() - 21.f }, life_bar_def);
		life_bar->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::BOTTOM);
		break;
	case HUD::TYPE::PLAYER_2:
		life_bar_def.direction = UI_Bar::DIR::UP;
		life_bar = app->ui->CreateBar({ viewport.GetRight() - 10.f, viewport.GetBottom() - 21.f }, life_bar_def);
		life_bar->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);
		break;
	case HUD::TYPE::PLAYER_3:
		life_bar_def.direction = UI_Bar::DIR::DOWN;
		life_bar = app->ui->CreateBar({ viewport.GetLeft() + 10.f, viewport.GetTop() + 21.f }, life_bar_def);
		life_bar->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::TOP);
		break;
	case HUD::TYPE::PLAYER_4:
		life_bar_def.direction = UI_Bar::DIR::DOWN;
		life_bar = app->ui->CreateBar({ viewport.GetRight() - 10.f, viewport.GetTop() + 21.f }, life_bar_def);
		life_bar->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::TOP);
		break;
	default:
		break;
	}

}

HUD::~HUD()
{
	app->ui->DeleteObject(basic_weapon_frame);
	app->ui->DeleteObject(special_weapon_frame);
	app->ui->DeleteObject(item_frame);
	app->ui->DeleteObject(ammo_image);
	app->ui->DeleteObject(ammo_bar);
	app->ui->DeleteObject(life_bar);

	basic_weapon_frame		= nullptr;
	special_weapon_frame	= nullptr;
	item_frame				= nullptr;
	ammo_image				= nullptr;
	ammo_bar				= nullptr;
	life_bar				= nullptr;
}
