#include "Particle.h"
#include "App.h"
#include "M_Render.h"
#include "ParticleSystem.h"
#include "Defs.h"


Particle::Particle():life(0) { }

void Particle::Init(fPoint pos, float startSpeed, float endSpeed, float angle, double rotSpeed, float startSize, float endSize, uint life, SDL_Rect textureRect, SDL_Color startColor, SDL_Color endColor, SDL_BlendMode blendMode)
{
	// Movement properties
	particle_state.particle_live.pos = pos;
	particle_state.particle_live.startVel.x = startSpeed * cos(DEGTORAD * angle);
	particle_state.particle_live.startVel.y = -startSpeed * sin(DEGTORAD * angle);
	particle_state.particle_live.endVel.x = endSpeed * cos(DEGTORAD * angle);
	particle_state.particle_live.endVel.y = -endSpeed * sin(DEGTORAD * angle);
	particle_state.particle_live.startRotSpeed = rotSpeed;
	particle_state.particle_live.curr_rot_speed = rotSpeed;

	// Life properties
	this->life = particle_state.particle_live.start_life = life;
	particle_state.particle_live.currentSize = particle_state.particle_live.startSize = startSize;
	particle_state.particle_live.endSize = endSize;
	particle_state.particle_live.t = 0.0f;

	// Color properties
	particle_state.particle_live.startColor = startColor;
	particle_state.particle_live.endColor = endColor;
	particle_state.particle_live.blend_mode = blendMode;
	particle_state.particle_live.pRect = particle_state.particle_live.rectSize = textureRect;

	// Add vortex to the system (optional and only one is allowed)
	 AddVortex({ 250.0f, 200.0f }, 10.0f, 30.0f);
}

void Particle::Update(float dt)
{
	// Age ratio is used to interpolate between particle properties
	particle_state.particle_live.ageRatio = (float)life / (float)particle_state.particle_live.start_life;

	// Particle size interpolation
	particle_state.particle_live.currentSize = InterpolateBetweenRange(particle_state.particle_live.startSize, particle_state.particle_live.t, particle_state.particle_live.endSize);

	// Particle speed interpolation
	particle_state.particle_live.currentVel.x = InterpolateBetweenRange(particle_state.particle_live.startVel.x, particle_state.particle_live.t, particle_state.particle_live.endVel.x);
	particle_state.particle_live.currentVel.y = InterpolateBetweenRange(particle_state.particle_live.startVel.y, particle_state.particle_live.t, particle_state.particle_live.endVel.y);

	// Assign new size to particle rect
	particle_state.particle_live.rectSize.w = particle_state.particle_live.rectSize.h = particle_state.particle_live.currentSize;

	// Calculating new particle position
	particle_state.particle_live.pos.x += particle_state.particle_live.currentVel.x * dt;
	particle_state.particle_live.pos.y += particle_state.particle_live.currentVel.y * dt;

	// BONUS CODE. Uncomment to unchain vortices
	//CalculateParticlePos(dt);

	// Decrementing particle life
	life--;
}

SDL_Color Particle::LerpColor(SDL_Color src_color, SDL_Color trg_color, float time_step)
{
	return { (Uint8)(src_color.r + (trg_color.r - src_color.r) * time_step),
			 (Uint8)(src_color.g + (trg_color.g - src_color.g) * time_step),
			 (Uint8)(src_color.b + (trg_color.b - src_color.b) * time_step),
			 (Uint8)(src_color.a + (trg_color.a - src_color.a) * time_step) };
}

void Particle::Draw()
{
	// Calculations to determine the current center of particle texture
	SDL_Rect tmp_rect = { (int)particle_state.particle_live.startSize, (int)particle_state.particle_live.startSize };
	float center_x = particle_state.particle_live.pos.x + ((tmp_rect.w - particle_state.particle_live.rectSize.w) * 0.5f);
	float center_y = particle_state.particle_live.pos.y + ((tmp_rect.h - particle_state.particle_live.rectSize.h) * 0.5f);

	// Color interpolation, only if the particle has enough life
	SDL_Color curr_color;
	if (particle_state.particle_live.start_life > MIN_LIFE_TO_INTERPOLATE)
	{
		curr_color = LerpColor(
			particle_state.particle_live.startColor,
			particle_state.particle_live.endColor,
			particle_state.particle_live.t);
	}

	// Blitting particle on screen
	app->render->BlitParticle(
		app->objectmanager->particle_system.GetParticleAtlas(),
		(int)center_x,
		(int)center_y,
		&particle_state.particle_live.pRect,
		&particle_state.particle_live.rectSize,
		curr_color,
		particle_state.particle_live.blend_mode,
		1.0f,
		particle_state.particle_live.curr_rot_speed);

	// Calculating new rotation according to rotation speed
	particle_state.particle_live.curr_rot_speed += particle_state.particle_live.startRotSpeed;

	// Time step increment to interpolate colors
	particle_state.particle_live.t += (1.0f / (float)particle_state.particle_live.start_life);

	if (particle_state.particle_live.t >= 1.0f)
	{
		particle_state.particle_live.t = 0.0f;
	}
}

bool Particle::IsAlive()
{
	return life > 0;
}

Particle* Particle::GetNext()
{
	return particle_state.next;
}

void Particle::SetNext(Particle* next)
{
	particle_state.next = next;
}

float Particle::InterpolateBetweenRange(float min, float timeStep, float max)
{
	return min + (max - min) * timeStep;
}

void Particle::AddVortex(fPoint pos, float speed, float scale)
{
	vortex.pos = pos;
	vortex.speed = speed;
	vortex.scale = scale;
}

void Particle::CalculateParticlePos(float dt)
{
	float dx = particle_state.particle_live.pos.x - vortex.pos.x;
	float dy = particle_state.particle_live.pos.y - vortex.pos.y;
	float vx = -dy * vortex.speed;
	float vy = dx * vortex.speed;
	float factor = 1.0f / (1.0f + (dx * dx + dy * dy) / vortex.scale);

	particle_state.particle_live.pos.x += (vx - particle_state.particle_live.currentVel.x) * factor + particle_state.particle_live.currentVel.x * dt;
	particle_state.particle_live.pos.y += (vy - particle_state.particle_live.currentVel.y) * factor + particle_state.particle_live.currentVel.y * dt;
}