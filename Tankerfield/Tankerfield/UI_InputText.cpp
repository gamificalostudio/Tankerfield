#include "App.h"
#include "M_UI.h"
#include "UI_Label.h"
#include "UI_InputText.h"
#include "UI_Quad.h"

UI_InputText::UI_InputText(const fPoint position, const UI_InputTextDef definition, UI_Listener * listener) : UI_Element(position, definition, listener),
max_characters(definition.max_characters), font(definition.font)
{
	editable_label = app->ui->CreateLabel(fPoint(position), UI_LabelDef("", font, definition.editable_text_color));
	editable_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	editable_label->SetParent(this);
	editable_label->SetMenuState(ELEMENT_STATE::HIDDEN);

	default_label = app->ui->CreateLabel(fPoint(position), UI_LabelDef(definition.default_text, font, definition.default_text_color));
	default_label->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	default_label->SetParent(this);
	default_label->SetMenuState(ELEMENT_STATE::HIDDEN);

	SDL_Rect rect = editable_label->GetDrawRect();
	fPoint point(rect.x + rect.w, rect.y + rect.h* 0.5f);
	cursor = app->ui->CreateQuad(point, UI_QuadDef({ 0,0,3,25 }, {255,255,255,255}));
	cursor->SetPivot(Pivot::X::LEFT, Pivot::Y::CENTER);
	cursor->SetParent(editable_label);
	cursor->SetMenuState(ELEMENT_STATE::HIDDEN);

}

bool UI_InputText::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
	{
		DeleteLastChar();
	}

	if (!app->input->input_text.empty())
	{
		std::string t = app->input->input_text;
		app->input->input_text.clear();
		AddText(t.data());
	}
	return true;
}

std::string UI_InputText::GetText()
{
	return input_text;
}

void UI_InputText::AddText(std::string text)
{
	if (active_input_text == true)
	{
		if (input_text.length() < max_characters)
		{
			input_text += text;
			editable_label->SetText(input_text);
			SetCursorPos();
		}

		if (input_state == INPUT_STATE::DEFAULT_TEXT)
		{
			SetInputState(INPUT_STATE::EDITABLE_TEXT);
		}
	}
}

void UI_InputText::AddChar(char character)
{
	if (input_text.length() < max_characters)
	{
		input_text += character;
		editable_label->SetText(input_text);
	}
}

void UI_InputText::DeleteLastChar()
{
	if (!input_text.empty()) 
	{
		input_text.pop_back();
		editable_label->SetText(input_text);
		SetCursorPos();

		if (input_text.empty())
		{
			SetInputState(INPUT_STATE::DEFAULT_TEXT);
		}
	}

}

void UI_InputText::SetCursorPos()
{
	SDL_Rect rect = editable_label->GetDrawRect();
	fPoint point(rect.x + rect.w, rect.y + rect.h* 0.5f);
	cursor->SetPos(point);
}

void UI_InputText::ActiveInputText()
{
	active_input_text = true;
	SetInputState(INPUT_STATE::DEFAULT_TEXT);
}

void UI_InputText::DesactiveInputText()
{
	active_input_text = false;
}

void UI_InputText::SetInputState(INPUT_STATE state)
{
	input_state = state;

	switch (state)
	{
	case INPUT_STATE::DEFAULT_TEXT:

		cursor->SetMenuState(ELEMENT_STATE::HIDDEN);
		cursor->FinishFX();
		cursor->alpha = 0;
		editable_label->SetMenuState(ELEMENT_STATE::HIDDEN);
		default_label->SetMenuState(ELEMENT_STATE::VISIBLE);
		default_label->SetFX(UI_Fade_FX::FX_TYPE::INTERMITTENT, 1.f, -1.f, 0.f, 255.f);

		break;
	case INPUT_STATE::EDITABLE_TEXT:

		cursor->SetMenuState(ELEMENT_STATE::VISIBLE);
		cursor->SetFX(UI_Fade_FX::FX_TYPE::INTERMITTENT, 0.5f, -1.f, 0.f, 255.f);

		editable_label->SetMenuState(ELEMENT_STATE::VISIBLE);
		default_label->SetMenuState(ELEMENT_STATE::HIDDEN);
		default_label->FinishFX();
		default_label->alpha = 0;

		break;
	}
}

void UI_InputText::Destroy()
{
	to_destroy = true;

	if (default_label != nullptr)
	{
		default_label->Destroy();
	}

	if (editable_label != nullptr)
	{
		editable_label->Destroy();
	}

	if (cursor != nullptr)
	{
		cursor->Destroy();
	}

}