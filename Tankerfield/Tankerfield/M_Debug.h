#ifndef __M_DEBUG_H__
#define __M_DEBUG_H__

#include "Module.h"

class M_Debug : public Module
{
	bool PreUpdate() override;
};

#endif