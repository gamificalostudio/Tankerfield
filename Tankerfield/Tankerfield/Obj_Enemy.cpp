#include <assert.h>

#include "M_UI.h"
#include "UI_Label.h"
#include "Obj_Enemy.h"
#include "M_Collision.h"
#include "Bullet_Laser.h"
#include "M_Audio.h"
#include "App.h"
#include "M_PickManager.h"
#include "M_Render.h"
#include "M_Scene.h"
#include "M_Pathfinding.h"
#include "M_AnimationBank.h"



Obj_Enemy::Obj_Enemy(fPoint pos) : Object(pos)
{
	damaged_sprite_time = 150;
	//colision si se puede desactivar

	check_path_time = 1.f; // 1s

	range_pos.center = pos_map;
	range_pos.radius = 0.5f;

	
	path_timer.Start();
}

bool Obj_Enemy::Update(float dt)
{
	Movement(dt);
	Attack();
	if (in_white)
	{
		ChangeTexture();
	}
	if (life_collider != nullptr)
		life_collider->SetPosToObj();
	return true;
}

void Obj_Enemy::ChangeTexture()
{
	if (damaged_sprite_timer.Read() > damaged_sprite_time)
	{
		curr_tex = last_texture;
		in_white = false;
	}
	
}

void Obj_Enemy::Attack()
{
	if (life > 0 && app->scene->stat_of_wave != WaveStat::NO_TYPE)
	{
		if (target != nullptr
			&& target->coll->GetTag() == Collider::TAG::PLAYER
			&& pos_map.DistanceNoSqrt(target->pos_map) < attack_range_squared
			&& perf_timer.ReadMs() > (double)attack_frequency)
		{
			curr_anim = &attack;
			target->SetLife(target->GetLife() - attack_damage);
			perf_timer.Start();
			app->audio->PlayFx(sfx_attack);
		}

		if (curr_anim == &attack
			&& curr_anim->Finished())
		{
			curr_anim = &idle;//walk?
			attack.Reset();
		}
	}
}
void Obj_Enemy::Movement(float &dt)
{
	switch (state)
	{
	case ENEMY_STATE::IDLE:
	{
		Idle();
	}
	break;
	case ENEMY_STATE::SPAWN:
	{

		Spawn(dt); //rewrite in every enemie. originally is empty.

	}
	break;
	case ENEMY_STATE::GET_PATH:
	{
		GetPath();

		path_timer.Start();
	}
	break;
	case ENEMY_STATE::MOVE:
	{
		int retflag = Move(dt);
		if (retflag == 2) break;

	}
	break;
	case ENEMY_STATE::RECHEAD_POINT:
	{
		RecheadPoint();

	}
	break;

	case ENEMY_STATE::GET_TELEPORT_POINT:
	{
		GetTeleportPoint();
	}
	break;
	case ENEMY_STATE::TELEPORT_IN:
	{
		TeleportIn(dt);
	}
	break;
	case ENEMY_STATE::TELEPORT_OUT:
	{
		TeleportOut(dt);
	}
	break;
	case  ENEMY_STATE::DEAD:
	{
		Dead();
	}
	break;
	case ENEMY_STATE::BURN:
	{
		Burn(dt);
	}
		break;
	default:
		assert(true && "The enemy have no state");
		break;
	}

}

void Obj_Enemy::RecheadPoint()
{
	move_vect.SetToZero();
	if (path.size() > 0)
	{
		next_pos = (fPoint)(*path.begin());
		next_pos += {0.5f, 0.5f};
		UpdateVelocity();
		update_velocity_vec.Start();
		state = ENEMY_STATE::MOVE;
	}
	else
		state = ENEMY_STATE::GET_PATH;
}

void Obj_Enemy::Dead()
{
	if (curr_anim != &death)
	{
		curr_anim = &death;
		app->audio->PlayFx(sfx_death);
		if (coll != nullptr)
		{
			coll->Destroy();
			coll = nullptr;
		}
	}
	else
	{
		if (death.Finished())
		{
			to_remove = true;
			
		}
	}
}

void Obj_Enemy::Idle()
{
	path.clear();
	move_vect.SetToZero();
	target = app->objectmanager->GetNearestTank(pos_map, detection_range);
	if (target != nullptr)
	{
		state = ENEMY_STATE::GET_PATH;
	}
	else
	{
		curr_anim = &idle;
	}
}

int Obj_Enemy::Move(float & dt)
{
	if (IsOnGoal(next_pos))
	{
		if (path.size() > 0)
			path.erase(path.begin());
		else
			state = ENEMY_STATE::GET_PATH;

		state = ENEMY_STATE::RECHEAD_POINT;
	}

	

	if (update_velocity_vec.ReadSec() > 1)
	{
		UpdateVelocity();
	}


	curr_anim = &walk;

	if (path_timer.ReadSec() >= check_path_time)
		state = ENEMY_STATE::GET_PATH;

	UpdatePos(dt);
	return 0;
}



void Obj_Enemy::GetPath()
{
	path.clear();
	move_vect.SetToZero();
	target = app->objectmanager->GetNearestTank(pos_map, detection_range);
	if (target != nullptr)
	{

		if (app->pathfinding->CreatePath((iPoint)pos_map, (iPoint)target->pos_map) != -1)
		{
			path.clear();
			path = *app->pathfinding->GetLastPath();
			if (path.size() > 0)
				path.erase(path.begin());
			next_pos = (fPoint)(*path.begin());
			UpdateVelocity();

			state = ENEMY_STATE::MOVE;
		}

	}
	else
	{
		state = ENEMY_STATE::IDLE;
	}
}

bool Obj_Enemy::Draw(float dt, Camera * camera)
{

	app->render->BlitScaled(
		curr_tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&frame,
		scale,
		scale);

	

	return true;
}

bool Obj_Enemy::Start()
{
	burn_texture = app->tex->Load(app->anim_bank->animations_xml_node.child("burn").child("animations").child("burn").attribute("texture").as_string());

	burn.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("burn").child("animations").child("burn"));
	dying_burn.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("burn").child("animations").child("dying_burn"));
	return true;
}

bool Obj_Enemy::CleanUp()
{
	app->scene->ReduceNumEnemies();

	to_remove = true;
	return true;
}

inline void Obj_Enemy::UpdateVelocity()
{
	fPoint new_move_vec = (fPoint)(next_pos)-pos_map;
	new_move_vec.Normalize();
	if (new_move_vec != move_vect)
	{
		move_vect = new_move_vec;
		angle = atan2(move_vect.y, -move_vect.x)  * RADTODEG - ISO_COMPENSATION;
	}
}

inline void Obj_Enemy::UpdatePos(const float& dt)
{
	pos_map += move_vect * speed * dt;
	range_pos.center = pos_map;
}

void Obj_Enemy::DrawDebug(const Camera* camera)
{
	if (path.size() >= 2)
	{
		for (std::vector<iPoint>::iterator iter = path.begin(); iter != path.end() - 1; ++iter)
		{
			fPoint point1 = { (*iter).x + 0.5F, (*iter).y + 0.5F };
			fPoint point2 = { (*(iter + 1)).x + 0.5F, (*(iter + 1)).y + 0.5F };
			app->render->DrawIsometricLine(point1, point2, { 255,255,255,255 }, camera);
		}
	}

}

inline void Obj_Enemy::Burn(const float & dt)
{
	if (burn_fist_enter)
	{
		curr_anim = &burn;
		fire_damage = life / 3;
		if (burn_texture != nullptr)
			curr_tex = burn_texture;

	
	}
	if (burn_fist_enter || timer_change_direction.ReadSec() >= max_time_change_direction)
	{
		if (life > 0)
		{
			int max_rand = 101;
			int max_rand_double = max_rand * 2;
			float one_divided_by_100 = 0.01f;

			move_vect = { ((rand() % max_rand_double) - max_rand)* one_divided_by_100 ,((rand() % max_rand_double) - max_rand)*one_divided_by_100 };
			move_vect.Normalize();

			angle = atan2(move_vect.y, -move_vect.x)  * RADTODEG - ISO_COMPENSATION;

			timer_change_direction.Start();

			max_time_change_direction = (rand() % max_rand)*one_divided_by_100;
			max_time_change_direction += 0.5f;
			life -= fire_damage;
		}
		else
		{
			curr_anim = &dying_burn;
			if (coll != nullptr)
			{
				coll->Destroy();
				coll = nullptr;
			}
			
		}

		if (burn_fist_enter)
			burn_fist_enter = false;
	}

	if (life > 0)
		UpdatePos(dt);
	else if (curr_anim == &dying_burn && curr_anim->Finished())
	{
		CleanUp();
	}

}


void Obj_Enemy::OnTriggerEnter(Collider * collider)
{
	if (collider->GetTag() == Collider::TAG::BULLET_LASER)
	{
		Laser_Bullet* obj = (Laser_Bullet*)collider->GetObj();
		if (obj->kill_counter < obj->kill_counter_max)		//sometimes in a frame does onCollision more times than it should if the enemies are together before the object is removed.
		{
			life -= collider->damage;

			damaged_sprite_timer.Start();
			last_texture = curr_tex;
			curr_tex = tex_damaged;
			in_white = true;

			if (life <= 0)
			{
				// DROP A PICK UP ITEM 
				app->pick_manager->PickUpFromEnemy(pos_map);
				state = ENEMY_STATE::DEAD;
			}
			else
			{
				app->audio->PlayFx(sfx_hit);
			}


			if (!obj->charged)
			{
				++obj->kill_counter;
				if (obj->kill_counter >= obj->kill_counter_max)
				{
					obj->to_remove = true;

				}
			}
		}

	}
}

void Obj_Enemy::OnTrigger(Collider* collider)
{
	if ((collider->GetTag() == Collider::TAG::BULLET) || (collider->GetTag() == Collider::TAG::FRIENDLY_BULLET))
	{
		life -= collider->damage;
		damaged_sprite_timer.Start();
		curr_tex = tex_damaged;
		in_white = true;
		if (life <= 0)
		{
			app->pick_manager->PickUpFromEnemy(pos_map);
			state = ENEMY_STATE::DEAD;
		}
		else
		{
			app->audio->PlayFx(sfx_hit);
		}
	}
}

bool Obj_Enemy::IsOnGoal(fPoint goal)
{
	return range_pos.IsPointIn(goal);
}