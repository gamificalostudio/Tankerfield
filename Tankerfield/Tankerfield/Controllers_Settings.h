#ifndef _CONTROLLERS_SETTINGS_H_
#define _CONTROLLERS_SETTINGS_H_
#include "M_UI.h"

class Controllers_Settings :public UI_Listener
{
public:
	Controllers_Settings();
	~Controllers_Settings();

	void ShowControllerSettings();

	void HideControllersSettings();

	void InputSelect();

private:
	UI_Element* Canvas = nullptr;
	UI_Image* panel = nullptr;
	UI_Button* return_button = nullptr;
	UI_InteractiveGroup* InteractiveGroup = nullptr;


};

#endif // _CONTROLLERS_SETTINGS_H_

