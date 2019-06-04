#include "Controllers_Settings.h"
#include "App.h"
#include "M_MainMenu.h"

#include "UI_Element.h"
#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_InteractiveGroup.h"

Controllers_Settings::Controllers_Settings()
{
	Canvas = app->ui->CreateElement(fPoint(0, 0), UI_ElementDef(), this);

	panel = app->ui->CreateImage(fPoint(20, 20), UI_ImageDef({ 10,1324,1651,881 }));
	panel->SetParent(Canvas);

	return_button = app->ui->CreateButton(fPoint(59, 426), UI_ButtonDef({ 10,1080,60,60 }, { 80,1080,60,60 }, { 150,1080,102 ,102 }, { 260 ,1080,102,102 }), this);
	return_button->SetParent(panel);

	InteractiveGroup = app->ui->CreateIntearctiveGroup(fPoint(0, 0), UI_InteractiveGroupDef(1, 1, nullptr), this);
	InteractiveGroup->SetParent(panel);

	InteractiveGroup->SetElement(return_button, iPoint(0, 0));
	
	//app->ui->CreateLabel()
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
