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
#include "M_Map.h"
#include "Obj_ElectroShotAnimation.h"




Obj_Enemy::Obj_Enemy(fPoint pos) : Object(pos)
{
	pugi::xml_node enemie_node = app->config.child("object").child("enemies");
	pugi::xml_node anim_node = app->anim_bank->animations_xml_node;

	tex_electro_dead = app->tex->Load(enemie_node.child("tex_electro_dead").child_value());
	electro_dead.frames = app->anim_bank->LoadFrames(anim_node.child("electro_dead"));

	electocuted = app->audio->LoadFx("audio/Fx/entities/enemies/electrocuted.wav",25);

	damaged_sprite_time = 150;
	//colision si se puede desactivar

	check_path_time = 1.f; // 1s

	range_pos.center = pos_map;
	range_pos.radius = 0.5f;

	times_to_repeat_animation = 3u;
	

	path_timer.Start();

	//TELEPORT VARIABLES =============
		teleport_enemies_max = app->objectmanager->tesla_trooper_info.teleport_max_enemies;
		check_teleport_time = 10; //10s
		teleport_timer.Start();
		portal_animation.frames = app->anim_bank->LoadFrames(app->config.child("object").child("portal").child("animations").child("open"));
		portal_close_anim.frames = app->anim_bank->LoadFrames(app->config.child("object").child("portal").child("animations").child("close"));
		portal_tex = app->tex->Load("textures/Objects/enemies/portal.png");
	
}

Obj_Enemy::~Obj_Enemy()
{
	if (life_collider != nullptr)
	{
		life_collider->Destroy();
		life_collider = nullptr;
	}
}

bool Obj_Enemy::Update(float dt)
{
	Movement(dt);

	if(can_attack)
		Attack();


	if(oiled && state != ENEMY_STATE::BURN)
		Oiled();

	ChangeTexture();

	if (life_collider != nullptr)
		life_collider->SetPosToObj();

	return true;
}

void Obj_Enemy::ChangeTexture()
{
	if (damaged && state != ENEMY_STATE::BURN &&
		state != ENEMY_STATE::STUNNED &&
		state != ENEMY_STATE::STUNNED_CHARGED &&
		!oiled && damaged_sprite_timer.Read() > damaged_sprite_time &&
		curr_tex != tex &&
		bool_electro_dead == false)
	{
		curr_tex = tex;
		damaged = false;
	}
}

void Obj_Enemy::Attack()
{
	if (life > 0 && app->scene->game_state != GAME_STATE::NO_TYPE)
	{
		if (curr_anim == &attack
			&& curr_anim->Finished())
		{
			curr_anim = &idle;//walk?
			attack.Reset();
		}
		else if (target != nullptr
				&& pos_map.DistanceNoSqrt(target->pos_map) <= attack_range_squared
				&& perf_timer.ReadMs() > (double)attack_frequency)
			{
				curr_anim = &attack;
				target->ReduceLife(attack_damage);
				perf_timer.Start();
				app->audio->PlayFx(sfx_attack);
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
		Move(dt);

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
		if (!is_electro_dead)
		{
			Dead();
		}
		else
		{
			ElectroDead();
		}
	}
	break;
	case ENEMY_STATE::STUNNED:
	{
		Stunned();
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
		UpdateMoveVec();
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
		// DROP A PICK UP ITEM 
		app->pick_manager->PickUpFromEnemy(pos_map);
		//curr_tex = tex;
		curr_anim = &death;
		app->audio->PlayFx(sfx_death);
		if (coll != nullptr)
		{
			coll->Destroy();
			coll = nullptr;
		}
		if (life_collider != nullptr)
		{
			life_collider->Destroy();
			life_collider = nullptr;
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

void Obj_Enemy::ElectroDead()
{
	if (curr_anim != &electro_dead)
	{
		// DROP A PICK UP ITEM 
		app->pick_manager->PickUpFromEnemy(pos_map);

		bool_electro_dead = true;
		curr_tex = tex_electro_dead;
		curr_anim = &electro_dead;
		app->audio->PlayFx(sfx_death);
		draw_offset = electrocuted_draw_offset;
		if (coll != nullptr)
		{
			coll->Destroy();
			coll = nullptr;
		}
	}
	else
	{
		if (electro_dead.Finished())
		{
			to_remove = true;
			app->audio->PauseFx(channel_electrocuted);

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

void Obj_Enemy::Move(const float & dt)
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
		UpdateMoveVec();
	}

	if (curr_anim != &attack)
		curr_anim = &walk;

	if (path_timer.ReadSec() >= check_path_time)
		state = ENEMY_STATE::GET_PATH;

	UpdatePos(dt);

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
			UpdateMoveVec();

			state = ENEMY_STATE::MOVE;
		}
		else
		{
			if (teleport_timer.ReadSec() >= check_teleport_time && path.size() == 0)
			{
				state = ENEMY_STATE::GET_TELEPORT_POINT;
				curr_anim = &idle;
			}
			else if (path.size() > 0)
			{
				state = ENEMY_STATE::MOVE;
				curr_anim = &walk;
			}
			else
			{
				state = ENEMY_STATE::IDLE;
				curr_anim = &idle;
				path_timer.Start();
			}
		}
	}
	else
	{
		state = ENEMY_STATE::IDLE;
	}
}

inline void Obj_Enemy::GetTeleportPoint()
{
	move_vect.SetToZero();
	float distance_to_tank = this->pos_map.DistanceManhattan(target->pos_map);
	SpawnPoint* nearest_spawners_points = nullptr;
	float last_distance_to_spawnpoint = 0.f;
	uint number_of_enemies = app->objectmanager->GetNumberOfEnemies();
	float min_num_instant_teleport_enemies = 10;
	if (number_of_enemies <= min_num_instant_teleport_enemies)
	{
		for (std::vector<SpawnPoint*>::iterator spawn_point = app->map->data.spawners_position_enemy.begin(); spawn_point != app->map->data.spawners_position_enemy.end(); ++spawn_point)
		{
			float distance_to_this_spawnpoint = (target->pos_map.DistanceManhattan((*spawn_point)->pos));

			if (nearest_spawners_points == nullptr || distance_to_this_spawnpoint < last_distance_to_spawnpoint)
			{
				nearest_spawners_points = (*spawn_point);
				last_distance_to_spawnpoint = distance_to_this_spawnpoint;
			}
		}
	}
	else
	{
		for (std::vector<SpawnPoint*>::iterator spawn_point = app->map->data.spawners_position_enemy.begin(); spawn_point != app->map->data.spawners_position_enemy.end(); ++spawn_point)
		{
			float distance_to_this_spawnpoint = this->pos_map.DistanceManhattan((*spawn_point)->pos);

			if ((target->pos_map.DistanceManhattan((*spawn_point)->pos) <= distance_to_tank
				/*&& distance_to_this_spawnpoint <= distance_to_tank*/)
				&& (nearest_spawners_points == nullptr || distance_to_this_spawnpoint < last_distance_to_spawnpoint)
				&& (teleport_spawnpoint == nullptr || teleport_spawnpoint != (*spawn_point)))
			{
				nearest_spawners_points = (*spawn_point);
				last_distance_to_spawnpoint = distance_to_this_spawnpoint;
			}
		}
	}


	if (nearest_spawners_points != nullptr && distance_to_tank > target->pos_map.DistanceManhattan(nearest_spawners_points->pos))
	{
		
		if (number_of_enemies <= min_num_instant_teleport_enemies)
		{
			check_teleport_time = 0;
		}
		else
		{
			check_teleport_time = nearest_spawners_points->pos.DistanceTo(pos_map) / speed;
			if (number_of_enemies <= teleport_enemies_max)
			{
				check_teleport_time = check_teleport_time * ((number_of_enemies) / teleport_enemies_max);
			}

		}
		teleport_spawnpoint = nearest_spawners_points;
		state = ENEMY_STATE::TELEPORT_IN;
		draw = false;
		in_portal = &portal_animation;
		angle = -90;

		teleport_anim_duration.Start();
	}
	else
	{
		state = ENEMY_STATE::GET_PATH;

	}
	teleport_timer.Start();
}

inline void Obj_Enemy::TeleportIn(float & dt)
{
	move_vect.SetToZero();
	if (in_portal != nullptr)
		in_portal->NextFrame(dt);

	if (teleport_anim_duration.ReadSec() >= 1)
	{
		in_portal = &portal_close_anim;
		
		if (in_portal->Finished())
		{
			in_portal->Reset();
			pos_map = teleport_spawnpoint->pos;
			state = ENEMY_STATE::TELEPORT_OUT;
			teleport_timer.Start();
			angle = 90;
		}
	}
}

inline void Obj_Enemy::TeleportOut(float & dt)
{
	if (in_portal->Finished())
	{
		in_portal->Reset();
		in_portal = &portal_animation;
		state = ENEMY_STATE::GET_PATH;
		angle = -90;
		draw = true;
	}
	if (in_portal != nullptr)
	{
		in_portal->NextFrame(dt);
	}
}

bool Obj_Enemy::Draw(Camera * camera)
{

	if ((state == ENEMY_STATE::TELEPORT_IN || state == ENEMY_STATE::TELEPORT_OUT) && in_portal != nullptr)
	{
		SDL_Rect portal_frame = in_portal->GetFrame(0);
		app->render->Blit(
			portal_tex,
			pos_screen.x - portal_frame.w * 0.5f,
			pos_screen.y - portal_frame.h,
			camera,
			&portal_frame);
	}

	if(draw)
		app->render->BlitScaled(
			curr_tex,
			pos_screen.x - draw_offset.x,
			pos_screen.y - draw_offset.y,
			camera,
			&frame,
			scale,
			scale);

	DrawAttackRange(camera);

	return true;
}

bool Obj_Enemy::Start()
{
	burn_texture = app->tex->Load(app->anim_bank->animations_xml_node.child("burn").child("animations").child("burn").attribute("texture").as_string());

	burn.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("burn").child("animations").child("burn"));
	dying_burn.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("burn").child("animations").child("dying_burn"));
	ResetAllAnimations();
	return true;
}

void Obj_Enemy::SetStats(int level)//Executes just after creation
{

}

void Obj_Enemy::DrawAttackRange(Camera * camera)
{
	if (app->debug->debug_enemy_attack_range)
	{
		app->render->DrawIsoCircle(pos_screen.x, pos_screen.y, attack_range * app->map->data.tile_height, camera, 255, 0, 0, 255);
	}
}

bool Obj_Enemy::CleanUp()
{
	to_remove = true;
	return true;
}

inline void Obj_Enemy::UpdateMoveVec()
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

inline void Obj_Enemy::Burn(const float & dt)
{
	if (burn_fist_enter)
	{
		curr_anim = &burn;
		fire_damage = life / 3;
		if (burn_texture != nullptr)
			curr_tex = burn_texture;
		oiled = false;
		speed = original_speed;
	/*	if (coll != nullptr)
		{
			coll->to_destroy = true;
		}
		if (life_collider != nullptr)
		{
			life_collider->to_destroy = true;
		}*/
	
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

inline void Obj_Enemy::Stunned()
{
	curr_tex = tex_electro_dead;
	curr_anim = &electro_dead;
	draw_offset = electrocuted_draw_offset;
	if ((int)electro_dead.current_frame >= 4)
	{
		electro_dead.Reset();

		if (stun_charged && times_animation_repeated >= times_to_repeat_animation || !stun_charged)
		{
			times_animation_repeated = 0u;
			state = state_saved;
			curr_tex = tex;
			curr_anim = anim_saved;
			app->audio->PauseFx(channel_electrocuted, 1);
			draw_offset = normal_draw_offset;
		}
		else
		{
			++times_animation_repeated;
			app->audio->PauseFx(channel_electrocuted);
			channel_electrocuted = app->audio->PlayFx(electocuted);

		}
	}
}


void Obj_Enemy::OnTriggerEnter(Collider * collider, float dt)
{
	if (state != ENEMY_STATE::BURN)
	{
		if (collider->GetTag() == TAG::BULLET_LASER)
		{
			Laser_Bullet* obj = (Laser_Bullet*)collider->GetObj();
			if (obj->kill_counter < obj->kill_counter_max)		//sometimes in a frame does onCollision more times than it should if the enemies are together before the object is removed.
			{
				life -= collider->damage;

				damaged_sprite_timer.Start();
			//	last_texture = curr_tex;
				curr_tex = tex_damaged;
				damaged = true;

				if (life <= 0)
				{

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

		if ((collider->GetTag() == TAG::BULLET) || (collider->GetTag() == TAG::FRIENDLY_BULLET))
		{
			ReduceLife(collider->damage, dt);
		}
		else if (collider->GetTag() == TAG::BULLET_OIL)
		{
			ReduceLife(life -= collider->damage, dt);
			oiled = true;
			oiled_timer.Start();
			damaged_sprite_timer.Start();
			curr_tex = tex_damaged;

			if (life <= 0)
			{
				state = ENEMY_STATE::DEAD;
			}
			else
			{
				app->audio->PlayFx(sfx_hit);
			}
		}

		else if (collider->GetTag() == TAG::OIL_POOL)
		{
			oiled = true;
			oiled_timer.Start();
		}
		else if (collider->GetTag() == TAG::FLAMETHROWER)
		{
			if (oiled)
			{
				state = ENEMY_STATE::BURN;
			}
			else
			{
				ReduceLife(collider->damage, dt);
			}
		}
	}
	
}

void Obj_Enemy::OnTrigger(Collider * collider, float dt)
{
	if (state != ENEMY_STATE::BURN)
	{
		if (collider->GetTag() == TAG::ELECTRO_SHOT)
		{
			Obj_Tank* player = (Obj_Tank*)collider->GetObj();

			if (std::find(player->GetEnemiesHitted()->begin(), player->GetEnemiesHitted()->end(), this) == player->GetEnemiesHitted()->end())
			{
				Eletro_Shot_Animation* electro_anim = (Eletro_Shot_Animation*)app->objectmanager->CreateObject(ObjectType::ELECTRO_SHOT_ANIMATION, player->pos_map);

				electro_anim->tank = player;
				electro_anim->draw_offset -= (iPoint)app->map->MapToScreenF(player->GetShotDir());
				electro_anim->enemy_pos_screen = pos_screen;
				electro_anim->enemy_pos_map = pos_map;
				electro_anim->hit_no_enemie = false;

				player->GetEnemiesHitted()->push_back(this);
				player->hit_no_enemie = false;

				life -= collider->damage;

				damaged_sprite_timer.Start();


				if (life <= 0)
				{
					channel_electrocuted = app->audio->PlayFx(electocuted);
					state = ENEMY_STATE::DEAD;
					is_electro_dead = true;

				}
				else
				{
					app->audio->PlayFx(sfx_hit);
					channel_electrocuted = app->audio->PlayFx(electocuted);
					if (state != ENEMY_STATE::STUNNED)
					{
						state_saved = state;
					}

					anim_saved = curr_anim;

					state = ENEMY_STATE::STUNNED;
					if (player->GetIsElectroShotCharged())
					{
						stun_charged = true;
					}
					else
					{
						stun_charged = false;
					}
				}
			}

			else
			{
				LOG("inside");
			}
		}
		else if (collider->GetTag() == TAG::BULLET)
		{
			Obj_Tank* player = (Obj_Tank*)collider->GetObj();
			WeaponInfo weapon_info = player->GetWeaponInfo();

		}
		else if (collider->GetTag() == TAG::FLAMETHROWER)
		{
			if (oiled)
			{
				state = ENEMY_STATE::BURN;
			}
			else
			{
				ReduceLife(collider->damage, dt);
			}
		}
	}
}


bool Obj_Enemy::IsOnGoal(fPoint goal)
{
	return range_pos.IsPointIn(goal);
}

void Obj_Enemy::DebugPathfinding(Camera * camera)
{
	if (path.size() >= 2)
	{
		for (std::vector<iPoint>::iterator iter = path.begin(); iter != path.end() - 1; ++iter)
		{
			fPoint point1 = { (*iter).x + 0.5F, (*iter).y + 0.5F };
			fPoint point2 = { (*(iter + 1)).x + 0.5F, (*(iter + 1)).y + 0.5F };
			app->render->DrawIsometricLine(point1, point2, { 255,0,0,255 }, camera);
		}
	}
}


void Obj_Enemy::Oiled()
{

		if (damaged_sprite_timer.Read() > damaged_sprite_time && 
			!bool_electro_dead
			&& state != ENEMY_STATE::STUNNED
			&& state != ENEMY_STATE::STUNNED_CHARGED )
		{
			curr_tex = oiled_tex;
			damaged = true;
		}
		speed = original_speed*0.25f;

		if (oiled_timer.Read() >= 5000)
		{
			oiled = false;
			speed = original_speed;
		}
}

inline void Obj_Enemy::ReduceLife(int damage, float dt)
{
	life -= damage;

	damaged_sprite_timer.Start();
	last_texture = curr_tex;
	curr_tex = tex_damaged;
	damaged = true;

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

void Obj_Enemy::ResetAllAnimations()
{
	idle.Reset();
	walk.Reset();
	attack.Reset();
	death.Reset();
	burn.Reset();
	dying_burn.Reset();
	electro_dead.Reset();
	portal_animation.Reset();
	portal_close_anim.Reset();
}
