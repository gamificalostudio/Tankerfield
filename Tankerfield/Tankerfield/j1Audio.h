#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include <list>
#include "j1Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class j1Audio : public j1Module
{
public:
	// Constructor
	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before quitting
	bool CleanUp();

	// Called before first frame
	bool Start();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

private:
	_Mix_Music* music = nullptr;
	std::list<Mix_Chunk*>	fx;
};

#endif // __j1AUDIO_H__