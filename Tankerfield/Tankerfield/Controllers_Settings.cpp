#include "Controllers_Settings.h"
#include "App.h"
#include "M_MainMenu.h"
#include "M_Fonts.h"

#include "UI_Element.h"
#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Label.h"
#include "UI_InteractiveGroup.h"

Controllers_Settings::Controllers_Settings(fPoint relative_pos)
{
	Canvas = app->ui->CreateElement(fPoint(relative_pos.x + 0, relative_pos.y + 0), UI_ElementDef(), this);;

	panel = app->ui->CreateImage(fPoint(relative_pos.x + 20, relative_pos.y + 20), UI_ImageDef({ 10,1324,1651,881 }));
	panel->SetParent(Canvas);

	return_button = app->ui->CreateButton(fPoint(relative_pos.x + 59, relative_pos.y + 426), UI_ButtonDef({ 10,1080,60,60 }, { 80,1080,60,60 }, { 150,1080,102 ,102 }, { 260 ,1080,102,102 }), this);
	return_button->SetParent(panel);

	InteractiveGroup = app->ui->CreateIntearctiveGroup(fPoint(relative_pos.x + 0, relative_pos.y + 0), UI_InteractiveGroupDef(1, 1, nullptr), this);
	InteractiveGroup->SetParent(panel);

	InteractiveGroup->SetElement(return_button, iPoint(0, 0));
	
	
	title = app->ui->CreateLabel(fPoint(relative_pos.x + 295, relative_pos.y + 76), UI_LabelDef("Controller Settings", app->font->label_font_24), this);
	title->SetParent(panel);

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

void Controllers_Settings::InputSelect()
{
	UI_Element* focused = InteractiveGroup->GetFocusedElement();
	if (focused == return_button)
	{
		HideControllersSettings();
		app->main_menu->SetState(MENU_STATE::OPTIONS);
	}




}
