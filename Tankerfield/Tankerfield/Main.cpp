#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <stdlib.h>

#include "Defs.h"
#include "Log.h"
#include "App.h"

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )

#include"Brofiler/Brofiler.h"
#pragma comment (lib,"Brofiler/ProfilerCore32.lib")



enum MAIN_STATE
{
	CREATE = 1,
	AWAKE,
	START,
	LOOP,
	CLEAN,
	FAIL,
	EXIT
};

App* app = NULL;

int main(int argc, char* args[])
{
	LOG("Engine starting ... %d");

	MAIN_STATE state = MAIN_STATE::CREATE;
	int result = EXIT_FAILURE;

	while (state != EXIT)
	{
		switch (state)
		{

			// Allocate the engine --------------------------------------------
		case CREATE:
			LOG("CREATION PHASE ===============================");

			app = DBG_NEW App(argc, args);

			if (app != NULL)
				state = AWAKE;
			else
				state = FAIL;

			break;

			// Awake all modules -----------------------------------------------
		case AWAKE:
			LOG("AWAKE PHASE ===============================");
			if (app->Awake() == true)
				state = START;
			else
			{
				LOG("ERROR: Awake failed");
				state = FAIL;
			}

			break;

			// Call all modules before first frame  ----------------------------
		case START:
			LOG("START PHASE ===============================");
			if (app->Start() == true)
			{
				state = LOOP;
				LOG("UPDATE PHASE ===============================");
			}
			else
			{
				state = FAIL;
				LOG("ERROR: Start failed");
			}
			break;

			// Loop all modules until we are asked to leave ---------------------
		case LOOP:
		{
			BROFILER_FRAME("frame");
			if (app->Update() == false)
				state = CLEAN;
		}
			break;

			// Cleanup allocated memory -----------------------------------------
		case CLEAN:
			LOG("CLEANUP PHASE ===============================");
			if (app->CleanUp() == true)
			{
				RELEASE(app);
				result = EXIT_SUCCESS;
				state = EXIT;
			}
			else
				state = FAIL;

			break;

			// Exit with errors and shame ---------------------------------------
		case FAIL:
			LOG("Exiting with errors :(");
			result = EXIT_FAILURE;
			state = EXIT;
			break;
		}
	}

	LOG("Exit: Application \n");
	_CrtDumpMemoryLeaks();
	return result;
}