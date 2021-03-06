#ifndef __LEADER_BOARD_H__
#define __LEADER_BOARD_H__

#include <vector>
#include "Point.h"
#include "UI_Element.h"

struct UI_TableDef: public UI_ElementDef
{
	int rows = 0;
	int columns = 0;
	int line_width = 0;

public:
	UI_TableDef()
	{};

	UI_TableDef(int rows, int columns, int line_width) : rows(rows), columns(columns), line_width(line_width)
	{};
};

class UI_Table: public UI_Element
{
public:

	UI_Table(const fPoint position, const UI_TableDef definition, int * widths, int * heights, UI_Listener* listener);

	~UI_Table();

	bool Draw();

public:

	void AssortElementToTable(UI_Element* element, const iPoint position);

private:

	iPoint GetElementScreenPos(const iPoint position);

private:

	int rows = 0;
	int columns = 0;
	int line_width = 0;
	int * widths = nullptr;
	int * heights = nullptr;
	int quad_height = 0;

	int x_offset =		0;
	int y_offset =		0;
		
	int total_width =	0;
	int total_height =	0;
};

#endif // !__LEADER_BOARD_H__
