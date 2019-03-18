#ifndef __MEMLEAKS_H__
#define __MEMLEAKS_H__

#include <stdlib.h>
#include <crtdbg.h>

#define _CRTDBG_MAP_ALLOC

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif

#define ReportMemoryLeaks() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)

#endif /* __MEMLEAKS_H__ */