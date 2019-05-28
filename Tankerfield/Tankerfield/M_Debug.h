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
	void SelectNumber();
	int GetNumberFromScancode(int num);

private:
	DebugElement debug_elem = DebugElement::INVALID;

	//Pressed number
	int digits = 0;
	int number = 0;
};

#endif