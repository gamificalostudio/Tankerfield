#include "Obj_Emitter.h"
#include <time.h>
#include "App.h"
#include "M_Map.h"


Obj_Emitter::Obj_Emitter(fPoint pos, EmitterData data) :
	Object(pos),
	// Particles size and movement
	angleRange(data.angleRange),
	startSpeed(data.startSpeed),
	endSpeed(data.endSpeed),
	startSize(data.startSize),
	endSize(data.endSize),
	rotSpeed(data.rotSpeed),
	// Particle emission calculations
	emitNumber(data.emitNumber),
	emitVariance(data.emitVariance),
	maxParticleLife(data.maxParticleLife),
	maxParticlesPerFrame(data.emitNumber + data.emitVariance),
	poolSize(maxParticlesPerFrame * (maxParticleLife + 1u)),
	// Color and render properties
	textureRect(data.textureRect),
	startColor(data.startColor),
	endColor(data.endColor),
	blendMode(data.blendMode),
	timeStep(1.0f / (float)maxParticleLife),
	// Emission properties
	active(true),
	lifetime(data.lifetime),
	// Random control parameters
	rotSpeedRand(data.rotSpeedRand),
	startSpeedRand(data.startSpeedRand),
	endSpeedRand(data.endSpeedRand),
	emitVarianceRand(data.emitVarianceRand),
	lifeRand(data.lifeRand),
	startSizeRand(data.startSizeRand),
	endSizeRand(data.endSizeRand)
{ 
	//srand(time(NULL));
	emitterPool = new ParticlePool(this);

	if (lifetime != -1.0f && lifetime > 0.0f)
	{
		lifeTimer.Start();
	}
}

Obj_Emitter::~Obj_Emitter()
{
	delete emitterPool;
	emitterPool = nullptr;
}

bool Obj_Emitter::Update(float dt)
{
	if (active)
	{
		// Particle generation from pool
		
		emissionRate = (int)(emitNumber + emitVariance * RangeRandomNum(emitVarianceRand.x, emitVarianceRand.y));

		for (int i = 1; i <= emissionRate; i++)
		{
			float tmpStartSpeed = startSpeed * RangeRandomNum(startSpeedRand.x, startSpeedRand.y);
			float tmpEndSpeed = endSpeed * RangeRandomNum(endSpeedRand.x, endSpeedRand.y);
			float randAngle = RangeRandomNum(angleRange.x, angleRange.y);
			float randStart= startSize * RangeRandomNum(startSizeRand.x, startSizeRand.y);
			float randEnd = startSize * RangeRandomNum(startSizeRand.x, startSizeRand.y);
			float randRadius = RangeRandomNum(randStart, randEnd);
			double randRotSpeed = rotSpeed * RangeRandomNum(rotSpeedRand.x, rotSpeedRand.y);

			emitterPool->Generate(pos_map, tmpStartSpeed, tmpEndSpeed, randAngle, randRotSpeed, randRadius, endSize, maxParticleLife, textureRect, startColor, endColor, blendMode);
			timeStep += timeStep;
		}
	}

	// Emission timing calculations
	
	if (stopTime > 0.0f && !active)
	{
		emissionTime = 0.0f;
		if (stopTimer.ReadMs() >= stopTime)
		{
			active = true;
			stopTime = 0.0f;
		}
	}
	
	if (emissionTime > 0.0f)
	{
		stopTime = 0.0f;
		if (emissionTimer.ReadMs() >= emissionTime)
		{
			active = false;
			emissionTime = 0.0f;
		}
	}

	if (lifetime > 0.0f)
	{
		if (lifeTimer.ReadMs() >= lifetime)
		{
			active = false;
			lifetime = 0.0f;
		}
	}

	// Updating particles in the pool
	/* NOTE: if lifetime is 0 and last particles have been updated
	then the emitter is automatically destroyed */
	if (!emitterPool->Update(dt) && lifetime == 0.0f)
	{
		to_remove = true;
	}

	return true;
}

bool Obj_Emitter::Draw(Camera * camera)
{
	bool ret = false;

	ret = emitterPool->Draw(camera);

	return ret;
}

float Obj_Emitter::RangeRandomNum(float min, float max)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = max - min;
	float r = random * diff;

	return min + r;
}

int Obj_Emitter::GetPoolSize() const
{
	return poolSize;
}

void Obj_Emitter::StartEmission(double timer)
{
	if (!active)
	{
		active = true;
		emissionTime = timer;
		emissionTimer.Start();
	}
}

void Obj_Emitter::StopEmission(double timer)
{
	if (active)
	{
		active = false;
		stopTime = timer;
		stopTimer.Start();
	}
}

void Obj_Emitter::CalculateDrawVariables()
{
	//INFO: All particles are in map_pos, there is no need to check if they are in isometric as with Object::CalculateDrawVariables

	pos_screen = app->map->MapToScreenF(pos_map);

	for (int i = 0; i < emitterPool->pool_size; ++i)
	{
		if (emitterPool->particle_array[i].IsAlive())
		{
			emitterPool->particle_array[i].particle_state.particle_live.pos_screen = app->map->MapToScreenF(emitterPool->particle_array[i].particle_state.particle_live.pos_map);
		}
	}

	int min_x = 0;
	int max_x = 0;
	int min_y = 0;
	int max_y = 0;

	int particle_min_x = 0;
	int particle_max_x = 0;
	int particle_min_y = 0;
	int particle_max_y = 0;

	bool init_frame_vars = false;
	//Set the values of the first particle

	//TODO: Take into account the scale of the particle

	for (int i = 0; i < emitterPool->pool_size; ++i)
	{
		particle_min_x = emitterPool->particle_array[i].particle_state.particle_live.pos_screen.x - emitterPool->particle_array[i].particle_state.particle_live.rect.w * 0.5f;
		particle_max_x = emitterPool->particle_array[i].particle_state.particle_live.pos_screen.x + emitterPool->particle_array[i].particle_state.particle_live.rect.w * 0.5f;
		particle_min_y = emitterPool->particle_array[i].particle_state.particle_live.pos_screen.y - emitterPool->particle_array[i].particle_state.particle_live.rect.h * 0.5f;
		particle_max_y = emitterPool->particle_array[i].particle_state.particle_live.pos_screen.y + emitterPool->particle_array[i].particle_state.particle_live.rect.h * 0.5f;

		if (emitterPool->particle_array[i].IsAlive())
		{
			if (!init_frame_vars)
			{
				min_x = particle_min_x;
				max_x = particle_max_x;
				min_y = particle_min_y,
				max_y = particle_max_y;
				init_frame_vars = true;
			}
			else
			{
				if (particle_min_x < min_x)
				{
					min_x = particle_min_x;
				}
				if (particle_max_x > max_x)
				{
					max_x = particle_max_x;
				}
				if (particle_min_y < min_y)
				{
					min_y = particle_min_y;
				}
				if (particle_max_y > max_y)
				{
					max_y = particle_max_y;
				}
			}
		}
	}
	frame = { min_x, min_y, max_x - min_x , max_y - min_y };
	//Workaround to make min_x and min_y be the positions from which the rectangle (show with F3) is drawn
	//We're substracting draw offset to pos_screen when we sort
	//So we make pos_screen.x - pos_screen.x + min_x = min_x
	//And that's how we make that the sorting rect has min_x and min_y as its position
	draw_offset.create(pos_screen.x - min_x, pos_screen.y - min_y);

}
