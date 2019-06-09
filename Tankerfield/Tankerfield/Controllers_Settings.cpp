#include "Controllers_Settings.h"
#include "Log.h"
#include "App.h"
#include "M_MainMenu.h"
#include "M_Fonts.h"
#include "M_Scene.h"

#include "UI_Element.h"
#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Label.h"
#include "UI_Table.h"
#include "UI_Image.h"
#include "UI_InteractiveGroup.h"

Controllers_Settings::Controllers_Settings(fPoint relative_pos, uint player, MENU_TYPE menu_type) : menu_type(menu_type)
{
	this->player = player;

	Canvas = app->ui->CreateElement(fPoint(relative_pos.x + 0, relative_pos.y + 0), UI_ElementDef(), this);;

	panel = app->ui->CreateImage(fPoint(relative_pos.x + 20, relative_pos.y + 20), UI_ImageDef({ 10,1324,1651,881 }));
	panel->SetParent(Canvas);
	SDL_Rect draw_rect = { (int)(relative_pos.x) + 20, (int)(relative_pos.y + 20), 920, 499 };
	panel->SetDrawRect(draw_rect);

	return_button = app->ui->CreateButton(fPoint(relative_pos.x + 89, relative_pos.y + 456), UI_ButtonDef({ 10,1080,60,60 }, { 80,1080,60,60 }, { 150,1080,102 ,102 }, { 260 ,1080,102,102 }), this);
	return_button->SetParent(panel);

	title = app->ui->CreateLabel(fPoint(relative_pos.x + 305, relative_pos.y + 70), UI_LabelDef("Controller Settings", app->font->label_font_38), this);
	title->SetParent(panel);
	UI_TableDef table_def(3,2,2);
	int width = 270, height = 70;
	int widths[2] = { width,width};
	int heights[3] = { height ,height ,height };
	table = app->ui->CreateTable(fPoint(relative_pos.x + 453, relative_pos.y + 240), table_def, widths, heights);
	table->SetParent(panel);
	
	attack_label = app->ui->CreateLabel(fPoint(0, 0), UI_LabelDef("Attack", app->font->label_font_24), this);
	table->AssortElementToTable(attack_label, iPoint(0, 0));
	attack_label->SetParent(table);

	interaction_label = app->ui->CreateLabel(fPoint(0, 0), UI_LabelDef("Interaction", app->font->label_font_24), this);
	table->AssortElementToTable(interaction_label, iPoint(0, 1));
	interaction_label->SetParent(table);

	attack_image = app->ui->CreateImage(fPoint(0, 0), UI_ImageDef(app->ui->button_sprites[(int)app->input->controllerInfo[player].attack_button]), nullptr);
	attack_image->SetParent(table);
	table->AssortElementToTable(attack_image, iPoint(1, 0));
	
	interaction_image = app->ui->CreateImage(fPoint(0, 0), UI_ImageDef(app->ui->button_sprites[(int)app->input->controllerInfo[player].interacton_button]), nullptr);
	interaction_image->SetParent(table);
	table->AssortElementToTable(interaction_image, iPoint(1, 1));

	item_image = app->ui->CreateImage(fPoint(0, 0), UI_ImageDef(app->ui->button_sprites[(int)app->input->controllerInfo[player].use_item_button]), nullptr);
	item_image->SetParent(table);
	table->AssortElementToTable(item_image, iPoint(1, 2));

	Use_item_label = app->ui->CreateLabel(fPoint(0, 0), UI_LabelDef("Use item", app->font->label_font_24), this);
	table->AssortElementToTable(Use_item_label, iPoint(0, 2));
	Use_item_label->SetParent(table);

	UI_Label* Vibration_label = app->ui->CreateLabel(fPoint(relative_pos.x + 275, relative_pos.y + 375), UI_LabelDef("Vibration", app->font->label_font_24), nullptr);
	Vibration_label->SetParent(panel);

	vibration_button_L = app->ui->CreateButton(fPoint(relative_pos.x + 535, relative_pos.y + 390), UI_ButtonDef({ 310,510,33,35 }, { 310, 550,33,35 }, { 310 ,645,46,44 }, { 302 ,592,55,50 }), nullptr);
	vibration_button_L->SetParent(panel);

	vibration_button_R = app->ui->CreateButton(fPoint(relative_pos.x + 625, relative_pos.y + 390), UI_ButtonDef({ 350,510,33,35 }, { 350, 550	,33,35 }, { 365 ,645,53,50 }, { 365 ,590,53,50 }), nullptr);
	vibration_button_R->SetParent(panel);

	vibration_value_label = app->ui->CreateLabel(fPoint(relative_pos.x + 580, relative_pos.y + 390), UI_LabelDef(std::to_string((int)(app->input->controllerInfo[player].vibration_percentage*100)),app->font->label_font_24));
	vibration_value_label->SetParent(panel);
	vibration_value_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	
	UI_Label* Sensitivity_label = app->ui->CreateLabel(fPoint(relative_pos.x + 270, relative_pos.y + 435), UI_LabelDef("Sensitivity", app->font->label_font_24), nullptr);
	Sensitivity_label->SetParent(panel);

	Sensitivity_button_L = app->ui->CreateButton(fPoint(relative_pos.x + 535, relative_pos.y + 450), UI_ButtonDef({ 310,510,33,35 }, { 310, 550,33,35 }, { 310 ,645,46,44 }, { 302 ,592,55,50 }), nullptr);
	Sensitivity_button_L->SetParent(panel);
	

	Sensitivity_button_R = app->ui->CreateButton(fPoint(relative_pos.x + 625, relative_pos.y + 450), UI_ButtonDef({ 350,510,33,35 }, { 350, 550	,33,35 }, { 365 ,645,53,50 }, { 365 ,590,53,50 }), nullptr);
	Sensitivity_button_R->SetParent(panel);

	sensitivity_value_label = app->ui->CreateLabel(fPoint(relative_pos.x + 580, relative_pos.y + 450), UI_LabelDef(std::to_string((int)(app->input->controllerInfo[player].death_zone_porcenatage*100)), app->font->label_font_24));
	sensitivity_value_label->SetParent(panel);
	sensitivity_value_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	InteractiveGroup = app->ui->CreateIntearctiveGroup(fPoint(relative_pos.x + 0, relative_pos.y + 0), UI_InteractiveGroupDef(player, nullptr), this); //  TODO Set Controller of player 
	InteractiveGroup->SetParent(panel);
	InteractiveGroup->SetControllers(player);

	InteractiveGroup->AddElement(attack_label);
	InteractiveGroup->AddElement(interaction_label);
	InteractiveGroup->AddElement(Use_item_label);
	InteractiveGroup->AddElement(vibration_button_L);
	InteractiveGroup->AddElement(vibration_button_R);
	InteractiveGroup->AddElement(Sensitivity_button_L);
	InteractiveGroup->AddElement(Sensitivity_button_R);
	InteractiveGroup->AddElement(return_button);
}

Controllers_Settings::~Controllers_Settings()
{}

void Controllers_Settings::ShowControllerSettings()
{
	InteractiveGroup->Active();
	Canvas->SetStateToBranch(ELEMENT_STATE::VISIBLE);
}

void Controllers_Settings::HideControllersSettings()
{
	InteractiveGroup->Desactive();
	Canvas->SetStateToBranch(ELEMENT_STATE::HIDDEN);
}

bool Controllers_Settings::UI_Selected( UI_Element* element)
{

	if (element == return_button)
	{
		if (menu_type == MENU_TYPE::MAIN_MENU)
		{
			app->main_menu->SetMenuState(MENU_STATE::OPTIONS);
		}
		else if (menu_type == MENU_TYPE::PAUSE_MENU)
		{
			app->scene->SetMenuState(MENU_STATE::OPTIONS);
		}
		
	}
	else if (element == vibration_button_L)

	{
		app->input->controllerInfo[player].vibration_percentage -= 0.100000000f;
		if (app->input->controllerInfo[player].vibration_percentage < 0.f)
		{
			app->input->controllerInfo[player].vibration_percentage = 0.f;
		}
		vibration_value_label->SetText(std::to_string((int)(app->input->controllerInfo[player].vibration_percentage*100)));
	}
	else if (element == vibration_button_R)
	{
		app->input->controllerInfo[player].vibration_percentage += 0.10f;
		if (app->input->controllerInfo[player].vibration_percentage > 1.f)
		{
			app->input->controllerInfo[player].vibration_percentage = 1.f;
		}
		vibration_value_label->SetText(std::to_string((int)(app->input->controllerInfo[player].vibration_percentage * 100)));
	}
	else if (element == Sensitivity_button_L)
	{
		app->input->controllerInfo[player].death_zone_porcenatage -= 0.100000000f;
		if (app->input->controllerInfo[player].death_zone_porcenatage < 0.f)
		{
			app->input->controllerInfo[player].death_zone_porcenatage = 0.f;
		}
		sensitivity_value_label->SetText(std::to_string((int)(app->input->controllerInfo[player].death_zone_porcenatage * 100)));
	}
	else if (element == Sensitivity_button_R)
	{
		app->input->controllerInfo[player].death_zone_porcenatage += 0.100000000f;
		if (app->input->controllerInfo[player].death_zone_porcenatage > 1.f)
		{
			app->input->controllerInfo[player].death_zone_porcenatage = 1.f;
		}
		sensitivity_value_label->SetText(std::to_string((int)(app->input->controllerInfo[player].death_zone_porcenatage * 100)));
	}
	else if (element == attack_label)
	{
		if (change_attack_button == KEY_IDLE)
		{
			attack_image->SetStateToBranch(ELEMENT_STATE::HIDDEN);
			change_attack_button = KEY_DOWN;
		}
		
	}
	else if (element == interaction_label)
	{
		if (change_interaction_button == KEY_IDLE)
		{
			interaction_image->SetStateToBranch(ELEMENT_STATE::HIDDEN);
			change_interaction_button = KEY_DOWN;
		}
		
	}
	else if (element == Use_item_label)
	{
		if (change_use_item_button == KEY_IDLE)
		{
			item_image->SetStateToBranch(ELEMENT_STATE::HIDDEN);
			change_use_item_button = KEY_DOWN;
		}
		
	}
	return true;
}

bool Controllers_Settings::UI_OnHoverEnter(UI_Element* element)
{
	if (element == (UI_Element*)attack_label)
	{
		attack_label->color_mod = { 200,0,0,255 };
	}
	if (element == (UI_Element*)interaction_label)
	{
		interaction_label->color_mod = { 200,0,0,255 };
	}
	if (element == (UI_Element*)Use_item_label)
	{
		Use_item_label->color_mod = { 200,0,0,255 };
	}
	return true;
};

bool Controllers_Settings::UI_OnHoverExit(UI_Element* element)
{
	if (element == (UI_Element*)attack_label)
	{
		attack_label->color_mod = { 255,255,255,255 };
		attack_image->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		change_attack_button = KEY_IDLE;
	}
	if (element == (UI_Element*)interaction_label)
	{
		interaction_label->color_mod = { 255,255,255,255 };
		interaction_image->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		change_interaction_button = KEY_IDLE;
	}
	if (element == (UI_Element*)Use_item_label)
	{
		Use_item_label->color_mod = { 255,255,255,255 };
		item_image->SetStateToBranch(ELEMENT_STATE::VISIBLE);
		change_use_item_button = KEY_IDLE;
	}
	return true;
}

bool Controllers_Settings::UI_OnHoverRepeat(UI_Element * element)
{
	if (element == (UI_Element*)attack_label)
	{
		if (change_attack_button == KEY_DOWN)
		{
			change_attack_button = KEY_REPEAT;
		}
		else if (change_attack_button == KEY_REPEAT)
		{
			if (app->input->GetControllerButtonDown(player, app->input->controllerInfo[player].attack_button))
			{
				attack_image->sprite_rect = (app->ui->button_sprites[(int)app->input->controllerInfo[player].attack_button]);
				attack_image->SetStateToBranch(ELEMENT_STATE::VISIBLE);
				change_attack_button = KEY_UP;
			}
		
		}
		else if (change_attack_button ==KEY_UP)
		{
			change_attack_button = KEY_IDLE;
		}
		
	}
	if (element == (UI_Element*)interaction_label)
	{
		if (change_interaction_button == KEY_DOWN)
		{
			change_interaction_button = KEY_REPEAT;
		}
		else if (change_interaction_button == KEY_REPEAT)
		{
			if (app->input->GetControllerButtonDown(player, app->input->controllerInfo[player].interacton_button))
			{
				interaction_image->sprite_rect = app->ui->button_sprites[(int)app->input->controllerInfo[player].interacton_button];
				interaction_image->SetStateToBranch(ELEMENT_STATE::VISIBLE);
				change_interaction_button = KEY_UP;
			}
			
		}
		else if (change_interaction_button == KEY_UP)
		{
			change_interaction_button = KEY_IDLE;
		}
	}
	if (element == (UI_Element*)Use_item_label)
	{
		if (change_use_item_button == KEY_DOWN)
		{
			change_use_item_button = KEY_REPEAT;
		}
		else if (change_use_item_button == KEY_REPEAT)
		{
			if (app->input->GetControllerButtonDown(player, app->input->controllerInfo[player].use_item_button))
			{
				item_image->sprite_rect = (app->ui->button_sprites[(int)app->input->controllerInfo[player].use_item_button]);
				item_image->SetStateToBranch(ELEMENT_STATE::VISIBLE);
				change_use_item_button = KEY_UP;
			}
			
		}
		else if (change_use_item_button == KEY_UP)
		{
			change_use_item_button = KEY_IDLE;
		}
	}
	return true;
}
