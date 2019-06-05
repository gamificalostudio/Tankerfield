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
#include "Obj_Item.h"

#include "UI_Image.h"
#include "UI_InGameElement.h"
#include "M_Map.h"
#include "UI_Label.h"
#include "UI_Bar.h"
#include "Camera.h"

Player_GUI::Player_GUI(Obj_Tank * player_object) : player(player_object)
{
	// Assets ===============================================================

	flash_texture = app->tex->Load("textures/ui/flash.png");
	margin = { 30.f, 30.f };

	int tank_num = player->GetTankNum();
	 
	viewport.create(
		player_object->camera_player->screen_section.x,
		player_object->camera_player->screen_section.y,
		player_object->camera_player->screen_section.w,
		player_object->camera_player->screen_section.h);

	viewport_with_margin = {
		viewport.GetLeft() + margin.x * 0.5f,
		viewport.GetTop()  + margin.y * 0.5f,
		viewport.w - margin.x,
		viewport.h - margin.y};

	// In Game Elements =====================================================

	UI_InGameElementDef arrow_def;
	arrow_def.is_arrow_actived = true;
	arrow_def.pointed_obj = player_object;

	player_arrow = app->ui->CreateInGameElement( fPoint(0.f, 0.f), arrow_def );

	// HUD  Elements ========================================================

	UI_ImageDef image_def;
	image_def.sprite_section = { 0, 0, 1280, 720 };

	flash = app->ui->CreateImage( fPoint( viewport.GetLeft() ,viewport.GetTop()), image_def);
	flash->SetTexture(flash_texture);
	flash->SetDrawRect((SDL_Rect)viewport);
	flash->alpha = 0.f;

	image_def.sprite_section = { 80, 10, 65, 65 };        

	if (tank_num == 0 || tank_num == 1)
	{
		item_frame = app->ui->CreateImage({ viewport.GetLeft() + margin.x + 30.f, viewport.GetTop() + margin.y + 30.f }, image_def);
		item_frame->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	}
	else if (tank_num == 2 || tank_num == 3)
	{
		item_frame = app->ui->CreateImage({ viewport.GetLeft() + margin.x + 30.f, viewport.GetBottom() - margin.y - 30.f }, image_def);
		item_frame->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	}
	image_def.sprite_section = { 0, 0, 0, 0 };

	if (tank_num == 0 || tank_num == 1)
	{
		item_icon = app->ui->CreateImage({ viewport.GetLeft() + margin.x + 30.f, viewport.GetTop() + margin.y + 30.f }, image_def);
		item_icon->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	}
	else if (tank_num == 2 || tank_num == 3)
	{
		item_icon = app->ui->CreateImage({ viewport.GetLeft() + margin.x + 30.f, viewport.GetBottom() - margin.y - 30.f }, image_def);
		item_icon->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	}

	image_def.sprite_section = { 10, 10, 60, 60 };

	if (tank_num == 0 || tank_num == 1)
	{
		weapon_frame = app->ui->CreateImage({ viewport.GetRight() - margin.x  ,viewport.GetTop() + margin.y }, image_def);
		weapon_frame->SetPivot(Pivot::X::RIGHT, Pivot::Y::TOP);
	}
	else if (tank_num == 2 || tank_num == 3)
	{
		weapon_frame = app->ui->CreateImage({ viewport.GetRight() - margin.x ,viewport.GetBottom() - margin.y }, image_def);
		weapon_frame->SetPivot(Pivot::X::RIGHT, Pivot::Y::BOTTOM);
	}
	image_def.sprite_section = { 0, 0, 0, 0 };

	if (tank_num == 0 || tank_num == 1)
	{
		weapon_icon = app->ui->CreateImage({ viewport.GetRight() - margin.x - 8.f,viewport.GetTop() + margin.y + 8.f }, image_def);
		weapon_icon->SetPivot(Pivot::X::RIGHT, Pivot::Y::TOP);
	}
	else if (tank_num == 2 || tank_num == 3)
	{
		weapon_icon = app->ui->CreateImage({ viewport.GetRight() - margin.x - 8.f ,viewport.GetBottom() - margin.y - 8.f }, image_def);
		weapon_icon->SetPivot(Pivot::X::RIGHT, Pivot::Y::BOTTOM);
	}

	SetWeaponIcon(WEAPON::BASIC);

	UI_BarDef life_bar_def(UI_Bar::DIR::UP, 1.f, { 0, 160, 0, 255 }, { 80, 80, 80, 255 });
	life_bar_def.section_width = 20.f;
	life_bar_def.section_height = 234.f;

	switch (tank_num)
	{
	case 0:
		life_bar_def.direction = UI_Bar::DIR::UP;
		life_bar = app->ui->CreateBar({ viewport.GetLeft() + 50.f, viewport.GetBottom() - 21.f }, life_bar_def);
		life_bar->SetPivot(Pivot::X::LEFT, Pivot::Y::BOTTOM);
		break;
	case 1:
		life_bar_def.direction = UI_Bar::DIR::UP;
		life_bar = app->ui->CreateBar({ viewport.GetRight() - 50.f, viewport.GetBottom() - 21.f }, life_bar_def);
		life_bar->SetPivot(Pivot::X::RIGHT, Pivot::Y::BOTTOM);
		break;
	case 2:
		life_bar_def.direction = UI_Bar::DIR::DOWN;
		life_bar = app->ui->CreateBar({ viewport.GetLeft() + 50.f, viewport.GetTop() + 21.f }, life_bar_def);
		life_bar->SetPivot(Pivot::X::LEFT, Pivot::Y::TOP);
		break;
	case 3:
		life_bar_def.direction = UI_Bar::DIR::DOWN;
		life_bar = app->ui->CreateBar({ viewport.GetRight() - 50.f, viewport.GetTop() + 21.f }, life_bar_def);
		life_bar->SetPivot(Pivot::X::RIGHT, Pivot::Y::TOP);
		break;
	default:
		break;
	}

	UI_BarDef charged_shot_bar_def(UI_Bar::DIR::DOWN, 0.f, { 180, 160, 0, 150 }, { 80, 80, 80, 0 });
	charged_shot_bar_def.section_width = 44.f;
	charged_shot_bar_def.section_height = 44.f;
	
	if (tank_num == 0 || tank_num == 1)
	{           
		charged_shot_bar_def.direction = UI_Bar::DIR::DOWN;
		charged_shot_bar = app->ui->CreateBar({ viewport.GetRight() - margin.x - 8.f, viewport.GetTop() + margin.y + 8.f }, charged_shot_bar_def);
		charged_shot_bar->SetPivot(Pivot::X::RIGHT, Pivot::Y::TOP);
	}
	else if (tank_num == 2 || tank_num == 3)
	{
		charged_shot_bar_def.direction = UI_Bar::DIR::UP;
		charged_shot_bar = app->ui->CreateBar({ viewport.GetRight() - margin.x - 8.f, viewport.GetBottom() - margin.y - 8.f }, charged_shot_bar_def);
		charged_shot_bar->SetPivot(Pivot::X::RIGHT, Pivot::Y::BOTTOM);
	}

	if (tank_num == 0 || tank_num == 1)
	{
		item_helper = app->ui->CreateImage(weapon_frame->position + fPoint(-65.f, 55.f), UI_ImageDef({ app->ui->button_sprites[(int)CONTROLLER_BUTTON::RT] }));
		item_helper->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	}
	else if (tank_num == 2 || tank_num == 3)
	{
		item_helper = app->ui->CreateImage(weapon_frame->position + fPoint(-65.f, -55.f), UI_ImageDef({ app->ui->button_sprites[(int)CONTROLLER_BUTTON::RT] }));
		item_helper->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	}

	if (tank_num == 0 || tank_num == 1)
	{
		weapon_helper = app->ui->CreateImage(item_frame->position + fPoint(28.F, 28.F), UI_ImageDef({ app->ui->button_sprites[(int)CONTROLLER_BUTTON::LB] }));
		weapon_helper->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	}
	else if (tank_num == 2 || tank_num == 3)
	{
		weapon_helper = app->ui->CreateImage(item_frame->position + fPoint(28.F, -28.F), UI_ImageDef({ app->ui->button_sprites[(int)CONTROLLER_BUTTON::LB] }));
		weapon_helper->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
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

void Player_GUI::DamageFlash()
{
	flash->color_mod = { 255, 0 , 0 ,255 };
	flash->SetFX( UI_Fade_FX::FX_TYPE::INTERMITTENT,0.5F, 1);
}

void Player_GUI::HealingFlash()
{
	flash->color_mod = { 0, 255 , 0 ,255 };
	flash->SetFX(UI_Fade_FX::FX_TYPE::INTERMITTENT, 0.8F, 1);
}


void Player_GUI::Update(float dt)
{
	// Update charged shot value ======================================== ???
	// Check if picked weapon and update the GUI if true
	
	for (std::list<UI_Image*>::const_iterator item = this->particles_weapon_frame_list.begin(); item != this->particles_weapon_frame_list.end();)
	{
		fPoint w_offset = { 0, 0 };
		if(this->player->GetTankNum() == 0 || this->player->GetTankNum() == 1)
			w_offset = { 22.0f, -20.0f }; // TODO: Get Weapon rect to avoid these magic numbers
		else if(this->player->GetTankNum() == 2 || this->player->GetTankNum() == 3)
			w_offset = { 22.0f, 20.0f };

		fPoint w_pos = GetWeaponFramePos();
		(*item)->SetPos(lerp((*item)->position, w_pos - w_offset, 3.25f * dt));
		if ((*item)->timer.ReadSec() > 2.0f)
		{
			(*item)->Destroy();
			item = particles_weapon_frame_list.erase(item);
		}
		else
		{
			++item;
		}
	}

	for (std::list<UI_Image*>::const_iterator item = this->particles_item_frame_list.begin(); item != this->particles_item_frame_list.end();)
	{
		fPoint i_offset = { 0.0f, 0.0f }; // TODO: Get Item rect to avoid these magic numbers

		fPoint i_pos = GetItemFramePos();
		(*item)->SetPos(lerp((*item)->position, i_pos - i_offset, 3.25f * dt));
		if ((*item)->timer.ReadSec() > 2.0f)
		{
			(*item)->Destroy();
			item = particles_item_frame_list.erase(item);
		}
		else
		{
			++item;
		}
	}
}

void Player_GUI::SetLifeBar(float life)
{
	life_bar->value = life / (float)player->GetMaxLife();
	life_bar->color_1 = { 255u - (Uint8)(180.f* life_bar->value),  (Uint8)(180.f* life_bar->value) , 0u , 255u };
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
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_DOUBLE_MISSILE];
		break;
	case WEAPON::HEALING_SHOT:
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_HEALING_SHOT];
		break;
	case WEAPON::ELECTRO_SHOT:
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_ELECTRO_SHOT];
		break;
	case WEAPON::FLAMETHROWER:
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_FLAMETHROWER];
		break;
	case WEAPON::LASER_SHOT:
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_LASER];
		break;
	case WEAPON::OIL:
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_OIL_SHOT];
		break;
	case WEAPON::BASIC:
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::WEAPON_BASIC];
		break;
	}
	weapon_icon->SetFX(UI_Fade_FX::FX_TYPE::INTERMITTENT, 1, 3.5F);

}

void Player_GUI::SetItemIcon(ItemType type)
{
	item_icon->SetState(ELEMENT_STATE::VISIBLE);

	switch (type)
	{
	case ItemType::HEALTH_BAG:
		item_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::ITEM_HEALTH_BAG];
		break;
	case ItemType::HAPPY_HOUR_ITEM:
		item_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::ITEM_HAPPY_HOUR];
		break;
	case ItemType::INSTANT_HELP:
		item_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::BIG][(int)ICON_TYPE::ITEM_INSTANT_HELP];
		break;
	case ItemType::NO_TYPE:
		item_icon->SetState(ELEMENT_STATE::HIDDEN);
		break;
	}

	item_icon->SetFX(UI_Fade_FX::FX_TYPE::INTERMITTENT, 1, 3.5F);

}

void Player_GUI::SetArrowColor(const SDL_Color color )
{
	player_arrow->color_mod = color;
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

void Player_GUI::CreateParticleToWeaponFrame()
{
	UI_ImageDef anim_image_def;
	anim_image_def.sprite_section = { 1745, 0, 78, 89 };
	
	UI_Image* particle_image = app->ui->CreateImage(app->map->MapToCamera(this->player->pos_map, this->player->camera_player), anim_image_def);
	particle_image->SetState(ELEMENT_STATE::VISIBLE);
	particle_image->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	particle_image->alpha = 175.0f;
	particle_image->timer.Start();
	this->particles_weapon_frame_list.push_back(particle_image);
}

void Player_GUI::CreateParticleToItemFrame()
{
	UI_ImageDef anim_image_def;
	anim_image_def.sprite_section = { 1745, 0, 78, 89 };

	UI_Image* particle_image = app->ui->CreateImage(app->map->MapToCamera(this->player->pos_map, this->player->camera_player), anim_image_def);
	particle_image->SetState(ELEMENT_STATE::VISIBLE);
	particle_image->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	particle_image->alpha = 175.0f;
	this->particles_item_frame_list.push_back(particle_image);
}

fPoint Player_GUI::GetWeaponFramePos() const
{
	return this->weapon_icon->position;
}

fPoint Player_GUI::GetItemFramePos() const
{
	return this->item_icon->position;
}

Player_GUI::~Player_GUI()
{
}
