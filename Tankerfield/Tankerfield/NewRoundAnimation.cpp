#include "NewRoundAnimation.h"
#include "Rect.h"
#include "Point.h"
#include "M_Window.h"
#include "App.h"
#include "UI_Element.h"
#include "UI_Image.h"
#include "Log.h"

void NewRoundAnimation::Start()
{
	max_particle_scale = 3.f;
	min_particle_speed = -120.f;
	max_particle_speed = -240.f;

	min_particle_alpha_speed = 30.f;
	max_particle_alpha_speed = 120.f;

	CreateNewRoundParticles();
	PrepareNewRoundUIParticles();

	fRect screen = app->win->GetWindowRect();
	UI_ImageDef image_def({ 1725, 1129, 385, 385 });
	center_energy = app->ui->CreateImage({ screen.w * 0.5f, screen.h * 0.5f }, image_def);
	center_energy->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	center_energy->alpha = 0.f;
	center_energy_alpha_fill_amount = 255.f / NEW_ROUND_PARTICLE_NUM;
	max_particle_time = 10000u;//10 seconds max time to transition for all the particles
	time_to_transition = 10.f;

	source_color = { 255, 255, 255, 255 };//White
	target_color = {  75, 180,   0, 255 };//Same green as the health bars in full life
}

bool NewRoundAnimation::Update(float dt)
{
	switch (phase)
	{

	case NEW_ROUND_ANIMATION_PHASE::PARTICLES:
	{
		UpdateNewRoundUIParticles(dt);

		if (particles_reached_trg == NEW_ROUND_PARTICLE_NUM ||
			particles_timer.Read() > max_particle_time)
		{
			phase = NEW_ROUND_ANIMATION_PHASE::PREPARE_COLOR_TRANSITION;
		}
	}break;

	case NEW_ROUND_ANIMATION_PHASE::PREPARE_COLOR_TRANSITION:
	{
		r_increment = (target_color.r - source_color.r) / time_to_transition;
		g_increment = (target_color.g - source_color.g) / time_to_transition;
		b_increment = (target_color.b - source_color.b) / time_to_transition;
		a_increment = (target_color.a - source_color.a) / time_to_transition;
		color_transition_timer.Start();
		phase = NEW_ROUND_ANIMATION_PHASE::COLOR_TRANSITION;
	}break;

	case::NEW_ROUND_ANIMATION_PHASE::COLOR_TRANSITION:
	{
		color_r += r_increment * dt;
		color_g += g_increment * dt;
		color_b += b_increment * dt;
		color_a += a_increment * dt;

		center_energy->color_mod.r = (uint)color_r;
		center_energy->color_mod.g = (uint)color_g;
		center_energy->color_mod.b = (uint)color_b;
		center_energy->color_mod.a = (uint)color_a;

		if (color_transition_timer.ReadSec() > time_to_transition)
		{
			phase = NEW_ROUND_ANIMATION_PHASE::HEAL;
		}
	}break;

	case NEW_ROUND_ANIMATION_PHASE::HEAL:
	{

	}break;

	//case NEW_ROUND_ANIMATION_PHASE::IN_ROUND: Doesn't do anything, simply returns true

	default:
	{
		LOG("Animation case not set correctly.");
	}break;

	}

	return true;
}

void NewRoundAnimation::UpdateNewRoundUIParticles(float dt)
{
	fRect screen = app->win->GetWindowRect();
	fPoint target_pos = { screen.w * 0.5f, screen.h * 0.5f };
	for (int i = 0; i < NEW_ROUND_PARTICLE_NUM; ++i)
	{
		if (!particles[i].reached_target)
		{
			if (particles[i].ui_image->position.DistanceNoSqrt(target_pos) <= dt * particles[i].speed_squared * dt
				/*new_round_ui_particles[i].ui_image->position.DistanceTo(target_pos) <= abs(new_round_ui_particles[i].speed * dt)*/)//It's the same but the above one is more optimized because it doens't calculate a square root
			{
				//INFO: Reach the target
				particles[i].ui_image->SetPos(target_pos);
				particles[i].reached_target = true;
				particles[i].ui_image->SetState(ELEMENT_STATE::HIDDEN);
				center_energy->alpha += center_energy_alpha_fill_amount;
				++particles_reached_trg;
			}
			else
			{
				//INFO: Move to the target
				particles[i].ui_image->SetPos(particles[i].ui_image->position + particles[i].direction * particles[i].speed * dt);
				if (particles[i].ui_image->alpha + particles[i].alpha_speed * dt >= 255.f)
				{
					particles[i].ui_image->alpha = 255.f;
				}
				else
				{
					particles[i].ui_image->alpha += particles[i].alpha_speed * dt;
				}
			}
		}
	}
}

void NewRoundAnimation::PrepareNewRoundUIParticles()
{
	fRect screen = app->win->GetWindowRect();
	fPoint target_pos = { screen.w * 0.5f, screen.h * 0.5f };
	for (int i = 0; i < NEW_ROUND_PARTICLE_NUM; ++i)
	{
		fPoint position;
		do
		{
			position = fPoint(
				static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / screen.w)),
				static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / screen.h)));
		} while (position == target_pos);
		particles[i].ui_image->SetPos(position);
		particles[i].direction = position - target_pos;
		particles[i].direction.Normalize();
		particles[i].curr_scale = rand() % (int)max_particle_scale;
		particles[i].reached_target = false;
		particles[i].ui_image->SetState(ELEMENT_STATE::VISIBLE);
		particles[i].ui_image->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
		particles[i].ui_image->alpha = 0.f;
		particles[i].alpha_speed = min_particle_alpha_speed + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_particle_alpha_speed - min_particle_alpha_speed)));
		particles[i].speed = min_particle_speed + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_particle_speed - min_particle_speed)));
		particles[i].speed_squared = particles[i].speed * particles[i].speed;
	}
	particles_reached_trg = 0;
}

void NewRoundAnimation::CreateNewRoundParticles()
{
	UI_ImageDef image_def({ 1725, 1514, 55, 55 });
	fPoint default_pos(0.f, 0.f);
	for (int i = 0; i < NEW_ROUND_PARTICLE_NUM; ++i)
	{
		particles[i].ui_image = app->ui->CreateImage(default_pos, image_def);
		particles[i].ui_image->SetState(ELEMENT_STATE::HIDDEN);
	}
}