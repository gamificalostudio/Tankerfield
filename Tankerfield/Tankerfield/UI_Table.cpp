#include "UI_Table.h"
#include "UI_Element.h"
#include "App.h"
#include "Log.h"
#include "M_Render.h"

UI_Table::UI_Table(const fPoint position, const UI_TableDef definition, int * widths, int * heights,  UI_Listener* listener) : UI_Element(position, definition, listener),
rows(definition.rows), columns(definition.columns),  line_width(definition.line_width)
{

	this->widths = new int[columns];
	this->heights = new int[rows];

	int width_amount = 0;
	int height_amount = 0;

	for (int i = 0; i < columns; ++i)
	{
		this->widths[i] = widths[i];
		width_amount += this->widths[i];
	}

	for (int i = 0; i < rows; ++i)
	{
		this->heights[i] = heights[i];
		height_amount += this->heights[i];
	}


	x_offset   = width_amount * 0.5f;
	y_offset   = height_amount * 0.5f;

	total_width  = width_amount + line_width * 2;
	total_height = height_amount + line_width * 2;

}

UI_Table::~UI_Table()
{
	if (widths != nullptr)
	{
		RELEASE_ARRAY(widths);
	}

	if (heights != nullptr)
	{
		RELEASE_ARRAY(heights);
	}
}

bool UI_Table::Draw()
{
	iPoint pos = {(int) position.x - x_offset,  (int)position.y - y_offset };
	iPoint pos_mod = { 0,0 };

	for (int y = 0; y <= rows; ++y)
	{
		pos_mod = pos - iPoint(line_width, line_width);
		app->render->DrawQuad({ pos_mod.x, pos_mod.y, total_width, line_width*2 }, 255, 255,255, 255, true, false);

		if (y != rows)
		{
			pos.y += heights[y];
		}
		
	}

	pos = { (int)position.x - x_offset, (int)position.y - y_offset };

	for (int x = 0; x <= columns; ++x)
	{
		pos_mod = pos - iPoint(line_width, line_width);
		app->render->DrawQuad({ pos_mod.x, pos_mod.y, line_width * 2,  total_height}, 255, 255, 255, 255, true, false);

		if (x != columns)
		{
			pos.x += widths[x];
		}
	}
	return true;
}


void UI_Table::AssortElementToTable(UI_Element * element, const iPoint position)
{
	if (element == nullptr || position.x >= columns || position.y >= rows)
	{
		LOG("Imposible Set Element");
		return;
	}

	element->SetPivot(Pivot::POS_X::CENTER, Pivot::POS_Y::CENTER);
	element->SetPos( (fPoint)GetElementScreenPos(position) );
}

iPoint UI_Table::GetElementScreenPos(const iPoint position)
{
	int width_amount = 0;
	int height_amount = 0;

	for (int i = 0; i < position.x; ++i)
	{
		width_amount += widths[i];
	}

	for (int i = 0; i < position.y; ++i)
	{
		height_amount += heights[i];
	}

	return iPoint( this->position.x - x_offset + width_amount + widths[position.x] * 0.5f , this->position.y - y_offset + height_amount + heights[position.y] * 0.5f);
}
