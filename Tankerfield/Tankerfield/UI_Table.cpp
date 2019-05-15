#include "UI_Table.h"
#include "UI_Element.h"
#include "App.h"
#include "M_Render.h"

UI_Table::UI_Table(const fPoint position, const UI_TableDef definition, UI_Listener* listener) : UI_Element(position, definition, listener), 
rows(definition.rows), columns(definition.columns), quad_width(definition.quad_width), quad_height(definition.quad_height), line_width(definition.line_width)
{
	for (int i = 0; i < rows* columns; ++i)
	{
		elements_list.push_back(nullptr);
	}

	x_offset   =   columns * quad_width * 0.5f;
	y_offset   =   rows * quad_height * 0.5f;

	total_width  = columns * quad_width + line_width * 2;
	total_height = rows * quad_height + line_width * 2;

}

bool UI_Table::Draw()
{
	iPoint pos = { position.x - x_offset,  position.y - y_offset };
	iPoint pos_mod = { 0,0 };
	for (int y = 0; y < rows; ++y)
	{
		pos_mod = pos - iPoint(line_width, line_width);
		app->render->DrawQuad({ pos_mod.x, pos_mod.y, total_width, line_width*2 }, 255, 255,255, 255);
		pos.y += quad_height;
	}

	pos = { position.x - x_offset,  position.y - y_offset };

	for (int x = 0; x < columns; ++x)
	{
		pos_mod = pos - iPoint(line_width, line_width);
		app->render->DrawQuad({ pos_mod.x, pos_mod.y, line_width * 2,  total_height}, 255, 255, 255, 255);
		pos.x += quad_width;
	}
	return true;
}


void UI_Table::SetElements(UI_Element * element, const iPoint position)
{
	elements_list[position.y * columns + position.x] = element;
}

UI_Element* UI_Table::GetElement(iPoint position)
{
	return elements_list[position.y * columns + position.x];
}
