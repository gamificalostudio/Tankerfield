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
	wave_element = app->ui->CreateImage({ full_screen.w * .5f ,  full_screen.h * .5f }, image_def);
	wave_element->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);

	UI_LabelDef label_def("1", app->ui->rounds_font);
	wave_number_label = app->ui->CreateLabel({ full_screen.w * .5f ,  full_screen.h * .5f }, label_def);
	wave_number_label->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	wave_number_label->SetParent(wave_element);

	image_def.sprite_section = { 120, 515, 179, 179 };
	wave_fx = app->ui->CreateImage({ full_screen.w * .5f ,  full_screen.h * .5f }, image_def);
	wave_fx->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	wave_fx->alpha = 0;

	image_def.sprite_section = { 10, 160, 50, 530 };
	left_tank_life = app->ui->CreateImage({ 0.f ,  full_screen.h * .5f }, image_def);
	left_tank_life->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::CENTER);

	image_def.sprite_section = { 60, 160, 50, 530 };
	right_tank_life = app->ui->CreateImage({ full_screen.w ,  full_screen.h * .5f }, image_def);
	right_tank_life->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::CENTER);
}

void General_HUD::FadeGeneralHUD(bool fade_on)
{
	float init_value = 0.f, target_value = 0.f;

	if (fade_on)
	{
		init_value = 0.f;
		target_value = 255.f;
	}
	else
	{
		init_value = 255.f;
		target_value = 0.f;
	}

	wave_number_label->SetFX(UI_Fade_FX::FX_TYPE::FADE, 2.F, init_value, target_value);
	wave_element->SetFX(UI_Fade_FX::FX_TYPE::FADE, 2.F, init_value, target_value);
	left_tank_life->SetFX(UI_Fade_FX::FX_TYPE::FADE, 2.F, init_value, target_value);
	right_tank_life->SetFX(UI_Fade_FX::FX_TYPE::FADE, 2.F, init_value, target_value);
}

void General_HUD::SetWaveNumber(int round)
{
	wave_number_label->SetText(std::to_string(1).c_str());
}

void General_HUD::FadeGameOver(bool fade_on)
{
	float init_value = 0.f, target_value = 0.f;

	if (fade_on)
	{
		init_value = 0.f;
		target_value = 255.f;
	}
	else
	{
		init_value = 255.f;
		target_value = 0.f;
	}

	game_word->SetFX(UI_Fade_FX::FX_TYPE::FADE, 2.F, init_value, target_value);
	over_word->SetFX(UI_Fade_FX::FX_TYPE::FADE, 2.F, init_value, target_value);
}