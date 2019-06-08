#ifndef __J1_PARTICLE_SYSTEM_H__
#define __J1_PARTICLE_SYSTEM_H__

#include "Point.h"
#include "Module.h"
#include <string>
#include "SDL/include/SDL.h"
#include <map>

#define MAX_NUM_EMITTERS_TYPE 1

class Obj_Emitter;
struct SDL_Texture;
struct SDL_Rect;
struct SDL_Color;

enum class ObjectType;

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
	SDL_Texture* particleAtlas = nullptr;
	std::string nameParticleAtlas;

	std::map<ObjectType, EmitterData> vecEmitterData;
	
public:

	ParticleSystem();

	virtual ~ParticleSystem();
	bool Awake();
	bool Start();
	bool CleanUp();

	SDL_Texture* GetParticleAtlas() const;
	void LoadEmitterData(pugi::xml_node& config, ObjectType type);

	friend class M_ObjManager;
};

#endif