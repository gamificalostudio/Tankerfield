#ifndef __LEADER_BOARD_H__
#define __LEADER_BOARD_H__

#include <vector>
#include "Point.h"
#include "UI_Element.h"

struct UI_TableDef: public UI_ElementDef
{
	int rows = 0;
	int columns = 0;
};

class UI_Table: public UI_Element
{
public:

	UI_Table(const fPoint position, const UI_TableDef definition, UI_Listener* listener);

	bool Draw();

public:

	void SetElements(UI_Element* element, const iPoint position);

	UI_Element * GetElement(iPoint position);

private:

	int rows = 0;
	int columns = 0;

	int* columns_width = nullptr;
	std::vector< UI_Element* > elements_list;

};

#endif // !__LEADER_BOARD_H__
