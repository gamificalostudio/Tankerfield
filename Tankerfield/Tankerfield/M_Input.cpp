
#include "Brofiler/Brofiler.h"
#include "SDL/include/SDL.h"

#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_Input.h"
#include "M_Window.h"
#include "M_Render.h"
#include "M_Map.h"
#include "M_UI.h"



#define MAX_KEYS 300

M_Input::M_Input() : Module()
{
	name = "input";

	keyboard = DBG_NEW KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
M_Input::~M_Input()
{
	if (keyboard != nullptr)
	{
		delete[] keyboard;
		keyboard = nullptr;
	}
	
	for (int i = 0; i < SDL_NumJoysticks(); ++i) 
	{
		if (SDL_IsGameController(i))
		{
			SDL_GameControllerClose(controllers[i].ctr_pointer);
		}
	}
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
	BROFILER_CATEGORY("M_InputPreUpdate", Profiler::Color::Green)
	static SDL_Event event;
	
	UpdateKeyboardState();
	UpdateMouseState();


	while(SDL_PollEvent(&event) != 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
			{
				window_events[WE_QUIT] = true;
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
					window_events[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
				case SDL_WINDOWEVENT_SHOWN:
				case SDL_WINDOWEVENT_FOCUS_GAINED:
				case SDL_WINDOWEVENT_MAXIMIZED:
				case SDL_WINDOWEVENT_RESTORED:
					window_events[WE_SHOW] = true;
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
				if (num_controller_connected < MAX_CONTROLLERS)
				{
					for (uint i = 0; i < MAX_CONTROLLERS; ++i)
					{
						if (!controllers[i].connected)
						{
							
							controllers[i].ctr_pointer= SDL_GameControllerOpen(i);
							if (!controllers[i].ctr_pointer)
							{
								LOG("Could not open gamecontroller %i: %s\n", i, SDL_GetError());
								break;
							}
							++num_controller_connected;
							controllers[i].connected = true;
							SDL_Joystick* j = SDL_GameControllerGetJoystick(controllers[i].ctr_pointer);
							controllers[i].joyId = SDL_JoystickInstanceID(j);
							controllers[i].haptic = SDL_HapticOpenFromJoystick(j);
							const char* ret_is_hap = (SDL_JoystickIsHaptic(j) == 1) ? "true" : "false";
							LOG("Joys stick is aptic: %s", ret_is_hap);
							if (controllers[i].haptic == nullptr)
							{
								LOG("SDL_HAPTIC ERROR: %s", SDL_GetError());
							}
							else
							{
								LOG("Rumble init %i",SDL_HapticRumbleInit(controllers[i].haptic));
								LOG("Play rumble return %i", controllers[i].PlayRumble(0.5, 100));
							
							}
							break;
						}
					}
				}
				break;
			}
			case SDL_CONTROLLERDEVICEREMOVED:
			{
				for (uint i = 0; i < MAX_CONTROLLERS; i++)
				{
					if (controllers[i].joyId == event.cdevice.which)
					{
						if (controllers[i].haptic != nullptr)
						{
							SDL_HapticClose(controllers[i].haptic);
							controllers[i].haptic = nullptr;
						}
						if (controllers[i].ctr_pointer != nullptr)
						{
							SDL_GameControllerClose(controllers[i].ctr_pointer);
							controllers[i].ctr_pointer = nullptr;
						}
						
						controllers[i].joyId = -1;
						controllers[i].connected = false;
						controllers[i].attached = false;
						--num_controller_connected;
						LOG("Disconnected gamepad index: %d", i);
						break;
					}
				}
				break;
			}
			case SDL_TEXTINPUT:
			{
				input_text = (std::string)event.text.text;
			}
				
		}
	}

	UpdateControllers();

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
bool M_Input::GetWindowEvent(EventWindow ev)
{
	return window_events[ev];
}

// Check key states (includes mouse and joy buttons)

void M_Input::GetMouseMotion(int& x, int& y)
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}

KeyState M_Input::GetControllerButtonState(int i, SDL_GameControllerButton button)
{
	if (i >= 0 && i < MAX_CONTROLLERS && controllers[i].connected)
	{
		return controllers[i].GetButtonState(button);
	}
	return KeyState::KEY_IDLE;
}


iPoint M_Input::GetControllerJoystick(int i, Joystick joystick, int dead_zone)
{
	if (i >= 0 && i < MAX_CONTROLLERS && controllers[i].connected)
	{
		return controllers[i].GetJoystick(joystick, dead_zone);
	}
	return  iPoint(0, 0);

}

Sint16 M_Input::GetControllerAxis(int i, SDL_GameControllerAxis axis, int dead_zone)
{
	if (i >= 0 && i < MAX_CONTROLLERS && controllers[i].connected)
	{
		return controllers[i].GetAxis(axis, dead_zone);
	}
	return 0;

}

KeyState M_Input::GetControllerJoystickState(int i, Joystick joystick, INPUT_DIR joystick_button)
{
	if (i >= 0 && i < MAX_CONTROLLERS && controllers[i].connected)
	{
		return controllers[i].GetJoystickState(joystick, joystick_button);
	}
	return KeyState::KEY_IDLE;

}

KeyState M_Input::GetControllerTriggerState(int i , SDL_GameControllerAxis axis)
{
	if (i >= 0 && i < MAX_CONTROLLERS && controllers[i].connected)
	{
		return controllers[i].GetTriggerState(axis);
	}
	return KeyState::KEY_IDLE;

}

void M_Input::ControllerPlayRumble(int i, float strengh, Uint32 length)
{
	if (i >= 0 && i < MAX_CONTROLLERS && controllers[i].connected)
	{
		controllers[i].PlayRumble(strengh, length);
	}
}

void M_Input::ControllerStopRumble(int i)
{
	if (i >= 0 && i < MAX_CONTROLLERS && controllers[i].connected)
	{
		controllers[i].StopRumble();
	}
}

void M_Input::DetachController(int i)
{
	if (i >= 0 && i < MAX_CONTROLLERS && controllers[i].connected)
	{
		controllers[i].attached = false;
	}
}

bool M_Input::IsConnectedControllet(int i)
{
	if (i >= 0 && i < MAX_CONTROLLERS && controllers[i].connected)
	{
		return true;
	}
	return false;
}

iPoint M_Input::GetMousePos_Tiles(const Camera * camera)
{
	iPoint ret;
	if (camera != nullptr)
		ret = app->render->ScreenToWorld(mouse_x, mouse_y, camera);
	else
		ret = app->render->ScreenToWorld(mouse_x, mouse_y, (*app->render->cameras.begin()));

	ret = app->map->ScreenToMapI(ret.x, ret.y);

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
	for (uint i = 0; i < MAX_CONTROLLERS; ++i)
	{
		if (controllers[i].connected)
		{
			for (int button = SDL_CONTROLLER_BUTTON_A; button < SDL_CONTROLLER_BUTTON_MAX; ++button)
			{
				if (SDL_GameControllerGetButton(controllers[i].ctr_pointer, (SDL_GameControllerButton)button))
				{

					if (controllers[i].button_state[button] == KEY_IDLE)
						controllers[i].button_state[button] = KEY_DOWN;
					else
						controllers[i].button_state[button] = KEY_REPEAT;
				}
				else
				{
					if (controllers[i].button_state[button] == KEY_REPEAT || controllers[i].button_state[button] == KEY_DOWN)
						controllers[i].button_state[button] = KEY_UP;
					else
						controllers[i].button_state[button] = KEY_IDLE;
				}
			}

			for (int trigger = SDL_CONTROLLER_AXIS_TRIGGERLEFT; trigger < SDL_CONTROLLER_AXIS_MAX; ++trigger)
			{
				int trigger_pos_on_array = trigger - SDL_CONTROLLER_AXIS_TRIGGERLEFT;
				if (controllers[i].GetAxis((SDL_GameControllerAxis)trigger) > 0)
				{
					if (controllers[i].trigger_state[trigger_pos_on_array] == KEY_IDLE)
						controllers[i].trigger_state[trigger_pos_on_array] = KEY_DOWN;
					else
						controllers[i].trigger_state[trigger_pos_on_array] = KEY_REPEAT;
				}
				else
				{
					if (controllers[i].trigger_state[trigger_pos_on_array] == KEY_REPEAT || controllers[i].trigger_state[trigger_pos_on_array] == KEY_DOWN)
						controllers[i].trigger_state[trigger_pos_on_array] = KEY_UP;
					else
						controllers[i].trigger_state[trigger_pos_on_array] = KEY_IDLE;
				}
			}

			//Joysticks
			//for joysticks
			int joystick_enum = 0;
			for (int sdl_joystick = SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX; sdl_joystick < SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERLEFT; ++sdl_joystick)
			{
				int joystick_value = controllers[i].GetAxis((SDL_GameControllerAxis)(sdl_joystick));
				//1 value
				if (joystick_value > 0)
				{
					if (controllers[i].joystick_state[joystick_enum] == KEY_IDLE)
						controllers[i].joystick_state[joystick_enum] = KEY_DOWN;
					else
						controllers[i].joystick_state[joystick_enum] = KEY_REPEAT;
				}
				else
				{
					if (controllers[i].joystick_state[joystick_enum] == KEY_REPEAT || controllers[i].joystick_state[joystick_enum] == KEY_DOWN)
						controllers[i].joystick_state[joystick_enum] = KEY_UP;
					else
						controllers[i].joystick_state[joystick_enum] = KEY_IDLE;
				}
				++joystick_enum;
				//-1 value
				if (joystick_value < 0)
				{
					if (controllers[i].joystick_state[joystick_enum] == KEY_IDLE)
						controllers[i].joystick_state[joystick_enum] = KEY_DOWN;
					else
						controllers[i].joystick_state[joystick_enum] = KEY_REPEAT;
				}
				else
				{
					if (controllers[i].joystick_state[joystick_enum] == KEY_REPEAT || controllers[i].joystick_state[joystick_enum] == KEY_DOWN)
						controllers[i].joystick_state[joystick_enum] = KEY_UP;
					else
						controllers[i].joystick_state[joystick_enum] = KEY_IDLE;
				}
				++joystick_enum;
			}
		}
		
	}
}

int M_Input::GetAbleController()
{
	for (uint i=0; i<MAX_CONTROLLERS;++i)
	{
		if (controllers[i].connected && !controllers[i].attached)
		{
			controllers[i].attached = true;
			return i;
		}
	}
	return -1;
}

Controller::Controller()
{
	memset(button_state, KEY_IDLE, sizeof(KeyState) * SDL_CONTROLLER_BUTTON_MAX);
	memset(trigger_state, KEY_IDLE, sizeof(KeyState) * (SDL_CONTROLLER_AXIS_MAX - SDL_CONTROLLER_AXIS_TRIGGERLEFT));
	memset(joystick_state, KEY_IDLE, sizeof(KeyState) * (uint)INPUT_DIR::MAX * (uint)Joystick::MAX);
}

KeyState Controller::GetButtonState(SDL_GameControllerButton button)
{
		return button_state[button];
}

KeyState Controller::GetJoystickState(Joystick joystick, INPUT_DIR joystick_button)
{
	return joystick_state[(uint)joystick * (uint)(INPUT_DIR::MAX) + (uint)joystick_button];
}

KeyState Controller::GetTriggerState(SDL_GameControllerAxis axis)
{
		return trigger_state[axis - SDL_CONTROLLER_AXIS_TRIGGERLEFT];
}

iPoint Controller::GetJoystick(Joystick joystick, int dead_zone)
{
	switch (joystick)
	{
	case Joystick::LEFT:
		return iPoint(GetAxis(SDL_CONTROLLER_AXIS_LEFTX), GetAxis(SDL_CONTROLLER_AXIS_LEFTY));
	case Joystick::RIGHT:
		return iPoint(GetAxis(SDL_CONTROLLER_AXIS_RIGHTX), GetAxis(SDL_CONTROLLER_AXIS_RIGHTY));
	}
}

Sint16 Controller::GetAxis(SDL_GameControllerAxis axis, int dead_zone)
{
	if (ctr_pointer == nullptr)
		return 0;

	Sint16 value = SDL_GameControllerGetAxis(ctr_pointer, axis);
	if (abs(value) > dead_zone)
	{
		return value;
	}
	else
	{
		return 0;
	}
}

//This funtion returns axis and triggers state value
// The state is a value ranging from -32768 to 32767.
//strengh -> from 0 to 1
//length  -> time of the rumble in milliseconds
int Controller::PlayRumble(float strengh, Uint32 length)
{
	return SDL_HapticRumblePlay(haptic, strengh, length);
	
}

int Controller::StopRumble()
{
	return SDL_HapticRumbleStop(haptic);
}

KeyState M_Input::GetControllerButtonOrTriggerState(int controller, CONTROLLER_BUTTON controller_button)
{
	switch (controller_button)
	{
	case CONTROLLER_BUTTON::A:
		return app->input->GetControllerButtonState(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A);
	case CONTROLLER_BUTTON::B:
		return app->input->GetControllerButtonState(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B);
	case CONTROLLER_BUTTON::Y:
		return app->input->GetControllerButtonState(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y);
	case CONTROLLER_BUTTON::X:
		return app->input->GetControllerButtonState(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X);
	case CONTROLLER_BUTTON::L:
		return app->input->GetControllerButtonState(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
	case CONTROLLER_BUTTON::R:
		return app->input->GetControllerButtonState(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
	case CONTROLLER_BUTTON::LT:
		return app->input->GetControllerTriggerState(controller, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERLEFT);
	case CONTROLLER_BUTTON::RT:
		return app->input->GetControllerTriggerState(controller, SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
	}
}
