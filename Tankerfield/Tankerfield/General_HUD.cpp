#include "General_HUD.h"
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
	// Split screen quads ==================================

	vertical_split_rect = app->ui->CreateQuad( screen_center, UI_QuadDef({ 0 ,0, 6 , (int)screen.h }, { 150, 150, 150, 255 }));
	vertical_split_rect->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);

	horizontal_split_rect = app->ui->CreateQuad({ screen.w * 0.5f, screen.h * 0.5f }, UI_QuadDef({ 0,0, (int)screen.w , 6 }, { 150, 150, 150, 255 }));
	horizontal_split_rect->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);

	background = app->ui->CreateQuad({ 0.f, 0.f }, UI_QuadDef({ 0,0, (int)screen.w , (int)screen.h }, { 0, 0, 0 , 200 }));
	background->alpha = 0;

	// Game Over && Win screen ====================================
	
	game_word = app->ui->CreateImage(screen_center + fPoint(10.f, -90.f), UI_ImageDef({ 555,10 ,424,188 }));
	game_word->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	game_word->alpha = 0;
	game_word->SetParent(background);

	over_word = app->ui->CreateImage(screen_center + fPoint(10.f, +90.f), UI_ImageDef({ 555 ,200,383 ,188 }));
	over_word->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	over_word->alpha = 0;
	over_word->SetParent(background);

	you_word = app->ui->CreateImage(screen_center + fPoint(10.f, -90.f), UI_ImageDef({ 990,10 ,314,183 }));
	you_word->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	you_word->alpha = 0;
	you_word->SetParent(background);

	survived_word = app->ui->CreateImage(screen_center + fPoint(10.f, +90.f), UI_ImageDef({ 990,200 ,732,183 }));
	survived_word->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	survived_word->alpha = 0;
	survived_word->SetParent(background);

	UI_LabelDef label_survived_def("text", app->font->label_font_38, { 200, 0 , 0 ,255});
	you_survived = app->ui->CreateLabel({ screen.w * .5f ,  screen.h * .5f + 280.f }, label_survived_def);
	you_survived->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	you_survived->SetParent(background);
	you_survived->alpha = 0;

	// LeaderBoard Screen ==========================================

	//UI_TableDef table_def;
	//table_def.columns = 3;
	//table_def.rows = 11;
	//table_def.line_width = 2;

	//int widths[3] = { 100, 300 , 300 };
	//int heights[11] = { 50, 50 , 50 , 50, 50, 50 , 50 , 50, 50 ,50, 50 };

	//UI_Table* table = app->ui->CreateTable(screen_center, table_def, widths, heights);
	//UI_Element* rank = app->ui->CreateLabel(fPoint(0.f, 0.f), UI_LabelDef("Rank", app->font->label_font_24));
	//table->AssortElementToTable(rank, iPoint(0, 0));

	//for (int i = 1; i < 11; ++i)
	//{
	//	UI_Element* number = app->ui->CreateLabel(fPoint(0.f, 0.f), UI_LabelDef(std::to_string(i), app->font->label_font_24));
	//	table->AssortElementToTable(number, iPoint(0, i));
	//}

	//// ===========================================

	//UI_InputTextDef input_def;
	//input_def.font = app->font->label_font_24;
	//input_def.max_characters = 10;
	//app->ui->CreateInputText(screen_center, input_def);



	// General HUD =================================================

	image_def.sprite_section = { 170 , 10, 105, 105 };
	round_element = app->ui->CreateImage({ screen.w * .5f ,  screen.h * .5f }, image_def);
	round_element->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);

	UI_LabelDef label_round_def("1", app->font->rounds_font);
	round_number_label = app->ui->CreateLabel({ screen.w * .5f ,  screen.h * .5f }, label_round_def);
	round_number_label->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	round_number_label->SetParent(round_element);

	image_def.sprite_section = { 120, 515, 179, 179 };
	round_fx = app->ui->CreateImage({ screen.w * .5f ,  screen.h * .5f }, image_def);
	round_fx->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	round_fx->alpha = 0.f;

	image_def.sprite_section = { 10, 160, 50, 530 };
	left_tank_life = app->ui->CreateImage({ 0.f ,  screen.h * .5f }, image_def);
	left_tank_life->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::CENTER);

	image_def.sprite_section = { 60, 160, 50, 530 };
	right_tank_life = app->ui->CreateImage({ screen.w ,  screen.h * .5f }, image_def);
	right_tank_life->SetPivot(Pivot::POS_X::RIGHT, Pivot::POS_Y::CENTER);

	FadeGeneralHUD(true);
}

General_GUI::~General_GUI()
{
	if (app->on_clean_up == false)
	{
		if (round_number_label != nullptr)
		{
			round_number_label->Destroy();
		}
		if (round_element != nullptr)
		{
			round_element->Destroy();
		}
		if (round_fx != nullptr)
		{
			round_fx->Destroy();
		}
		if (left_tank_life != nullptr)
		{
			left_tank_life->Destroy();
		}
		if (right_tank_life != nullptr)
		{
			right_tank_life->Destroy();
		}
		if (game_word != nullptr)
		{
			game_word->Destroy();
		}
		if (over_word != nullptr)
		{
			over_word->Destroy();
		}
		if (background != nullptr)
		{
			background->Destroy();
		}
		if (vertical_split_rect != nullptr)
		{
			vertical_split_rect->Destroy();
		}
		if (horizontal_split_rect != nullptr)
		{
			horizontal_split_rect->Destroy();
		}
		if (you_survived != nullptr)
		{
			you_survived->Destroy();
		}
		if (you_survived != nullptr)
		{
			you_survived->Destroy();
		}
	}
	
	round_number_label = nullptr;
	round_element = nullptr;
	round_fx = nullptr;
	left_tank_life = nullptr;
	right_tank_life = nullptr;
	game_word = nullptr;
	over_word = nullptr;
	background = nullptr;
	vertical_split_rect = nullptr;
	horizontal_split_rect = nullptr;
	you_survived = nullptr;
	you_word = nullptr;
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
	round_fx->SetFX(UI_Fade_FX::FX_TYPE::INTERMITTENT, 1.F, 3.F);
}

void General_GUI::FadeGameOverScreen(bool fade_on, int rounds_survived)
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
		you_survived->SetFX(type, 2.F);
	}

	background->SetFX(type, 2.F);
	game_word->SetFX(type, 2.F);
	over_word->SetFX(type, 2.F); 
}

void General_GUI::FadeWinScreen(bool fade_on)
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

	String round_str = "NOT BAD AT ALL FOR A ROOKIES...";
   
	you_survived->SetText(round_str);
	you_survived->SetFX(type, 2.F);
    
	background->SetFX(type, 2.F);
	you_word->SetFX(type, 2.F);
	survived_word->SetFX(type, 2.F);
}