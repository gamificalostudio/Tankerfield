#include "General_HUD.h"
#include "PugiXml/src/pugixml.hpp"
#include "PugiXml/src/pugiconfig.hpp"
#include "Log.h"
#include "App.h"
#include "M_UI.h"
#include "M_Window.h"
#include "M_Fonts.h"

#include "UI_Image.h"
#include "UI_Label.h"
#include "UI_Element.h"
#include "UI_Quad.h"
#include "UI_Table.h"
#include "UI_InputText.h"


General_GUI::General_GUI()
{
	UI_ImageDef image_def;
	fRect screen = app->win->GetWindowRect();
	fPoint screen_center = { screen.w * 0.5f, screen.h * 0.5f };


	// Split screen quads ========================================

	vertical_split_rect = app->ui->CreateQuad(screen_center, UI_QuadDef({ 0 ,0, 6 , (int)screen.h }, { 100, 100, 100, 255 }));
	vertical_split_rect->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	horizontal_split_rect = app->ui->CreateQuad({ screen.w * 0.5f, screen.h * 0.5f }, UI_QuadDef({ 0,0, (int)screen.w , 6 }, { 100, 100, 100, 255 }));
	horizontal_split_rect->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	background = app->ui->CreateQuad({ 0.f, 0.f }, UI_QuadDef({ 0,0, (int)screen.w , (int)screen.h }, { 0, 0, 0 , 200 }));
	background->alpha = 0;

	// Control helper ============================================
	
	control_helper_image = app->ui->CreateImage(screen_center + fPoint(- 40.f, 400.f), UI_ImageDef(app->ui->button_sprites[(int)CONTROLLER_BUTTON::A]));
	control_helper_image->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	control_helper_image->alpha = 0;
	control_helper_image->SetParent(background);

	control_helper_label = app->ui->CreateLabel(screen_center + fPoint(10.f, 400.f), UI_LabelDef("Accept", app->font->label_font_24));
	control_helper_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	control_helper_label->alpha = 0;
	control_helper_label->SetParent(background);

	// Game Over && Win screen ====================================
	
	game_word = app->ui->CreateImage(screen_center + fPoint(10.f, -90.f), UI_ImageDef({ 555,10 ,424,188 }));
	game_word->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	game_word->alpha = 0;
	game_word->SetParent(background);

	over_word = app->ui->CreateImage(screen_center + fPoint(10.f, +90.f), UI_ImageDef({ 555 ,200,383 ,188 }));
	over_word->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	over_word->alpha = 0;
	over_word->SetParent(background);

	you_word = app->ui->CreateImage(screen_center + fPoint(10.f, -90.f), UI_ImageDef({ 990,10 ,314,183 }));
	you_word->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	you_word->alpha = 0;
	you_word->SetParent(background);

	survived_word = app->ui->CreateImage(screen_center + fPoint(10.f, +90.f), UI_ImageDef({ 990,200 ,732,183 }));
	survived_word->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	survived_word->alpha = 0;
	survived_word->SetParent(background);

	UI_LabelDef label_survived_def("text", app->font->label_font_38, { 200, 0 , 0 ,255});
	you_survived = app->ui->CreateLabel({ screen.w * .5f ,  screen.h * .5f + 280.f }, label_survived_def);
	you_survived->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	you_survived->SetParent(background);
	you_survived->alpha = 0;

	// General HUD =================================================

	image_def.sprite_section = { 170 , 10, 105, 105 };
	round_element = app->ui->CreateImage({ screen.w * .5f ,  screen.h * .5f }, image_def);
	round_element->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	UI_LabelDef label_round_def("1", app->font->rounds_font);
	round_number_label = app->ui->CreateLabel({ screen.w * .5f ,  screen.h * .5f }, label_round_def);
	round_number_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	round_number_label->SetParent(round_element);

	image_def.sprite_section = { 120, 515, 179, 179 };
	round_fx = app->ui->CreateImage({ screen.w * .5f ,  screen.h * .5f }, image_def);
	round_fx->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	round_fx->alpha = 0.f;

	image_def.sprite_section = { 10, 160, 50, 530 };
	left_tank_life = app->ui->CreateImage({ 0.f ,  screen.h * .5f }, image_def);
	left_tank_life->SetPivot(Pivot::X::LEFT, Pivot::Y::CENTER);

	image_def.sprite_section = { 60, 160, 50, 530 };
	right_tank_life = app->ui->CreateImage({ screen.w  ,  screen.h * .5f }, image_def);
	right_tank_life->SetPivot(Pivot::X::RIGHT, Pivot::Y::CENTER);

	FadeGeneralHUD(true);
}

General_GUI::~General_GUI()
{
}

void General_GUI::FadeGeneralHUD(bool fade_on)
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

	round_fx->alpha = 0;
	round_fx->FinishFX();

	round_number_label	->SetFX(type, 2.F);
	round_element		->SetFX(type, 2.F);
	left_tank_life		->SetFX(type, 2.F);
	right_tank_life		->SetFX(type, 2.F);
}

void General_GUI::SetRoundNumber(int round)
{
	round_number_label->SetText(std::to_string(round).c_str());
}

void General_GUI::RoundFX()
{
	round_fx->SetFX(UI_Fade_FX::FX_TYPE::INTERMITTENT, 0.8F, 4.F);
}

bool General_GUI::GetIsVisible()
{
	return visible;
}


void General_GUI::FadeGameOverScreen(bool fade_on, int rounds_survived)
{
	UI_Fade_FX::FX_TYPE type;

	if (fade_on)
	{
		type = UI_Fade_FX::FX_TYPE::FADE_ON;
		background->SetFX(type, 2.F);
		control_helper_image->SetFX(type, 2.F);
		control_helper_label->SetFX(type, 2.F);
	}
	else
	{
		type = UI_Fade_FX::FX_TYPE::FADE_OUT;
	}
	
	String round_str;

	if (rounds_survived != -1)
	{
		if (rounds_survived > 1)
		{
			round_str = "YOU SURVIVED " + to_string(rounds_survived) + " ROUNDS";
		}
		else
		{
			round_str = "YOU SURVIVED " + to_string(1) + " ROUND";
		}

		you_survived->SetText(round_str);
	}

	you_survived->SetFX(type, 2.F);
	game_word->SetFX(type, 2.F);
	over_word->SetFX(type, 2.F); 
}

void General_GUI::ShowGeneralGUI()
{
	visible = true;
	vertical_split_rect		->SetState(ELEMENT_STATE::VISIBLE);
	horizontal_split_rect	->SetState(ELEMENT_STATE::VISIBLE);
	round_fx				->SetState(ELEMENT_STATE::VISIBLE);
	round_number_label		->SetState(ELEMENT_STATE::VISIBLE);
	round_element			->SetState(ELEMENT_STATE::VISIBLE);
	left_tank_life			->SetState(ELEMENT_STATE::VISIBLE);
	right_tank_life			->SetState(ELEMENT_STATE::VISIBLE);
}

void General_GUI::HideGeneralGUI()
{
	visible = false;
	vertical_split_rect		->SetState(ELEMENT_STATE::HIDDEN);
	horizontal_split_rect	->SetState(ELEMENT_STATE::HIDDEN);
	round_fx				->SetState(ELEMENT_STATE::HIDDEN);
	round_number_label		->SetState(ELEMENT_STATE::HIDDEN);
	round_element			->SetState(ELEMENT_STATE::HIDDEN);
	left_tank_life			->SetState(ELEMENT_STATE::HIDDEN);
	right_tank_life			->SetState(ELEMENT_STATE::HIDDEN);
}
