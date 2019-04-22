#ifndef __GENERAL_HUD_H__
#define __GENERAL_HUD_H__

class UI_Image;
class UI_Bar;
class UI_Button;
class UI_Label;
class UI_Quad;

class General_HUD
{
public:

	General_HUD();

	~General_HUD();

	void FadeGeneralHUD(bool fade_on);

	void SetRoundNumber(int round);

	void FadeGameOver(bool fade_on);


private:

	// HUD General -------------------------------------------

	UI_Label* round_number_label = nullptr;

	UI_Image* round_element = nullptr;

	UI_Image* round_fx = nullptr;

	UI_Image* left_tank_life = nullptr;

	UI_Image* right_tank_life = nullptr;

	UI_Image* game_word = nullptr;

	UI_Image* over_word = nullptr;

	UI_Quad*  background = nullptr;

};

#endif // __MODULE_UI_H__