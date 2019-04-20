#include "General_HUD.h"
#include "App.h"
#include "M_UI.h"
#include "UI_Image.h"
#include "UI_Label.h"
#include "UI_Element.h"
#include "M_Window.h"

General_HUD::General_HUD()
{
	UI_ImageDef image_def;
	fRect full_screen = app->win->GetWindowRect();

	game_word = app->ui->CreateImage(fPoint(750.f, 335.f), UI_ImageDef({ 555,10 ,424,188 }));
	over_word = app->ui->CreateImage(fPoint(770.f, 521.f), UI_ImageDef({ 555 ,200,383 ,188 }));
	game_word->SetState(ELEMENT_STATE::HIDDEN);
	over_word->SetState(ELEMENT_STATE::HIDDEN);

	image_def.sprite_section = { 170 , 10, 105, 105 };
	round_element = app->ui->CreateImage({ full_screen.w * .5f ,  full_screen.h * .5f }, image_def);
	round_element->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);

	UI_LabelDef label_def("1", app->ui->rounds_font);
	round_number_label = app->ui->CreateLabel({ full_screen.w * .5f ,  full_screen.h * .5f }, label_def);
	round_number_label->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	round_number_label->SetParent(round_element);

	image_def.sprite_section = { 120, 515, 179, 179 };
	round_fx = app->ui->CreateImage({ full_screen.w * .5f ,  full_screen.h * .5f }, image_def);
	round_fx->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	round_fx->alpha = 0.F;

	image_def.sprite_section = { 10, 160, 50, 530 };
	left_tank_life = app->ui->CreateImage({ 0.f ,  full_screen.h * .5f }, image_def);
	left_tank_life->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::CENTER);

	image_def.sprite_section = { 60, 160, 50, 530 };
	right_tank_life = app->ui->CreateImage({ full_screen.w ,  full_screen.h * .5f }, image_def);
	right_tank_life->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::CENTER);

	FadeGeneralHUD(true);
}

void General_HUD::FadeGeneralHUD(bool fade_on)
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

	round_number_label	->SetFX(type, 3.5F);
	round_element		->SetFX(type, 3.5F);
	left_tank_life		->SetFX(type, 3.5F);
	right_tank_life		->SetFX(type, 3.5F);
}

void General_HUD::SetRoundNumber(int round)
{
	round_number_label->SetText(std::to_string(round).c_str());
	round_fx->SetFX(UI_Fade_FX::FX_TYPE::INTERMITTENT, 1.F, 3.F);
}

void General_HUD::FadeGameOver(bool fade_on)
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

	game_word->SetFX(type, 2.F);
	over_word->SetFX(type, 2.F);
}