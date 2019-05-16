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
	ChangeTexture();
	Oiled();

	return true;
}

void Obj_Enemy::ChangeTexture()
{
	if ((damaged_sprite_timer.Read() > damaged_sprite_time))
	{
		if (oiled == false) 
		{
			curr_tex = tex;
		}
		else
		{
			curr_tex = oiled_tex;
		}
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

	pos_map += move_vect * speed * dt;
	range_pos.center = pos_map;
	curr_anim = &walk;

	if (path_timer.ReadSec() >= check_path_time)
		state = ENEMY_STATE::GET_PATH;

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

bool Obj_Enemy::CleanUp()
{
	app->scene->ReduceNumEnemies();
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

void Obj_Enemy::OnTriggerEnter(Collider * collider)
{
	if (collider->GetTag() == Collider::TAG::BULLET_LASER)
	{
		Laser_Bullet* obj = (Laser_Bullet*)collider->GetObj();
		if (obj->kill_counter < obj->kill_counter_max)		//sometimes in a frame does onCollision more times than it should if the enemies are together before the object is removed.
		{
			life -= collider->damage;

			damaged_sprite_timer.Start();
			curr_tex = tex_damaged;

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

	else if (collider->GetTag() == Collider::TAG::BULLET_OIL)
	{
		life -= collider->damage;
		oiled = true;
		oiled_timer.Start();
		damaged_sprite_timer.Start();
		curr_tex = tex_damaged;

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

	else if (collider->GetTag() == Collider::TAG::OIL_POOL)
	{
		oiled = true;
		oiled_timer.Start();
	}
}

bool Obj_Enemy::IsOnGoal(fPoint goal)
{
	return range_pos.IsPointIn(goal);
}


void Obj_Enemy::Oiled()
{
	if (oiled == true)
	{
		speed = original_speed / 2;
	}

	if (oiled_timer.Read() >= 5000)
	{
		oiled = false;
		speed = original_speed;
	}
}