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
#include "ElectroShotAnimation.h"




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


	fire3.frames = app->anim_bank->LoadFrames(app->anim_bank->animations_xml_node.child("fires").child("animations").child("fire3"));
	fire_tex = app->tex->Load(app->anim_bank->animations_xml_node.child("fires").child("animations").child("fire3").attribute("texture").as_string(""));

	path_timer.Start();
}

bool Obj_Enemy::Update(float dt)
{
	Movement(dt);
	Attack();
	if(in_white)
		ChangeTexture();

	return true;
}

void Obj_Enemy::ChangeTexture()
{
	
	if (damaged_sprite_timer.Read() > damaged_sprite_time && 
		curr_tex != tex && 
		state != ENEMY_STATE::STUNNED &&
		state != ENEMY_STATE::STUNNED_CHARGED &&
		state != ENEMY_STATE::DEAD)
	{
		curr_tex = last_texture;
	}
}

void Obj_Enemy::Attack()
{
	if (life > 0 && app->scene->stat_of_wave != WaveStat::NO_TYPE)
	{
		if (target != nullptr
			&& target->coll->GetTag() == TAG::PLAYER
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

	curr_anim = curr_anim;

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

void Obj_Enemy::ElectroDead()
{
	if (curr_anim != &electro_dead)
	{
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
	if (collider->GetTag() == TAG::BULLET_LASER)
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

	if (collider->GetTag() == TAG::ELECTRO_SHOT)
	{
		Obj_Tank* player = (Obj_Tank*)collider->GetObj();
		//player->draw_electro_shot = true;
		
		if (std::find(player->GetEnemiesHitted()->begin(), player->GetEnemiesHitted()->end(), this) == player->GetEnemiesHitted()->end())
		{
			Eletro_Shot_Animation* electro_anim = (Eletro_Shot_Animation*)app->objectmanager->CreateObject(ObjectType::ELECTRO_SHOT_ANIMATION, player->pos_map);
			
			electro_anim->offset_dir_screen = app->map->MapToScreenF(player->GetShotDir());
			electro_anim->distance = player->pos_screen.DistanceTo(this->pos_screen + app->map->MapToScreenF(/*more distance para que no se quede el rayo al borde*/player->GetShotDir()));
			electro_anim->player_enemy_distance_point = app->map->MapToScreenF(this->pos_map - player->pos_map);

			player->GetEnemiesHitted()->push_back(this);
			player->hit_no_enemie = false;

			life -= collider->damage;

			damaged_sprite_timer.Start();
			/*curr_tex = tex_damaged;*/
			//float player_enemy_distance = player->pos_map.DistanceTo(this->pos_map);
			
			if (life <= 0)
			{
				// DROP A PICK UP ITEM 
				channel_electrocuted = app->audio->PlayFx(electocuted);
				app->pick_manager->PickUpFromEnemy(pos_map);
				state = ENEMY_STATE::DEAD;
				is_electro_dead = true;
				
			}
			else
			{
				app->audio->PlayFx(sfx_hit);
				channel_electrocuted = app->audio->PlayFx(electocuted);
				state_saved = state;
				
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
	}
}

void Obj_Enemy::OnTrigger(Collider* collider)
{
	if ((collider->GetTag() == TAG::BULLET) || (collider->GetTag() == TAG::FRIENDLY_BULLET))
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
}

bool Obj_Enemy::IsOnGoal(fPoint goal)
{
	return range_pos.IsPointIn(goal);
}