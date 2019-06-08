#ifndef __PARTICLE_POOL_H__
#define __PARTICLE_POOL_H__

#include "Particle.h"
#include "Emitter.h"
#include "Point.h"
#include <vector>

class Obj_Emitter;

class ParticlePool
{
private:
	
	int pool_size = 0;
	Particle * first_available	= nullptr;
	Particle * particle_array	= nullptr;


public:

	ParticlePool(Obj_Emitter* emitter);
	virtual ~ParticlePool();

	//Generates a new particle each time it's called
	void Generate(fPoint pos, float startSpeed, float endSpeed, float angle, float rotSpeed, float startSize, float endSize, uint life, SDL_Rect textureRect, SDL_Color startColor, SDL_Color endColor, SDL_BlendMode blendMode);
	
	// Update (move and draw) particles in the pool. If there are no particles alive returns false
	bool Update(float dt);
};

#endif