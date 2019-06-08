#include "Controllers_Settings.h"
#include "App.h"
#include "M_MainMenu.h"
#include "M_Fonts.h"

#include "UI_Element.h"
#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Label.h"
#include "UI_Table.h"
#include "UI_Image.h"
#include "UI_InteractiveGroup.h"

Controllers_Settings::Controllers_Settings(fPoint relative_pos, uint player)
{
	Canvas = app->ui->CreateElement(fPoint(relative_pos.x + 0, relative_pos.y + 0), UI_ElementDef(), this);;

	panel = app->ui->CreateImage(fPoint(relative_pos.x + 20, relative_pos.y + 20), UI_ImageDef({ 10,1324,1651,881 }));
	panel->SetParent(Canvas);
	SDL_Rect draw_rect = { (int)(relative_pos.x) + 20, (int)(relative_pos.y + 20), 920, 499 };
	panel->SetDrawRect(draw_rect);

	return_button = app->ui->CreateButton(fPoint(relative_pos.x + 89, relative_pos.y + 456), UI_ButtonDef({ 10,1080,60,60 }, { 80,1080,60,60 }, { 150,1080,102 ,102 }, { 260 ,1080,102,102 }), this);
	return_button->SetParent(panel);

	InteractiveGroup = app->ui->CreateIntearctiveGroup(fPoint(relative_pos.x + 0, relative_pos.y + 0), UI_InteractiveGroupDef(2, 2, nullptr), this);
	InteractiveGroup->SetParent(panel);

	
	
	title = app->ui->CreateLabel(fPoint(relative_pos.x + 305, relative_pos.y + 70), UI_LabelDef("Controller Settings", app->font->label_font_38), this);
	title->SetParent(panel);
	UI_TableDef table_def(3,2,2);
	int width = 270, height = 70;
	int widths[2] = { width,width};
	int heights[3] = { height ,height ,height };
	table = app->ui->CreateTable(fPoint(relative_pos.x + 453, relative_pos.y + 240), table_def, widths, heights);
	table->SetParent(panel);
	
	UI_Label* attack_label = app->ui->CreateLabel(fPoint(0, 0), UI_LabelDef("Attack", app->font->label_font_24), nullptr);
	table->AssortElementToTable(attack_label, iPoint(0, 0));
	attack_label->SetParent(table);

	attack_image = app->ui->CreateImage(fPoint(0, 0), UI_ImageDef(app->input->buttons_image[(int)app->input->controllerInfo[player].attack_button]), this);
	attack_image->SetParent(table);
	table->AssortElementToTable(attack_image, iPoint(1, 0));
	
	interaction_image = app->ui->CreateImage(fPoint(0, 0), UI_ImageDef(app->input->buttons_image[(int)app->input->controllerInfo[player].interacton_button]), this);
	interaction_image->SetParent(table);
	table->AssortElementToTable(interaction_image, iPoint(1, 1));

	item_image = app->ui->CreateImage(fPoint(0, 0), UI_ImageDef(app->input->buttons_image[(int)app->input->controllerInfo[player].use_item_button]), this);
	item_image->SetParent(table);
	table->AssortElementToTable(item_image, iPoint(1, 2));

	UI_Label* interaction_label = app->ui->CreateLabel(fPoint(0, 0), UI_LabelDef("Interaction", app->font->label_font_24), nullptr);
	table->AssortElementToTable(interaction_label, iPoint(0, 1));
	interaction_label->SetParent(table);

	UI_Label* Use_item_label = app->ui->CreateLabel(fPoint(0, 0), UI_LabelDef("Use item", app->font->label_font_24), nullptr);
	table->AssortElementToTable(Use_item_label, iPoint(0, 2));
	Use_item_label->SetParent(table);

	UI_Label* Vibration_label = app->ui->CreateLabel(fPoint(relative_pos.x + 275, relative_pos.y + 375), UI_LabelDef("Vibration", app->font->label_font_24), nullptr);
	Vibration_label->SetParent(panel);

	vibration_button_L = app->ui->CreateButton(fPoint(relative_pos.x + 535, relative_pos.y + 390), UI_ButtonDef({ 310,510,33,35 }, { 310, 550,33,35 }, { 310 ,645,46,44 }, { 302 ,592,55,50 }), nullptr);
	vibration_button_L->SetParent(panel);

	vibration_button_R = app->ui->CreateButton(fPoint(relative_pos.x + 625, relative_pos.y + 390), UI_ButtonDef({ 350,510,33,35 }, { 350, 550	,33,35 }, { 365 ,645,53,50 }, { 365 ,590,53,50 }), nullptr);
	vibration_button_R->SetParent(panel);

	UI_Label* Sensitivity_label = app->ui->CreateLabel(fPoint(relative_pos.x + 270, relative_pos.y + 435), UI_LabelDef("Sensitivity", app->font->label_font_24), nullptr);
	Sensitivity_label->SetParent(panel);

	Sensitivity_button_L = app->ui->CreateButton(fPoint(relative_pos.x + 535, relative_pos.y + 450), UI_ButtonDef({ 310,510,33,35 }, { 310, 550,33,35 }, { 310 ,645,46,44 }, { 302 ,592,55,50 }), nullptr);
	Sensitivity_button_L->SetParent(panel);

	Sensitivity_button_R = app->ui->CreateButton(fPoint(relative_pos.x + 625, relative_pos.y + 450), UI_ButtonDef({ 350,510,33,35 }, { 350, 550	,33,35 }, { 365 ,645,53,50 }, { 365 ,590,53,50 }), nullptr);
	Sensitivity_button_R->SetParent(panel);


	InteractiveGroup->SetElement(return_button);
	InteractiveGroup->SetElement(vibration_button_L);
	InteractiveGroup->SetElement(vibration_button_R);

}

Controllers_Settings::~Controllers_Settings()
{}

void Controllers_Settings::ShowControllerSettings()
{
	Canvas->SetStateToBranch(ELEMENT_STATE::VISIBLE);
}

void Controllers_Settings::HideControllersSettings()
{
	Canvas->SetStateToBranch(ELEMENT_STATE::HIDDEN);
}

bool Controllers_Settings::UI_Selected( UI_Element* element)
{

	if (element == return_button)
	{
		app->main_menu->SetState(MENU_STATE::OPTIONS);
	}
	if (element == vibration_button_L)
	{

	}

	return true;

}
