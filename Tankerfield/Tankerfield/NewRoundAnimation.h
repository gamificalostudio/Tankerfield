#ifndef __NEW_ROUND_ANIMATION_H__
#define __NEW_ROUND_ANIMATION_H__

#define NEW_ROUND_PARTICLE_NUM 50

#include "Point.h"
#include "Timer.h"
#include "SDL/include/SDL_pixels.h"

class UI_Image;

struct NewRoundUIParticles
{
	UI_Image * ui_image = nullptr;
	fPoint direction = { 0.f, 0.f };
	float curr_scale = 1.f;
	float speed = 0.f;
	float speed_squared = 0.f;
	bool reached_target = false;
	float alpha_speed = 0.f;
};

struct NewRoundAnimation
{
public:
	void Start();
	bool Update(float dt);

	void PrepareColorTransition();

private:
	void CreateNewRoundParticles();
	void PrepareNewRoundUIParticles();
	void UpdateNewRoundUIParticles(float dt);
	void ReduceCenterEnergyAlpha(float dt);

private:
	//New round animation
	NewRoundUIParticles particles[NEW_ROUND_PARTICLE_NUM];
	float max_particle_scale = 0.f;

	float min_particle_speed = 0.f;
	float max_particle_speed = 0.f;

	float min_particle_alpha_speed = 0.f;
	float max_particle_alpha_speed = 0.f;

	//If for any error, the particles didn't reach the target, this timer will make sure that you get to the next round
	Timer particles_timer;
	uint max_particle_time = 0u;

	int particles_reached_trg = 0;

	UI_Image * center_energy = nullptr;//The white image over the rhombus on the center of the screen
	float center_energy_alpha_fill_amount = 0.f;

	//variables used to not lose decimals while transitioning color
	float color_r = 0.f;
	float color_g = 0.f;
	float color_b = 0.f;
	float color_a = 0.f;

	float r_increment = 0.f;
	float g_increment = 0.f;
	float b_increment = 0.f;
	float a_increment = 0.f;

	SDL_Color source_color = { 0, 0, 0, 255 };
	SDL_Color target_color = { 0, 0, 0, 255 };

	float color_transition_time = 0.f;
	Timer color_transition_timer;

	UI_Image * heal_particle[2] = { nullptr };
	float heal_particle_speed = 0.f;

	float center_energy_reduce_alpha_speed = 0.f;
	uint center_energy_alpha_start_heal = 0u;

	//TODO: Create Start method (fill variables)
	//TODO: Create void Reset method
	//TODO: Put methods in here

	enum class HEAL_PARTICLE
	{
		LEFT,
		RIGHT,
		MAX
	};

	enum class NEW_ROUND_ANIMATION_PHASE
	{
		PARTICLES,
		COLOR_TRANSITION,
		REDUCE_ALPHA,
		HEAL,
		IN_ROUND,
	};

	NEW_ROUND_ANIMATION_PHASE phase = NEW_ROUND_ANIMATION_PHASE::PARTICLES;
};

#endif
