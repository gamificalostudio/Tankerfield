#ifndef __J1_PARTICLE_SYSTEM_H__
#define __J1_PARTICLE_SYSTEM_H__

#include "Point.h"
#include "Module.h"
#include "Obj_Emitter.h"
#include <list>
#include <string>
#include "SDL/include/SDL.h"

#define MAX_NUM_EMITTERS_TYPE 1

class Obj_Emitter;
struct SDL_Texture;
struct SDL_Rect;
struct SDL_Color;

enum EmitterType
{
	EMITTER_TYPE_NONE = -1,
	EMITTER_TYPE_FIRE
};

struct EmitterData
{
	fPoint angleRange = { 0.0f, 0.0f };
	float startSpeed = 0.0f, endSpeed = 0.0f;
	float startSize = 0.0f, endSize = 0.0f;
	uint emitNumber = 0u;
	uint emitVariance = 0u;
	uint maxParticleLife = 0u;
	SDL_Rect textureRect = { 0, 0 };
	double lifetime = -1.0f;
	SDL_Color startColor = { 0, 0, 0, 0 };
	SDL_Color endColor = { 0, 0, 0, 0 };
	SDL_BlendMode blendMode = SDL_BlendMode::SDL_BLENDMODE_NONE;
	double rotSpeed = 0;

	// Randoms
	fPoint rotSpeedRand = { 0.0f, 0.0f };
	fPoint startSpeedRand = { 0.0f, 0.0f };
	fPoint endSpeedRand = { 0.0f, 0.0f };
	fPoint emitVarianceRand = { 0.0f, 0.0f };
	fPoint lifeRand = { 0.0f, 0.0f };
	fPoint startSizeRand = { 0.0f, 0.0f };
	fPoint endSizeRand = { 0.0f, 0.0f };
};

class ParticleSystem
{

private:

	std::list<Obj_Emitter*> emitters_list;
	SDL_Texture* particleAtlas = nullptr;
	std::string nameParticleAtlas;

	// Static array that stores all the data of emitters
	EmitterData vecEmitterData[MAX_NUM_EMITTERS_TYPE];
	
public:

	ParticleSystem();

	virtual ~ParticleSystem();
	bool Awake();
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	// Emitter methods
	Obj_Emitter* AddEmiter(fPoint pos, EmitterType type);
	bool RemoveEmitter(Obj_Emitter& emitter);
	bool RemoveAllEmitters();

	SDL_Texture* GetParticleAtlas() const;
	void LoadEmitterData(pugi::xml_node& config, EmitterType type);
};

#endif