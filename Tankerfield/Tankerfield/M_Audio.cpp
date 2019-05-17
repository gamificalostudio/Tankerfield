
#include "Brofiler/Brofiler.h"
#include "SDL/include/SDL.h"
#include "SDL_mixer/include/SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )


#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_Audio.h"


M_Audio::M_Audio() : Module()
{
	music = NULL;
	name.assign("audio");
}

// Destructor
M_Audio::~M_Audio()
{}

// Called before render is available
bool M_Audio::Start()
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if ((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}
	else
	{
		Mix_VolumeMusic(100);
		Mix_AllocateChannels(16);
	}
	explosion_fx = LoadFx("audio/Fx/explosion.wav");

	return ret;
}

// Called before quitting
bool M_Audio::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if (music != NULL)
	{
		Mix_FreeMusic(music);
	}

	std::list<Mix_Chunk*>::const_iterator item;
	for (item = fx.begin(); item != fx.end(); item++)
		Mix_FreeChunk(*item);

	fx.clear();
	sfx_map.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool M_Audio::PlayMusic(const char* path, float fade_time)
{
	BROFILER_CATEGORY("M_AudioPlayMusic", Profiler::Color::Red)
	bool ret = true;

	if (!active)
		return false;

	if (music != NULL)
	{
		if (fade_time > 0.0f)
		{
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if (music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if (fade_time > 0.0f)
		{
			if (Mix_FadeInMusic(music, -1, (int)(fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if (Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	//LOG("Successfully playing %s", path);
	return ret;
}


// Load WAV
unsigned int M_Audio::LoadFx(const char* path, int volume)
{
	unsigned int ret = 0;

	if (!active)
		return 0;

	std::map<std::string, uint>::iterator iter = sfx_map.find(path);
	if (iter != sfx_map.end())
	{
		return ret = iter->second;
	}
	else
	{
		Mix_Chunk* chunk = Mix_LoadWAV(path);
		//set volume in a range of 0 to MIX_MAX_VOLUME (128)
		Mix_VolumeChunk(chunk, volume);
		if (chunk == NULL)
		{
			LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
		}
		else
		{
			fx.push_back(chunk);
			ret = fx.size();
			sfx_map[path] = ret;
		}
	}
	return ret;
}

// Play WAV
int M_Audio::PlayFx
(unsigned int id, int repeat)
{
	int ret = -1;

	if (!active)
		return false;

	if (id > 0 && id <= fx.size())
	{
		std::list<Mix_Chunk*>::const_iterator item = std::next(fx.begin(), id - 1);
		ret = Mix_PlayChannel(-1, *item, repeat);
	}

	return ret;
}

void M_Audio::PauseMusic(int fade_out)
{
	Mix_FadeOutMusic(fade_out);
}

void M_Audio::PauseFx(unsigned int channel, int fade_out)
{
	Mix_FadeOutChannel(channel, fade_out);	
}

unsigned int M_Audio::GetExplosionFx()
{
	return explosion_fx;
}

