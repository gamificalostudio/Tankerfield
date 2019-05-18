#include "App.h"
#include "M_UI.h"

#include "UI_Label.h"
#include "UI_InputText.h"
#include "UI_Quad.h"

UI_InputText::UI_InputText(const fPoint position, const UI_InputTextDef definition, UI_Listener * listener) : UI_Element(position, definition, listener),
max_characters(definition.max_characters), font(definition.font)
{
	editable_label = app->ui->CreateLabel(fPoint(position), UI_LabelDef("", font, definition.editable_text_color));
	editable_label->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	editable_label->SetParent(this);
	editable_label->SetState(ELEMENT_STATE::HIDDEN);

	default_label = app->ui->CreateLabel(fPoint(position), UI_LabelDef(definition.default_text, font, definition.default_text_color));
	default_label->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	default_label->SetParent(this);
	default_label->SetState(ELEMENT_STATE::VISIBLE);

	SDL_Rect rect = editable_label->GetDrawRect();
	fPoint point(rect.x + rect.w, rect.y + rect.h* 0.5f);
	cursor = app->ui->CreateQuad(point, UI_QuadDef({ 0,0,3,25 }, {255,255,255,255}));
	cursor->SetPivot(Pivot::POS_X::LEFT, Pivot::POS_Y::CENTER);
	cursor->SetParent(editable_label);
	cursor->SetState(ELEMENT_STATE::HIDDEN);
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
	if (input_text.length() < max_characters)
	{
		input_text += text;
		editable_label->SetText(input_text);
		SetCursorPos();
	}

	if (cursor->GetState() == ELEMENT_STATE::HIDDEN)
	{
		cursor->SetState(ELEMENT_STATE::VISIBLE);
		editable_label->SetState(ELEMENT_STATE::VISIBLE);
		default_label->SetState(ELEMENT_STATE::HIDDEN);
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
	}

	if (input_text.empty())
	{
		cursor->SetState(ELEMENT_STATE::HIDDEN);
		editable_label->SetState(ELEMENT_STATE::HIDDEN);
		default_label->SetState(ELEMENT_STATE::VISIBLE);
	}

}

void UI_InputText::SetCursorPos()
{
	SDL_Rect rect = editable_label->GetDrawRect();
	fPoint point(rect.x + rect.w, rect.y + rect.h* 0.5f);
	cursor->SetPos(point);
}