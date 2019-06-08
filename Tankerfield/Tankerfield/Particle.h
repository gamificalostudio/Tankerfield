#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Point.h"
#include "SDL/include/SDL.h"

#define MIN_LIFE_TO_INTERPOLATE 15

struct SDL_Texture;

class Particle
{
	private:

	/*  This is the only variable we care about no matter if
	   the particle is alive or dead */
	uint life = 0;

	struct Vortex
	{
		fPoint pos = { 0.0f, 0.0f };
		float speed = 0.0f;
		float scale = 0.0f;
	} vortex;

	union ParticleInfo
	{
		/* This struct holds the state of the particle when 
		   it's being update (it's still alive).*/
		struct ParticleState
		{
			uint start_life;
			fPoint pos;
			fPoint startVel;
			fPoint endVel;
			fPoint currentVel;
			float currentSize, startSize, endSize;
			float ageRatio;
			float angle;
			double startRotSpeed;
			double curr_rot_speed;
			SDL_Rect pRect;
			SDL_Rect rectSize;
			SDL_Color startColor;
			SDL_Color endColor;
			SDL_BlendMode blend_mode;
			float t;

			ParticleState() {}

		} particle_live;

		/* If the particle is dead, then the 'next' member comes 
		   into play and the struct it's not used. This pointer
		   called 'next' holds a pointer to the next available 
		   particle after this one. */
		Particle* next;

		ParticleInfo() {}
	} particle_state;

	public:

	Particle();

	// Initializes new generated particle
	void Init(fPoint pos, float startSpeed, float endSpeed, float angle, double rotSpeed, float startSize, float endSize, uint life, SDL_Rect textureRect, SDL_Color startColor, SDL_Color endColor, SDL_BlendMode blendMode);

	// Generic methods
	void Update(float dt);
	void Draw();
	bool IsAlive();

	// Particle pointer methods
	Particle* GetNext();
	void SetNext(Particle* next);

	float InterpolateBetweenRange(float min, float timeStep, float max);

	// Adds a vortex to the system
	void AddVortex(fPoint pos, float speed, float scale);

	/* Calculates particle position considering its velocity
	   and if there's a vortex in the system */
	void CalculateParticlePos(float dt);

	SDL_Color LerpColor(SDL_Color src_color, SDL_Color trg_color, float time_step);
};

#endif