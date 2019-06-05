#include "UI_InGameElement.h"
#include "App.h"
#include "M_UI.h"
#include "M_Map.h"
#include "M_Fonts.h"
#include "Player_GUI.h"

#include "Obj_Tank.h"
#include "Obj_PickUp.h"
#include "Object.h"

#include "UI_Image.h"
#include "UI_Label.h"
#include "UI_Quad.h"

#include "Point.h"

UI_InGameElement::UI_InGameElement(const fPoint position, const UI_InGameElementDef definition) : UI_Element(position, definition, nullptr), pointed_obj(definition.pointed_obj), is_arrow_actived(definition.is_arrow_actived)
{
	// Set Arrow =================================================

	if (definition.is_arrow_actived == true)
	{
		arrow_animation = &app->ui->arrow_anim;
		color_mod = definition.arrow_color;
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

	fPoint obj_pos = app->map->MapToCamera(pointed_obj->pos_map, app->ui->current_camera);
	fPoint player_pos = app->map->MapToCamera(app->ui->current_gui->player->pos_map, app->ui->current_camera);

	SDL_Point sdl_point = { (int)obj_pos.x,  (int)obj_pos.y };

	if (SDL_PointInRect( &sdl_point, &(SDL_Rect)app->ui->current_gui->viewport_with_margin))
	{
		return true;
	}

	fPoint vector(obj_pos - player_pos);
	float angle = atan2(vector.y, vector.x) * RADTODEG; 
	fRect viewport = app->ui->current_gui->viewport_with_margin;
	fPoint points[4] = { fPoint(viewport.GetLeft() , viewport.GetTop()) , fPoint(viewport.GetRight() , viewport.GetTop()), fPoint(viewport.GetRight() , viewport.GetBottom()), fPoint(viewport.GetLeft() , viewport.GetBottom()) };

	fPoint final_point = { 0.f,0.f };

	for (int i = 0; i < 4 ; ++i)
	{
		int j = 0;  

		if (i == 3)
			j = 0;
		else
			j = i + 1;

		if (get_line_intersection(obj_pos , player_pos , points[i] , points[j], &final_point) == true)
		{
			break;
		}
	}

	SDL_Rect src = { 120, 160, 45, 45 };
	SDL_Rect dst = { (int)final_point.x - 45, (int)final_point.y - 45 * 0.5f, 45, 45 };
	SDL_Point rect_point = { 45, 45 / 2 };

	SDL_SetTextureColorMod(app->ui->GetAtlas(), color_mod.r, color_mod.g, color_mod.b);
	SDL_RenderCopyEx(app->render->renderer, app->ui->GetAtlas(), &src, &dst, angle, &rect_point, SDL_FLIP_NONE);
	SDL_SetTextureColorMod(app->ui->GetAtlas(), 0, 0, 0);

}

UI_IG_Weapon::UI_IG_Weapon(const fPoint position, const UI_InGameElementDef definition) : UI_InGameElement(position, definition)
{
	UI_ImageDef img_def;

	// Add frame ====================================================

	img_def.sprite_section = { 330, 160, 50, 70 };
	img_def.is_in_game = true;

	weapon_frame = app->ui->CreateImage(position, img_def);
	weapon_frame->SetPivot(Pivot::X::CENTER, Pivot::Y::BOTTOM);
	weapon_frame->SetParent(this);

	// Add icon ====================================================

	weapon_icon = app->ui->CreateImage(position + app->map->ScreenToMapF(0.f, -29.f), img_def);
	weapon_icon->SetPivot(Pivot::X::CENTER, Pivot::Y::BOTTOM);
	weapon_icon->SetParent(weapon_frame);

	Obj_PickUp* pick_up_obj = (Obj_PickUp*)pointed_obj;
	
	switch (pick_up_obj->type_of_weapon)
	{
	case WEAPON::DOUBLE_MISSILE:
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_DOUBLE_MISSILE];
		break;
	case WEAPON::HEALING_SHOT:
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_HEALING_SHOT];
		break;
	case WEAPON::ELECTRO_SHOT:
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_ELECTRO_SHOT];
		break;
	case WEAPON::FLAMETHROWER:
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_FLAMETHROWER];
		break;
	case WEAPON::LASER_SHOT:
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_LASER];
		break;
	case WEAPON::OIL:
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_OIL_SHOT];
		break;
	case WEAPON::BASIC:
		weapon_icon->sprite_rect = app->ui->icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::WEAPON_BASIC];
		break;
	}

	level_indicator = app->ui->CreateImage(position + app->map->ScreenToMapF(-32.f,- 64.f), img_def);
	level_indicator->SetParent(weapon_frame);
	UpdateLevel();


	weapon_frame	->SetFX(UI_Fade_FX::FX_TYPE::FADE_ON, 2.F);
	weapon_icon		->SetFX(UI_Fade_FX::FX_TYPE::FADE_ON, 2.F);
	level_indicator	->SetFX(UI_Fade_FX::FX_TYPE::FADE_ON, 2.F);
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
		level_indicator->sprite_rect = { 460, 160, 15, 10 * abs(diference)};
	}
	else
	{
		level_indicator->sprite_rect = { 460, 205, 15, 10 * abs(diference) };
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
	item_frame->SetPivot(Pivot::X::CENTER, Pivot::Y::BOTTOM);
	item_frame->SetParent(this);

	// Add icon ====================================================
	Obj_PickUp* pick_up_obj = (Obj_PickUp*)pointed_obj;

	switch (pick_up_obj->type_of_item)
	{
	case ItemType::HEALTH_BAG:
		img_def.sprite_section = app->ui->icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::ITEM_HEALTH_BAG];
		break;
	case ItemType::HAPPY_HOUR_ITEM:
		img_def.sprite_section = app->ui->icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::ITEM_HAPPY_HOUR];
		break;
	case ItemType::INSTANT_HELP:
		img_def.sprite_section = app->ui->icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::ITEM_INSTANT_HELP];
		break;
	default:
		img_def.sprite_section = app->ui->icon_sprites[(int)ICON_SIZE::SMALL][(int)ICON_TYPE::ITEM_HEALTH_BAG];
		break;
	}

	item_icon = app->ui->CreateImage( position + app->map->ScreenToMapF(0.f, - 42.f), img_def);
	item_icon->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	item_icon->SetParent(item_frame);
	
	item_icon->SetFX(UI_Fade_FX::FX_TYPE::FADE_ON, 2.F);
	item_frame->SetFX(UI_Fade_FX::FX_TYPE::FADE_ON, 2.F);
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

void UI_IG_Helper::AddButtonHelper( const CONTROLLER_BUTTON button_type, const fPoint offset)
{
	UI_ImageDef def(app->ui->button_sprites[(int)button_type]);
	def.is_in_game = true;

	UI_Image*  ui_helper = app->ui->CreateImage(position + app->map->ScreenToMapF(offset.x, offset.y), def);
	ui_helper->single_camera = this->single_camera;
	ui_helper->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	ui_helper->SetParent(this);
	helper_elements.push_back(ui_helper);
}

void UI_IG_Helper::AddTextHelper(const String text, const fPoint offset)
{
	UI_LabelDef def(text, app->font->label_font_24);
	def.is_in_game = true;

	UI_Label* ui_helper = app->ui->CreateLabel(position + app->map->ScreenToMapF(offset.x, offset.y), def);
	ui_helper->single_camera = this->single_camera;
	ui_helper->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	
	UI_QuadDef quad_def({ 0,0, ui_helper->sprite_rect.w, ui_helper->sprite_rect.h }, { 100, 100, 100, 100 });
	quad_def.is_in_game = true;

	UI_Quad* back_quad = app->ui->CreateQuad(position + app->map->ScreenToMapF(offset.x, offset.y), quad_def);
	back_quad->single_camera = this->single_camera;
	back_quad->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	back_quad->SetParent(this);
	ui_helper->SetParent(back_quad);

	helper_elements.push_back(back_quad);
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

//fPoint final_arrow_point;
//fRect viewport = app->ui->current_gui->viewport;
//fPoint vector = pointed_obj_pos - player_pos;
//float angle = atan2(vector.y, vector.x) * RADTODEG;
//
//float distance_to_border_y = 0.f;

//if (pointed_obj_pos.y > viewport.GetBottom())
//{
//	distance_to_border_y = viewport.GetBottom() - player_pos.y;
//	final_arrow_point.x = player_pos.x + (distance_to_border_y *  vector.x) / vector.y;
//	final_arrow_point.y = viewport.GetBottom();
//}
//else if (pointed_obj_pos.y < viewport.GetTop())
//{
//	distance_to_border_y = viewport.GetTop() - player_pos.y;
//	final_arrow_point.x = player_pos.x + (distance_to_border_y *  vector.x) / vector.y;
//	final_arrow_point.y = viewport.GetTop();
//}
//else if 
//{
//	final_arrow_point = 
//}


//pointed_obj_pos = app->map->MapToCamera(app->ui->current_gui->player->pos_map - vector * 2.f, app->ui->current_camera);

//SDL_SetTextureColorMod(app->ui->GetAtlas(), color_mod.r, color_mod.g, color_mod.b);
//app->render->BlitUI( app->ui->GetAtlas(), pointed_obj_pos.x - (float)sprite_rect.w * 0.5f, pointed_obj_pos.y - (float)sprite_rect.h * 0.5f, &sprite_rect ,app->ui->current_camera);
//SDL_SetTextureColorMod(app->ui->GetAtlas(), 255, 255, 255);