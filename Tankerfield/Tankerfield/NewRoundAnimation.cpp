#include "NewRoundAnimation.h"
#include "Rect.h"
#include "Point.h"
#include "M_Window.h"
#include "App.h"
#include "UI_Element.h"
#include "UI_Image.h"
#include "Log.h"
#include "M_Scene.h"
#include "General_HUD.h"
#include "UI_Label.h"
#include "M_Audio.h"

void NewRoundAnimation::Start()
{
	max_particle_scale = 3.f;
	min_particle_speed = -120.f;
	max_particle_speed = -240.f;

	min_particle_alpha_speed = 30.f;
	max_particle_alpha_speed = 120.f;

	CreateNewRoundParticles();

	fRect screen = app->win->GetWindowRect();

	UI_ImageDef image_def({ 1725, 1129, 385, 385 });
	center_energy = app->ui->CreateImage({ screen.w * 0.5f, screen.h * 0.5f }, image_def);
	center_energy->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
	center_energy->alpha = 0.f;
	center_energy_alpha_fill_amount = 255.f / NEW_ROUND_PARTICLE_NUM;
	max_particle_time = 10000u;//10 seconds max time to transition for all the particles
	color_transition_time = 5.75f;

	source_color = { 255, 255, 255, 255 };//White
	target_color = { 75, 180,   0, 255 };//Same green as the health bars in full life

	color_r = source_color.r;
	color_g = source_color.g;
	color_b = source_color.b;
	color_a = source_color.a;

	r_increment = (target_color.r - source_color.r) / color_transition_time;
	g_increment = (target_color.g - source_color.g) / color_transition_time;
	b_increment = (target_color.b - source_color.b) / color_transition_time;
	a_increment = (target_color.a - source_color.a) / color_transition_time;

	image_def.sprite_section = { 1725, 1514, 55, 55 };
	for (int i = 0; i < (int)HEAL_PARTICLE::MAX; ++i)
	{
		heal_particle[i] = app->ui->CreateImage({ screen.w * 0.5f, screen.h * 0.5f }, image_def);
		heal_particle[i]->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
		heal_particle[i]->SetMenuState(ELEMENT_STATE::HIDDEN);
		heal_particle[i]->color_mod = target_color;
	}

	heal_particle_speed = 3.5f;

	center_energy->SetParent(app->scene->general_gui->round_element);
	heal_particle[0]->SetParent(center_energy);
	heal_particle[1]->SetParent(center_energy);
	app->scene->general_gui->round_number_label->SetParent(center_energy);

	center_energy_reduce_alpha_speed = 500.f;

	life_increase = 10;

	center_energy_alpha_start_heal = 75;

	particle_reach_sfx = app->audio->LoadFx("audio/Fx/new_wave/sd_0.wav");
	change_color_sfx = app->audio->LoadFx("audio/Fx/new_wave/336741__steshystesh__spaceship-whoosh-2.wav");
	throw_healing_balls_sfx = app->audio->LoadFx("audio/Fx/new_wave/87043__runnerpack__weapappear.wav");
}

void NewRoundAnimation::CreateNewRoundParticles()
{
	UI_ImageDef image_def({ 1725, 1514, 55, 55 });
	fPoint default_pos(0.f, 0.f);
	for (int i = 0; i < NEW_ROUND_PARTICLE_NUM; ++i)
	{
		particles[i].ui_image = app->ui->CreateImage(default_pos, image_def);
		particles[i].ui_image->SetMenuState(ELEMENT_STATE::HIDDEN);
	}
}

void NewRoundAnimation::PrepareAnimation()
{
	PrepareNewRoundUIParticles();

	phase = NEW_ROUND_ANIMATION_PHASE::PARTICLES;
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
		particles[i].ui_image->SetMenuState(ELEMENT_STATE::VISIBLE);
		particles[i].ui_image->SetPivot(Pivot::X::CENTER, Pivot::Y::CENTER);
		particles[i].ui_image->alpha = 0.f;
		particles[i].alpha_speed = min_particle_alpha_speed + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_particle_alpha_speed - min_particle_alpha_speed)));
		particles[i].speed = min_particle_speed + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max_particle_speed - min_particle_speed)));
		particles[i].speed_squared = particles[i].speed * particles[i].speed;
	}
	particles_reached_trg = 0;
	particles_timer.Start();
	center_energy->SetMenuState(ELEMENT_STATE::VISIBLE);
}

bool NewRoundAnimation::Update(float dt)
{
	switch (phase)
	{

	case NEW_ROUND_ANIMATION_PHASE::PARTICLES:
	{
		UpdateNewRoundUIParticles(dt);

		if (particles_reached_trg == NEW_ROUND_PARTICLE_NUM)
		{
			app->audio->PlayFx(change_color_sfx);
			PrepareColorTransition();
			++app->scene->round;
			app->scene->general_gui->SetRoundNumber(app->scene->round);
			phase = NEW_ROUND_ANIMATION_PHASE::COLOR_TRANSITION;
		}
		//If for any reasons all particles don't reach the center
		else if (particles_timer.Read() > max_particle_time)
		{
			app->audio->PlayFx(change_color_sfx);
			center_energy->alpha = 255.f;
			for (int i = 0; i < NEW_ROUND_PARTICLE_NUM; ++i)
			{
				particles[i].ui_image->SetMenuState(ELEMENT_STATE::HIDDEN);
			}
			PrepareColorTransition();
			++app->scene->round;
			app->scene->general_gui->SetRoundNumber(app->scene->round);
			phase = NEW_ROUND_ANIMATION_PHASE::COLOR_TRANSITION;
		}
	}break;

	case NEW_ROUND_ANIMATION_PHASE::COLOR_TRANSITION:
	{
		color_r += r_increment * dt;
		color_g += g_increment * dt;
		color_b += b_increment * dt;
		color_a += a_increment * dt;

		center_energy->color_mod.r = (uint)color_r;
		center_energy->color_mod.g = (uint)color_g;
		center_energy->color_mod.b = (uint)color_b;
		center_energy->color_mod.a = (uint)color_a;

		if (color_transition_timer.ReadSec() > color_transition_time)
		{
			for (int i = 0; i < (int)HEAL_PARTICLE::MAX; ++i)
			{
				heal_particle[i]->SetMenuState(ELEMENT_STATE::VISIBLE);
			}
			phase = NEW_ROUND_ANIMATION_PHASE::REDUCE_ALPHA;
		}
	}break;

	case NEW_ROUND_ANIMATION_PHASE::REDUCE_ALPHA: {
		ReduceCenterEnergyAlpha(dt);
		if (center_energy->alpha < center_energy_alpha_start_heal)
		{
			app->audio->PlayFx(throw_healing_balls_sfx);
			phase = NEW_ROUND_ANIMATION_PHASE::HEAL;
		}
	}break;

	case NEW_ROUND_ANIMATION_PHASE::HEAL:
	{
		ReduceCenterEnergyAlpha(dt);
		fRect screen = app->win->GetWindowRect();
		heal_particle[(int)HEAL_PARTICLE::LEFT]->SetPos(heal_particle[(int)HEAL_PARTICLE::LEFT]->position - fPoint(heal_particle_speed, 0.f));
		heal_particle[(int)HEAL_PARTICLE::RIGHT]->SetPos(heal_particle[(int)HEAL_PARTICLE::RIGHT]->position + fPoint(heal_particle_speed, 0.f));
		if (heal_particle[(int)HEAL_PARTICLE::LEFT]->position.x < 0.f
			&& heal_particle [(int)HEAL_PARTICLE::RIGHT]->position.x > screen.w)
		{
			fRect screen = app->win->GetWindowRect();
			for (int i = 0; i < (int)HEAL_PARTICLE::MAX; ++i)
			{
				heal_particle[i]->SetMenuState(ELEMENT_STATE::HIDDEN);
				heal_particle[i]->SetPos({ screen.w * 0.5f, screen.h * 0.5f });
			}
			center_energy->color_mod = source_color;
			center_energy->alpha = 0.f;
			center_energy->SetMenuState(ELEMENT_STATE::HIDDEN);
			HealPlayers();
			app->scene->game_state = GAME_STATE::ENTER_IN_WAVE;
			phase = NEW_ROUND_ANIMATION_PHASE::WAITING;
		}
	}break;

	case NEW_ROUND_ANIMATION_PHASE::WAITING:
	{
		//Doesn nothing (shouldn't call the Update either so there is no problem)
	}break;

	//case NEW_ROUND_ANIMATION_PHASE::IN_ROUND: Doesn't do anything, simply returns true

	default:
	{
		LOG("Animation case not set correctly.");
	}break;

	}

	return true;
}

void NewRoundAnimation::HealPlayers()
{
	for (int i = 0; i < 4; ++i)
	{
		app->objectmanager->obj_tanks[i]->IncreaseLife(life_increase);
	}
}

void NewRoundAnimation::ReduceCenterEnergyAlpha(float dt)
{
	float final_alpha = center_energy->alpha - center_energy_reduce_alpha_speed * dt;
	if (final_alpha < 0.f)
	{
		center_energy->alpha = 0.f;
	}
	else
	{
		center_energy->alpha = final_alpha;
	}
}

void NewRoundAnimation::PrepareColorTransition()
{
	color_r = source_color.r;
	color_g = source_color.g;
	color_b = source_color.b;
	color_a = source_color.a;
	color_transition_timer.Start();
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
				app->audio->PlayFx(particle_reach_sfx);
				particles[i].ui_image->SetPos(target_pos);
				particles[i].reached_target = true;
				particles[i].ui_image->SetMenuState(ELEMENT_STATE::HIDDEN);
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