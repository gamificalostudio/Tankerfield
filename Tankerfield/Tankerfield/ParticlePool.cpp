#include "ParticlePool.h"
#include <assert.h>


// This pool constructor sets our particles to available
ParticlePool::ParticlePool(Obj_Emitter* emitter)
{
	pool_size = emitter->GetPoolSize();
	particle_array = new Particle[pool_size];

	first_available = &particle_array[0];
	for (int i = 0; i < pool_size - 1; ++i)
	{
		particle_array[i].SetNext(&particle_array[i + 1]);
	}
	particle_array[pool_size - 1].SetNext(nullptr);
}

ParticlePool::~ParticlePool()
{
	delete[] particle_array;
	particle_array = nullptr;
}

void ParticlePool::Generate(fPoint pos, float startSpeed, float endSpeed, float angle, float rotSpeed, float startSize, float endSize, uint life, SDL_Rect textureRect, SDL_Color startColor, SDL_Color endColor, SDL_BlendMode blendMode)
{
	assert(first_available != nullptr);

	first_available->Init(
		pos,
		startSpeed,
		endSpeed,
		angle,
		rotSpeed,
		startSize,
		endSize,
		life,
		textureRect,
		startColor,
		endColor,
		blendMode);

	first_available = first_available->GetNext();
}

bool ParticlePool::Update(float dt)
{
	bool ret = false;

	for (int i = 0; i < pool_size; ++i)
	{
		if (particle_array[i].IsAlive())
		{
			particle_array[i].Update(dt);
			particle_array[i].Draw();
			ret = true;
		}
		else
		{
			particle_array[i].SetNext(first_available);
			first_available = &particle_array[i];
		}
	}

	return ret;
}