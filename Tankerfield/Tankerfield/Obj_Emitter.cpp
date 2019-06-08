#include "Obj_Emitter.h"
#include <time.h>


Obj_Emitter::Obj_Emitter(fPoint pos, EmitterData data)
{ 
	srand(time(NULL));
	
	// Particles size and movement
	this->angleRange = data.angleRange;
	this->startSpeed = data.startSpeed;
	this->endSpeed = data.endSpeed;
	this->startSize = data.startSize;
	this->endSize = data.endSize;
	this->pos = pos;
	this->rotSpeed = data.rotSpeed;

	// Particle emission calculations
	this->emitNumber = data.emitNumber;
	this->emitVariance = data.emitVariance;
	this->maxParticleLife = data.maxParticleLife;
	maxParticlesPerFrame = data.emitNumber + data.emitVariance;

	poolSize = maxParticlesPerFrame * (maxParticleLife + 1u);

	emitterPool = new ParticlePool(this);

	// Color and render properties
	this->textureRect = data.textureRect;
	this->startColor = data.startColor;
	this->endColor = data.endColor;
	this->blendMode = data.blendMode;
	timeStep = 1.0f / (float)maxParticleLife;

	// Emission properties
	active = true;
	lifetime = data.lifetime;

	// Random control parameters
	rotSpeedRand = data.rotSpeedRand;
	startSpeedRand = data.startSpeedRand;
	endSpeedRand = data.endSpeedRand;
	emitVarianceRand = data.emitVarianceRand;
	lifeRand = data.lifeRand;
	startSizeRand = data.startSizeRand;
	endSizeRand = data.endSizeRand;

	if (this->lifetime != -1.0f && this->lifetime > 0.0f)
		lifeTimer.Start();
}

Obj_Emitter::~Obj_Emitter()
{
	delete emitterPool;
	emitterPool = nullptr;
}

void Obj_Emitter::Update(float dt)
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

			emitterPool->Generate(pos, tmpStartSpeed, tmpEndSpeed, randAngle, randRotSpeed, randRadius, endSize, maxParticleLife, textureRect, startColor, endColor, blendMode);
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
		to_destroy = true;
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

void Obj_Emitter::MoveEmitter(fPoint newPos)
{
	pos = newPos;
}

fPoint Obj_Emitter::GetEmitterPos() const
{
	return pos;
}