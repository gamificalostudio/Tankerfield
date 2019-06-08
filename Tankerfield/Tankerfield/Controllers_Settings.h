#ifndef _CONTROLLERS_SETTINGS_H_
#define _CONTROLLERS_SETTINGS_H_
#include "M_UI.h"

class Controllers_Settings :public UI_Listener
{
public:
	Controllers_Settings(fPoint relative_pos, uint player);
	~Controllers_Settings();

	void ShowControllerSettings();

	void HideControllersSettings();

	bool UI_Selected(UI_Element * element);



	UI_InteractiveGroup* InteractiveGroup = nullptr;
private:
	
	UI_Image* attack_image = nullptr;
	UI_Image* interaction_image = nullptr;
	UI_Image* item_image = nullptr;

	UI_Element* Canvas = nullptr;
	
	UI_Image* panel = nullptr;
	UI_Button* return_button = nullptr;
	UI_Label* title = nullptr;
	UI_Table* table = nullptr;
	UI_Button* vibration_button_L = nullptr;
	UI_Button* vibration_button_R = nullptr;
	
	UI_Button* Sensitivity_button_L = nullptr;
	UI_Button* Sensitivity_button_R = nullptr;

	uint Player = 5;



};

#endif // _CONTROLLERS_SETTINGS_H_

