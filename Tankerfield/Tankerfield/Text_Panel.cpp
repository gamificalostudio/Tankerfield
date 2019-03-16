#include "p2Defs.h"
#include "p2Log.h"
#include "Text_Panel.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1Textures.h"
#include "j1App.h"

TextPanel::TextPanel(const iPoint position, TextPanel_Definition definition, Gui_Listener * listener): UI_Object(position, listener)
{
	section.x = section.y = 0;
	this->definition = definition;
	SetText(definition.text);
}

TextPanel::~TextPanel()
{
	if (text_texture != nullptr)
	{
		App->tex->UnLoad(text_texture);
	}
}

void TextPanel::SetText(String text)
{
	
}

bool TextPanel::Draw()
{

	return true;
}

//
//void TextPanel::SetText(String text)
//{
//	if (text_texture != nullptr)
//	{
//		App->tex->UnLoad(text_texture);
//	}
//
//	String txt(text);
//
//	int last_texture_x = 0;
//	int total_text_length = txt.Length();
//	const char * char_text = txt.GetString();
//	int last_line_first_char = 0;
//	int previous_word_last_letter = 0;
//	int current_line_fist_letter = 0;
//
//	lines = 0;
//	// Determinate font height ----------------------------------
//	App->font->CalcSize(txt.GetString(), font_width, font_height, definition.font);
//
//	// Iterate text by found lines ------------------------------
//
//	for (uint i = 0; i < total_text_length; ++i)
//	{
//		if (char_text[i + 1] == ' ')
//		{
//			// Check pixel width of substring -----------------------------
//			p2SString str(txt);
//
//			str.CutString(current_line_fist_letter - 1, i + 1);
//			App->font->CalcSize(str.GetString(), font_width, font_height, definition.font);
//
//			LOG("%s", str.GetString());
//
//			// If substring is wider than a line create a line from the previous word last letter
//			if (font_width > definition.width)
//			{
//				p2SString line_str(txt);
//
//				// Skip spaces between lines ==============
//				uint j = i + 1;
//
//				if (previous_word_last_letter == 0)
//				{
//					previous_word_last_letter = i;
//				}
//
//				while (j <= total_text_length)
//				{
//					if (char_text[j] != ' ')
//					{
//						break;
//					}
//					++j;
//				}
//				i = j;
//
//				line_str.CutString(current_line_fist_letter - 1, previous_word_last_letter + 1);
//				LOG("%s", line_str.GetString());
//				App->font->CalcSize(line_str.GetString(), font_width, font_height, definition.font);
//				rects.PushBack({ last_texture_x, 0 , font_width, font_height });
//				// Set values ============================
//				previous_word_last_letter = i;
//				last_texture_x = last_texture_x + font_width;
//				current_line_fist_letter = j;
//				++lines;
//
//				continue;
//			}
//
//			if (char_text[i] != ' ')
//			{
//				previous_word_last_letter = i;
//			}
//
//		}
//		else if (char_text[i + 1] == '\0')
//		{
//			// Add leftover text
//			++lines;
//
//			p2SString str(txt);
//
//			str.CutString(current_line_fist_letter - 1, i + 1);
//			App->font->CalcSize(str.GetString(), font_width, font_height, definition.font);
//			rects.PushBack({ last_texture_x, 0 , font_width, font_height });
//
//			LOG("%s", str.GetString());
//		}
//
//		section.w = definition.width;
//		section.h = lines * (font_height + definition.line_spacing);
//		text_texture = App->font->Print(text.GetString(), definition.color, definition.font);
//	}
//}
//
//
//bool TextPanel::Draw()
//{
//	if (text_texture == nullptr)
//	{
//		return true;
//	}
//	for (uint i = 0; i < rects.Count(); ++i)
//	{
//		App->render->Blit(text_texture, position.x - section.w *0.5f, position.y - section.h *0.5f + i * (font_height + definition.line_spacing), rects.At(i), false, 0.0f);
//	}
//
//	return true;
//}