#include "App.h"
#include "M_MainMenu.h"
#include "M_Scene.h"
#include "M_Window.h"
#include "LeaderBoard.h"
#include "UI_Table.h"
#include "UI_InputText.h"
#include "M_Fonts.h"
#include "Log.h"
#include "UI_Label.h"
#include "UI_Image.h"

LeaderBoard::LeaderBoard(fPoint position, std::string doc_path, bool only_read) : position(position) ,doc_path(doc_path), only_read(only_read)
{
	leader_board_doc.load_file(doc_path.c_str());

	// Create UI Elements ====================================

	UI_TableDef table_def;
	table_def.columns = 3;
	table_def.rows = 11;
	table_def.line_width = 2;

	int widths[3] = { 100, 300 , 100 };
	int heights[11] = { 50, 50 , 50 , 50, 50, 50 , 50 , 50, 50 ,50, 50 };

	if (only_read == false)
	{
		UI_InputTextDef input_def;
		input_def.font = app->font->label_font_24;
		input_def.max_characters = 10;
		input_def.default_text_color = { 200, 200,200,255 };
		input_def.default_text = "Enter your squad name";
		input_text = app->ui->CreateInputText(position - fPoint(0, 400.f), input_def);
	}

	leader_board_table = app->ui->CreateTable(position, table_def, widths, heights);
	leader_board_table->SetMenuState(ELEMENT_STATE::HIDDEN);
}

void LeaderBoard::ShowLeaderBoard()
{
	leader_board_table->SetMenuState(ELEMENT_STATE::VISIBLE);

	for (std::list<UI_Element*>::iterator iter = leader_board_elements.begin(); iter != leader_board_elements.end(); ++iter)
	{
		(*iter)->SetMenuState(ELEMENT_STATE::VISIBLE);
	}
}

void LeaderBoard::HideLeaderBoard()
{
	leader_board_table->SetMenuState(ELEMENT_STATE::HIDDEN);

	for (std::list<UI_Element*>::iterator iter = leader_board_elements.begin(); iter != leader_board_elements.end(); ++iter)
	{
		(*iter)->SetMenuState(ELEMENT_STATE::HIDDEN);
	}
}

void LeaderBoard::FadeLeaderBoardScreen(bool fade_on)
{
	UI_Fade_FX::FX_TYPE type;

	if (fade_on)
	{
		type = UI_Fade_FX::FX_TYPE::FADE_ON;
		input_text->ActiveInputText();
		leader_board_table->SetMenuState(ELEMENT_STATE::VISIBLE);
	}
	else
	{
		type = UI_Fade_FX::FX_TYPE::FADE_OUT;
		input_text->DesactiveInputText();
		input_text->SetFX(type, 2.F);
	}

	leader_board_table->SetFX(type, 2.F);
	
	for (std::list<UI_Element*>::iterator iter = leader_board_elements.begin(); iter != leader_board_elements.end(); ++iter)
	{
		(*iter)->SetFX(type, 2.F);
	}
}

bool LeaderBoard::UpdateLeaderBoard(int round)
{
	bool ret = false;

	

	if (leader_board_doc == NULL)
	{
		LOG("leader_board.xml not found");
		return false;
	}

	pugi::xml_node leader_board_node = leader_board_doc.child("leader_board");
	int rank_pos = 1;

	// Case: XML is empty ===========================================================

	if (leader_board_doc.child("leader_board").child("data") == NULL)
	{
		new_score_node = leader_board_node.prepend_child("data");
		ret = true;
	}

	// Case: XML is not empty ========================================================

	for (pugi::xml_node data_node = leader_board_doc.child("leader_board").child("data"); data_node && ret == false; data_node = data_node.next_sibling("data"))
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

		for (pugi::xml_node data_node = leader_board_doc.child("leader_board").child("data"); data_node; data_node = data_node.next_sibling("data"))
		{
			data_node.attribute("rank") = rank_pos;
			++rank_pos;
		}

		leader_board_doc.save_file(doc_path.c_str());
	}

	return ret;

}

void LeaderBoard::FillLeaderBoardTable()
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

		if (rank_pos == current_rank && only_read == false)
		{
			current_name = name;

			number->color_mod = { 255, 0, 0 ,255 };
			name->color_mod = { 255, 0, 0 ,255 };
			round->color_mod = { 255, 0, 0 ,255 };
		}

		++rank_pos;
	}

}

void LeaderBoard::UpdateLeaderBoardSquadName()
{
	if (new_score_node != NULL && leader_board_doc != NULL)
	{
		new_score_node.attribute("name") = input_text->GetText().c_str();
		leader_board_doc.save_file(doc_path.c_str());
	}
}

void LeaderBoard::SetInputTextToNameLabel()
{
	if (current_name != nullptr)
	{
		current_name->SetText(input_text->GetText());
	}
}