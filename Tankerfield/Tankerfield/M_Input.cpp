#include "p2Defs.h"
#include "p2Log.h"
#include "App.h"
#include "M_Input.h"
#include "M_Window.h"
#include "M_Render.h"
#include "M_Map.h"
#include "SDL/include/SDL.h"


#define MAX_KEYS 300

M_Input::M_Input() : Module()
{
	name = "input";

	keyboard = new j1KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(j1KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(j1KeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
M_Input::~M_Input()
{
	
}

// Called before render is available
bool M_Input::Awake(pugi::xml_node& config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called before the first frame
bool M_Input::Start()
{
	bool ret = true;
	SDL_StopTextInput();

	if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0)
	{
		LOG("SDL_INIT_GAMECONTROLLER could not initialize! SDL_Error: %s\n", SDL_GetError());
		return ret = false;
	}
	if (SDL_Init(SDL_INIT_HAPTIC) < 0)
	{
		LOG("SDL_INIT_HAPTIC could not initialize! SDL_Error: %s\n ", SDL_GetError());
		return ret = false;
	}
	return true;
}

// Called each loop iteration
bool M_Input::PreUpdate()
{
	static SDL_Event event;
	
	UpdateKeyboardState();
	UpdateMouseState();

	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
			{
				windowEvents[WE_QUIT] = true;
				break;
			}
			case SDL_WINDOWEVENT:
			{
				switch (event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
				case SDL_WINDOWEVENT_HIDDEN:
				case SDL_WINDOWEVENT_MINIMIZED:
				case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
				case SDL_WINDOWEVENT_SHOWN:
				case SDL_WINDOWEVENT_FOCUS_GAINED:
				case SDL_WINDOWEVENT_MAXIMIZED:
				case SDL_WINDOWEVENT_RESTORED:
					windowEvents[WE_SHOW] = true;
					break;
				}
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				mouse_buttons[event.button.button - 1] = KEY_DOWN;
				//LOG("Mouse button %d down", event.button.button-1);
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				mouse_buttons[event.button.button - 1] = KEY_UP;
				//LOG("Mouse button %d up", event.button.button-1);
				break;
			}
			case SDL_MOUSEMOTION:
			{
				int scale = app->win->GetScale();
				mouse_motion_x = event.motion.xrel / scale;
				mouse_motion_y = event.motion.yrel / scale;
				mouse_x = event.motion.x / scale;
				mouse_y = event.motion.y / scale;
				//LOG("Mouse motion x %d y %d", mouse_motion_x, mouse_motion_y);
				break;
			}
			case SDL_CONTROLLERDEVICEADDED:
			{
				if (controllers.size() < MAX_CONTROLLERS)
				{
					int num_joystincks = SDL_NumJoysticks();
					for (int i = 0; i < num_joystincks; ++i)
					{
						bool is_joystick = false;
						for (std::vector<Controller*>::iterator iter = controllers.begin(); iter != controllers.end(); ++iter)
						{
							if ((*iter)->index_number == i)
							{
								is_joystick = true;
							}
						}
						if (!is_joystick)
						{
							Controller* controller = new Controller();
							controller->ctr_pointer = SDL_GameControllerOpen(i);
							SDL_Joystick* j = SDL_GameControllerGetJoystick(controller->ctr_pointer);
							controller->joyId = SDL_JoystickInstanceID(j);
							controller->index_number = i;
							controller->haptic = SDL_HapticOpen(i);
							LOG("Joys stick is aptic: %i", SDL_JoystickIsHaptic(j));
							if (controller->haptic == NULL)
							{
								LOG("SDL_HAPTIC ERROR: %s", SDL_GetError());
							}
							else
							{
								SDL_HapticRumbleInit(controller->haptic);
							}
							controllers.push_back(controller);
						}
					}
				}
				break;
			}
			case SDL_CONTROLLERDEVICEREMOVED:
			{
				for (std::vector<Controller*>::iterator iter = controllers.begin(); iter != controllers.end();)
				{
					if (SDL_GameControllerGetAttached((*iter)->ctr_pointer) == false)
					{
						if((*iter)->haptic!=nullptr)
							SDL_HapticClose((*iter)->haptic);

						if ((*iter)->ctr_pointer != nullptr)
						SDL_GameControllerClose((*iter)->ctr_pointer);

						delete (*iter);
						(*iter) = nullptr;
						iter = controllers.erase(iter);
					}
					else
					{
						++iter;
					}
				}
				break;
			}
		}
	}

	return true;
}

// Called before quitting
bool M_Input::CleanUp()
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	//RELEASE_ARRAY(keyboard);
	return true;
}

// ---------
bool M_Input::GetWindowEvent(j1EventWindow ev)
{
	return windowEvents[ev];
}

void M_Input::GetMousePosition(int& x, int& y)
{
	x = mouse_x;
	y = mouse_y;
}

void M_Input::GetMouseMotion(int& x, int& y)
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}


iPoint M_Input::GetMousePos_Tiles()
{
	iPoint ret;
	ret = app->render->ScreenToWorld(mouse_x, mouse_y);
	ret = app->map->WorldToMap(ret.x, ret.y);

	return ret;
}

void M_Input::UpdateKeyboardState()
{
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}
}

void M_Input::UpdateMouseState()
{
	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if (mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}
}

void M_Input::UpdateControllers()
{
	for (std::vector<Controller*>::iterator iter = controllers.begin(); iter != controllers.end(); ++iter)
	{
		for (int button = SDL_CONTROLLER_BUTTON_A; button < SDL_CONTROLLER_BUTTON_MAX; ++button)
		{
			if (SDL_GameControllerGetButton((*iter)->ctr_pointer, (SDL_GameControllerButton)button))
			{
				
				if ((*iter)->key_state[button] == KEY_IDLE)
					(*iter)->key_state[button] = KEY_DOWN;
				else
					(*iter)->key_state[button] = KEY_REPEAT;
			}
			else
			{
				if ((*iter)->key_state[button] == KEY_REPEAT || (*iter)->key_state[button] == KEY_DOWN)
					(*iter)->key_state[button] = KEY_UP;
				else
					(*iter)->key_state[button] = KEY_IDLE;
			}
		}
		
	}
}

Controller** M_Input::GetAbleController()
{
	Controller** ret = nullptr;
	for (std::vector<Controller*>::iterator iter = controllers.begin(); iter != controllers.end(); ++iter)
	{
		if (!(*iter)->attached)
		{
			(*iter)->attached = true;
			return &(*iter);
		}
	}
	return ret;
}

