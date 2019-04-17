#include "Brofiler/Brofiler.h"
#include "Player_GUI.h"
#include "App.h"
#include "Defs.h"
#include "Log.h"
#include <typeinfo>

#include "M_Window.h"
#include "M_UI.h"
#include "M_Render.h"

#include "Object.h"
#include "Obj_Tank.h"

#include "UI_Image.h"
#include "UI_Label.h"
#include "UI_Bar.h"

Player_GUI::Player_GUI(const Player_GUI::TYPE type, Obj_Tank * player_object): type(type), player(player_object)
{
	viewport.create( player_object->camera_player->viewport.x, player_object->camera_player->viewport.y , player_object->camera_player->viewport.w, player_object->camera_player->viewport.h);
	viewport_with_margin = { (int) (viewport.GetLeft() + margin.x * 0.5f) ,  (int)(viewport.GetTop() + +margin.y * 0.5f) , (int)(viewport.w - margin.x) ,(int)(viewport.h - margin.y) };
	margin = { 30.f, 30.f };

	UI_ImageDef image_def;

	// Individual player ========================================================

	image_def.sprite_section = { 100, 70, 70, 70 };

	if (type == TYPE::PLAYER_1 || type == TYPE::PLAYER_2)
	{
		item_frame = app->ui->CreateImage({ viewport.GetLeft() + margin.x + 30.f, viewport.GetTop() + margin.y + 30.f }, image_def);
		item_frame->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	}
	else if (type == TYPE::PLAYER_3 || type == TYPE::PLAYER_4)
	{
		item_frame = app->ui->CreateImage({ viewport.GetLeft() + margin.x + 30.f, viewport.GetBottom() - margin.y - 30.f }, image_def);
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
	weapon_frame = app->ui->CreateImage({ viewport.GetRight() - 24.f - margin.x ,viewport.GetTop() + margin.y }, image_def);
	weapon_frame->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::TOP);
}
else if (type == TYPE::PLAYER_3 || type == TYPE::PLAYER_4)
{
	weapon_frame = app->ui->CreateImage({ viewport.GetRight() - 24.f - margin.x ,viewport.GetBottom() - margin.y }, image_def);
	weapon_frame->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);
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
case Player_GUI::TYPE::PLAYER_1:
	life_bar_def.direction = UI_Bar::DIR::UP;
	life_bar = app->ui->CreateBar({ viewport.GetLeft() + 10.f, viewport.GetBottom() - 21.f }, life_bar_def);
	life_bar->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::BOTTOM);
	break;
case Player_GUI::TYPE::PLAYER_2:
	life_bar_def.direction = UI_Bar::DIR::UP;
	life_bar = app->ui->CreateBar({ viewport.GetRight() - 10.f, viewport.GetBottom() - 21.f }, life_bar_def);
	life_bar->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::BOTTOM);
	break;
case Player_GUI::TYPE::PLAYER_3:
	life_bar_def.direction = UI_Bar::DIR::DOWN;
	life_bar = app->ui->CreateBar({ viewport.GetLeft() + 10.f, viewport.GetTop() + 21.f }, life_bar_def);
	life_bar->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::TOP);
	break;
case Player_GUI::TYPE::PLAYER_4:
	life_bar_def.direction = UI_Bar::DIR::DOWN;
	life_bar = app->ui->CreateBar({ viewport.GetRight() - 10.f, viewport.GetTop() + 21.f }, life_bar_def);
	life_bar->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::TOP);
	break;
default:
	break;
}
}

void Player_GUI::SetHelper()
{
	fPoint    helper_pos = { viewport.GetLeft() + viewport.w * 0.5f, viewport.GetBottom() - 50.f };

	float x_offset = 0.f;

	UI_Element* aux = nullptr;

	for (std::vector<Helper*>::iterator iter = helpers_vector.begin(); iter != helpers_vector.end(); ++iter)
	{
		if ((*iter)->type == Helper::HELPER_TYPE::TEXT)
		{
			Text_Helper* helper =(Text_Helper*) (*iter);
			aux = app->ui->CreateLabel({ 0.f, 0.f }, UI_LabelDef(helper->text, app->ui->font_open_sants_bold_12));

		}
		else if ((*iter)->type == Helper::HELPER_TYPE::BUTTON)
		{
			Button_Helper* helper = (Button_Helper*) (*iter);
			aux = app->ui->CreateImage({ 0.f, 0.f }, UI_ImageDef(app->ui->button_sprite[(int)helper->button_type]));

		}

		x_offset += aux->sprite_section.w + (*iter)->x_margin;
		aux->offset = (*iter)->x_margin;
		aux->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::CENTER);
		helper_elements.push_back(aux);
	}

	x_offset *= 0.5f;

	float cumulated_x = 0.f;

	for (std::vector<UI_Element*>::iterator iter = helper_elements.begin(); iter != helper_elements.end(); ++iter)
	{
		(*iter)->SetPos(fPoint(helper_pos.x - x_offset + cumulated_x, helper_pos.y));
		cumulated_x += ( (*iter)->sprite_section.w + (*iter)->offset );
	}
}

void Player_GUI::AddButtonHelper(const Button_Helper helper)
{
	Button_Helper* new_helper = new Button_Helper(helper);
	helpers_vector.push_back(new_helper);
}

void Player_GUI::AddTextHelper(const Text_Helper helper)
{
	Text_Helper* new_helper = new Text_Helper(helper);
	helpers_vector.push_back(new_helper);
}


void Player_GUI::SetLife(float life)
{

}


Player_GUI::~Player_GUI()
{
	if (app->on_clean_up == false)
	{
		weapon_frame->Destroy();
		item_frame->Destroy();
		ammo_image->Destroy();
		ammo_bar->Destroy();
		life_bar->Destroy();

		weapon_frame = nullptr;
		item_frame = nullptr;
		ammo_image = nullptr;
		ammo_bar = nullptr;
		life_bar = nullptr;
	}

}
