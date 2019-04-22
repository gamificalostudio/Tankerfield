#ifndef __M_AUDIO_H__
#define __M_AUDIO_H__

#include <list>
#include <map>

#include "Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class M_Audio : public Module
{
public:
	// Constructor
	M_Audio();

	// Destructor
	virtual ~M_Audio();

	// Called before quitting
	bool CleanUp();

	// Called before first frame
	bool Start();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	//_Mix_Music* LoadMusic(const char* path, float fade_time);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path, int volume = 50);

	// Play a previously loaded WAV
	//if the reapeat ==-1 it loops infinite
	//this funtion returns the number of the channel the sfx is play on, so you can stop that chanel if you want
	int PlayFx(unsigned int fx, int repeat = 0);

	void PauseMusic(int fade_out);

	void PauseFx(unsigned int id, int fade_out);

public:
	uint GetExplosionFx();

	uint explosion_fx;

private:
	_Mix_Music* music = nullptr;
	std::map<std::string, uint> sfx_map;
	std::list<Mix_Chunk*>	fx;
};

#endif // __j1AUDIO_H__