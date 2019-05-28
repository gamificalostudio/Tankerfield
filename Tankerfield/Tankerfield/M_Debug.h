#ifndef __M_DEBUG_H__
#define __M_DEBUG_H__

#include "Module.h"

enum class DebugElement
{
	INVALID = -1,
	WEAPON,
	OBJECT
};

class M_Debug : public Module
{
	bool PreUpdate() override;

private:
	void SelectDebugNumber();
	void ClearDebugNumber();
	int GetNumberFromScancode(int num);

private:
	DebugElement debug_elem = DebugElement::INVALID;

	//Pressed number
	int debug_num = 0;
};

#endif