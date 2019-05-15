#include "LeaderBoard.h"
#include "UI_Element.h"

UI_Table::UI_Table(const fPoint position, const UI_TableDef definition, UI_Listener* listener) : UI_Element(position, definition, listener), rows(definition.rows), columns(definition.columns)
{
	columns_width = new int[columns];

	for (int i = 0; i < rows* columns; ++i)
	{
		elements_list.push_back(nullptr);
	}
}

bool UI_Table::Draw()
{
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
