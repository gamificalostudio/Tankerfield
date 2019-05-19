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

	vertical_split_rect = app->ui->CreateQuad(screen_center, UI_QuadDef({ 0 ,0, 6 , (int)screen.h }, { 150, 150, 150, 255 }));
	vertical_split_rect->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	horizontal_split_rect = app->ui->CreateQuad({ screen.w * 0.5f, screen.h * 0.5f }, UI_QuadDef({ 0,0, (int)screen.w , 6 }, { 150, 150, 150, 255 }));
	horizontal_split_rect->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);

	background = app->ui->CreateQuad({ 0.f, 0.f }, UI_QuadDef({ 0,0, (int)screen.w , (int)screen.h }, { 0, 0, 0 , 200 }));
	background->alpha = 0;

	// Control helper ============================================
	
	control_helper_image = app->ui->CreateImage(screen_center + fPoint(- 40.f, 400.f), UI_ImageDef(app->ui->button_sprites[(int)CONTROLLER_BUTTON::A]));
	control_helper_image->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	control_helper_image->alpha = 0;
	control_helper_image->SetParent(background);

	control_helper_label = app->ui->CreateLabel(screen_center + fPoint(10.f, 400.f), UI_LabelDef("Acept", app->font->label_font_24));
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

	// LeaderBoard Screen ==========================================

	UI_TableDef table_def;
	table_def.columns = 3;
	table_def.rows = 11;
	table_def.line_width = 2;

	int widths[3] = { 100, 300 , 300 };
	int heights[11] = { 50, 50 , 50 , 50, 50, 50 , 50 , 50, 50 ,50, 50 };

	leader_board_table = app->ui->CreateTable(screen_center, table_def, widths, heights);
	leader_board_table->SetParent(background);
	leader_board_table->alpha = 0;

	UI_InputTextDef input_def;
	input_def.font = app->font->label_font_24;
	input_def.max_characters = 10;
	input_def.default_text_color = { 200, 200,200,255 };
	input_def.default_text = "Enter your squad name";
;	input_text = app->ui->CreateInputText(screen_center - fPoint( 0, 400.f), input_def);
	input_text->alpha = 0.f;

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
	right_tank_life = app->ui->CreateImage({ screen.w ,  screen.h * .5f }, image_def);
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

void General_GUI::SetInputTextToNameLabel()
{
	if (current_name != nullptr)
	{
		current_name->SetText(input_text->GetText());
	}
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

void General_GUI::FadeWinScreen(bool fade_on)
{
	UI_Fade_FX::FX_TYPE type;

	if (fade_on)
	{
		type = UI_Fade_FX::FX_TYPE::FADE_ON;
		background->SetFX(type, 2.F);
	}
	else
	{
		type = UI_Fade_FX::FX_TYPE::FADE_OUT;
	}

	String round_str = "NOT BAD AT ALL FOR A ROOKIES...";
   
	you_survived->SetText(round_str);
	you_survived->SetFX(type, 2.F);
	you_word->SetFX(type, 2.F);
	survived_word->SetFX(type, 2.F);
}

void General_GUI::FadeLeaderBoardScreen(bool fade_on)
{
	UI_Fade_FX::FX_TYPE type;

	if (fade_on)
	{
		type = UI_Fade_FX::FX_TYPE::FADE_ON;
		input_text->ActiveInputText();
	}
	else
	{
		type = UI_Fade_FX::FX_TYPE::FADE_OUT;
		background->SetFX(type, 2.F);
		input_text->DesactiveInputText();
		control_helper_image->SetFX(type, 2.F);
		control_helper_label->SetFX(type, 2.F);
	}
	
	leader_board_table->SetFX(type, 2.F);
	input_text->SetFX(type, 2.F);

	for (std::list<UI_Element*>::iterator iter = leader_board_elements.begin(); iter != leader_board_elements.end(); ++iter)
	{
		(*iter)->SetFX(type, 2.F);
	}
}

bool General_GUI::UpdateLeaderBoard(std::string path, int round)
{
	bool ret = false;

	leader_board_doc.load_file(path.c_str());

	if (leader_board_doc == NULL)
	{
		LOG("leader_board.xml not found");
		return false;
	}

	doc_path = path;
	pugi::xml_node leader_board_node = leader_board_doc.child("leader_board");
	int rank_pos = 1;

	// Case: XML is empty ===========================================================

	if (leader_board_doc.child("leader_board").child("data") == NULL)
	{
		new_score_node = leader_board_node.prepend_child("data");
		ret = true;
	}

	// Case: XML is not empty ========================================================

	for (pugi::xml_node data_node = leader_board_doc.child("leader_board").child("data") ; data_node && ret == false ; data_node = data_node.next_sibling("data"))
	{
		if (data_node.attribute("round").as_int(0) <= round)
		{
			new_score_node = data_node;
			break;
		}

		++rank_pos;
	}

	if (ret == false)
	{
		// It is placed between current data nodes -------------------------
		if (new_score_node != NULL)
		{
			new_score_node = leader_board_node.insert_child_before("data", new_score_node);
			ret = true;
		}

		// It is placed at the end of data nodes ---------------------------
		else
		{
			new_score_node = leader_board_node.append_child("data");
			ret = true;
		}
	}
	
	// Add new data node ===========================================================

	if (ret == true)
	{
		new_score_node.append_attribute("rank") = rank_pos;
		new_score_node.append_attribute("round") = round;
		new_score_node.append_attribute("name") = "";
	
		current_rank = rank_pos;

		// Update next data nodes rank positions ---------------------------

		rank_pos = 1;

		for (pugi::xml_node data_node = leader_board_doc.child("leader_board").child("data"); data_node ; data_node = data_node.next_sibling("data"))
		{
			data_node.attribute("rank") = rank_pos;
			++rank_pos;
		}

		leader_board_doc.save_file(path.c_str());
	}

	return ret;

}

void General_GUI::FillLeaderBoardTable()
{
	int rank_pos = 1;

	UI_Label* aux = app->ui->CreateLabel(fPoint(0, 0), UI_LabelDef("Rank", app->font->label_font_24));
	leader_board_table->AssortElementToTable(aux, iPoint(0, 0));
	leader_board_elements.push_back(aux);

	aux = app->ui->CreateLabel(fPoint(0, 0), UI_LabelDef("Squad Name", app->font->label_font_24));
	leader_board_table->AssortElementToTable(aux, iPoint(1, 0));
	leader_board_elements.push_back(aux);

	aux = app->ui->CreateLabel(fPoint(0, 0), UI_LabelDef("Round", app->font->label_font_24));
	leader_board_table->AssortElementToTable(aux, iPoint(2, 0));
	leader_board_elements.push_back(aux);


	for (pugi::xml_node data_node = leader_board_doc.child("leader_board").child("data"); data_node && rank_pos <= 10; data_node = data_node.next_sibling("data"))
	{
		UI_Label* number = app->ui->CreateLabel(fPoint(0.f, 0.f), UI_LabelDef(data_node.attribute("rank").as_string(""), app->font->label_font_24));
		leader_board_elements.push_back(number);
		leader_board_table->AssortElementToTable(number, iPoint(0, rank_pos));

		UI_Label* name = app->ui->CreateLabel(fPoint(0.f, 0.f), UI_LabelDef(data_node.attribute("name").as_string(""), app->font->label_font_24));
		leader_board_elements.push_back(name);
		leader_board_table->AssortElementToTable(name, iPoint(1, rank_pos));

		UI_Label* round = app->ui->CreateLabel(fPoint(0.f, 0.f), UI_LabelDef(data_node.attribute("round").as_string(""), app->font->label_font_24));
		leader_board_elements.push_back(round);
		leader_board_table->AssortElementToTable(round, iPoint(2, rank_pos));

		if (rank_pos == current_rank)
		{
			current_name = name;
			
			number->color_mod = { 255, 0, 0 ,255 };
			name->color_mod = { 255, 0, 0 ,255 };
			round->color_mod = { 255, 0, 0 ,255 };
		}

		++rank_pos;
	}

}

void General_GUI::UpdateLeaderBoardSquadName()
{
	if (new_score_node != NULL && leader_board_doc != NULL)
	{
		new_score_node.attribute("name") = input_text->GetText().c_str();
		leader_board_doc.save_file(doc_path.c_str());
	}
}
