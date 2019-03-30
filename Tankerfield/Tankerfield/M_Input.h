#ifndef __M_INPUT_H__
#define __M_INPUT_H__

#include <vector>

#include "SDL\include\SDL_gamecontroller.h"
#include "SDL\include\SDL_haptic.h"
#include "SDL\include\SDL.h"
#include "Module.h"
#include "Point.h"


//#define NUM_KEYS 352
#define NUM_MOUSE_BUTTONS 5
#define MAX_CONTROLLERS 4
//#define LAST_KEYS_PRESSED_BUFFER 50

struct SDL_Rect;

enum EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

enum class Joystick
{
	LEFT,
	RIGHT,
	INVALID
};

class M_Input;

struct Controller
{
private:
	int index_number = -1;
	SDL_JoystickID joyId = -1;
	KeyState key_state[SDL_CONTROLLER_BUTTON_MAX];
	SDL_GameController* ctr_pointer = nullptr;
	SDL_Haptic* haptic = nullptr;

public:

	KeyState GetButtonState(SDL_GameControllerButton button)
	{
		return key_state[button];
	}

	bool attached = false;

public:
	
	iPoint GetJoystick(Joystick joystick)
	{
		switch (joystick)
		{
		case Joystick::LEFT:
			return iPoint(GetAxis(SDL_CONTROLLER_AXIS_LEFTX), GetAxis(SDL_CONTROLLER_AXIS_LEFTY));
		case Joystick::RIGHT:
			return iPoint(GetAxis(SDL_CONTROLLER_AXIS_RIGHTX), GetAxis(SDL_CONTROLLER_AXIS_RIGHTX));
		}
	}

	//This funtion returns axis and triggers state value
	// The state is a value ranging from -32768 to 32767.
	Sint16 GetAxis(SDL_GameControllerAxis axis, int dead_zone = 1000)
	{
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

	//strengh -> from 0 to 1
	//length  -> strength of the rumble to play as a 0-1 float value
	int PlayRumble(float strengh, Uint32 length);
	int StopRumble();
  
	friend class M_Input;
};

class M_Input : public Module
{

public:

	M_Input();

	// Destructor
	virtual ~M_Input();

	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	// Called before the first frame
	bool Start()override;

	// Called each loop iteration
	bool PreUpdate()override;

	// Called before quitting
	bool CleanUp()override;

	// Gather relevant win events
	bool GetWindowEvent(EventWindow ev);

	// Check key states (includes mouse and joy buttons)
	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	KeyState GetMouseButton(int id) const
	{
		return mouse_buttons[id - 1];
	}

	// Check if a certain window event happened
	//bool GetWindowEvent(int code);

	// Get mouse / axis position
	void GetMousePosition(int &x, int &y)
	{
		x = mouse_x;
		y = mouse_y;
	}

	void GetMouseMotion(int& x, int& y);

private:
	iPoint GetMousePos_Tiles();
	void UpdateKeyboardState();
	void UpdateMouseState();
	void UpdateControllers();

private:
	bool		windowEvents[WE_COUNT];
	KeyState*	keyboard;
	KeyState	mouse_buttons[NUM_MOUSE_BUTTONS];
	int			mouse_motion_x;
	int			mouse_motion_y;
	int			mouse_x;
	int			mouse_y;

public:
	std::vector<Controller*> controllers;
	Controller** GetAbleController();
};

#endif // __j1INPUT_H__