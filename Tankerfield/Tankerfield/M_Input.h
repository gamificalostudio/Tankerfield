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
#define DEFAULT_DEAD_ZONE 7500
//#define LAST_KEYS_PRESSED_BUFFER 50

struct SDL_Rect;
class Camera;

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
	KEY_UP,
};

//Order is based on SDL_GameControllerAxis, if changed, it will stop working
enum class Joystick
{
	LEFT,
	RIGHT,
	MAX
};

//Order is based on SDL_GameControllerAxis, if changed, it will stop working
enum class INPUT_DIR
{
	RIGHT,
	LEFT,
	DOWN,
	UP,
	MAX
};


class M_Input;

struct Controller
{
private:
	bool connected = false;
	int index_number = -1;
	SDL_JoystickID joyId = -1;
	KeyState button_state[SDL_CONTROLLER_BUTTON_MAX];
	KeyState joystick_state[(uint)Joystick::MAX * (uint)INPUT_DIR::MAX];//Only used for joysticks, not for triggers (they have 1, 0 and -1)
	KeyState trigger_state[SDL_CONTROLLER_AXIS_MAX - SDL_CONTROLLER_AXIS_TRIGGERLEFT];//Only used for triggers, not for other axis (they only have 1 and 0)
	SDL_GameController* ctr_pointer = nullptr;
	SDL_Haptic* haptic = nullptr;
	Object* attached_obj = nullptr;
public:
	Controller();

	

private:
	bool attached = false;

	inline KeyState GetButtonState(SDL_GameControllerButton button);

	inline iPoint GetJoystick(Joystick joystick, int dead_zone = DEFAULT_DEAD_ZONE);

	//This funtion returns axis and triggers state value
	// The state is a value ranging from -32768 to 32767.
	inline Sint16 GetAxis(SDL_GameControllerAxis axis, int dead_zone = DEFAULT_DEAD_ZONE);

	//Treat joysticks like buttons or keys to more easily manage them
	inline KeyState GetJoystickState(Joystick joystick, INPUT_DIR joystick_button);

	//Treat triggers like buttons or keys to more easily manage them
	inline KeyState GetTriggerState(SDL_GameControllerAxis axis);

	//strengh -> from 0 to 1
	//length  -> strength of the rumble to play as a 0-1 float value
	inline int PlayRumble(float strengh, Uint32 length);

	inline int StopRumble();

	void DetachController()
	{
		attached = false;
	}

	friend class M_Input;
};

class M_Input : public Module
{

public:

	M_Input();

	// Destructor
	~M_Input();

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


	//Controller funtions===================================================================================================

	KeyState GetControllerButtonState(Controller** controller, SDL_GameControllerButton button);

	iPoint GetControllerJoystick(Controller** controller, Joystick joystick, int dead_zone = DEFAULT_DEAD_ZONE);
	//This funtion returns axis and triggers state value
	// The state is a value ranging from -32768 to 32767.
	Sint16 GetControllerAxis(Controller** controller, SDL_GameControllerAxis axis, int dead_zone = DEFAULT_DEAD_ZONE);

	//Treat joysticks like buttons or keys to more easily manage them
	KeyState GetControllerJoystickState(Controller** controller, Joystick joystick, INPUT_DIR joystick_button);
	//Treat triggers like buttons or keys to more easily manage them
	KeyState GetControllerTriggerState(Controller** controller, SDL_GameControllerAxis axis);

	//strengh -> from 0 to 1
	//length  -> strength of the rumble to play as a 0-1 float value
	int ControllerPlayRumble(Controller** controller, float strengh, Uint32 length);

	int ControllerStopRumble(Controller** controller);

	void DetachController(Controller** controller);

private:
	iPoint GetMousePos_Tiles(const Camera* camera = nullptr);
	void UpdateKeyboardState();
	void UpdateMouseState();
	void UpdateControllers();

private:
	bool		window_events[WE_COUNT];
	KeyState*	keyboard = nullptr;
	KeyState	mouse_buttons[NUM_MOUSE_BUTTONS];
	int			mouse_motion_x = NULL;
	int			mouse_motion_y = NULL;
	int			mouse_x = NULL;
	int			mouse_y = NULL;

public:
	std::string input_text;
	Controller controllers[MAX_CONTROLLERS];
	int GetAbleController();
};

#endif // __j1INPUT_H__