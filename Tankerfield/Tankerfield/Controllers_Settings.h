#ifndef _CONTROLLERS_SETTINGS_H_
#define _CONTROLLERS_SETTINGS_H_
#include "M_UI.h"

class Controllers_Settings :public UI_Listener
{
public:
	Controllers_Settings(fPoint relative_pos, uint player, MENU_TYPE menu_type);

	~Controllers_Settings();

	void ShowControllerSettings();

	void HideControllersSettings();

	bool UI_Selected(UI_Element * element) override;

	bool UI_OnHoverEnter(UI_Element * object) override;

	bool UI_OnHoverExit(UI_Element* element) override;

	bool UI_OnHoverRepeat(UI_Element* element) override;

	UI_InteractiveGroup* InteractiveGroup = nullptr;
private:
	
	UI_Label* attack_label = nullptr;
	UI_Label* interaction_label = nullptr;
	UI_Label* Use_item_label = nullptr;

	MENU_TYPE menu_type = MENU_TYPE::NO_TYPE;


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
	UI_Label* vibration_value_label = nullptr;
	
	UI_Button* Sensitivity_button_L = nullptr;
	UI_Button* Sensitivity_button_R = nullptr;
	UI_Label* sensitivity_value_label = nullptr;

	uint player = 5;

	KeyState change_attack_button = KEY_IDLE;
	KeyState change_interaction_button = KEY_IDLE;
	KeyState change_use_item_button = KEY_IDLE;
	


};

#endif // _CONTROLLERS_SETTINGS_H_

