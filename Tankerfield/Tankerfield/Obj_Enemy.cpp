//#include "Obj_Enemy.h"
//#include "M_Collision.h"
//#include "Bullet_Laser.h"
//#include "M_Audio.h"
//#include "App.h"
//#include "M_PickManager.h"
//#include "M_Render.h"
//#include "M_Scene.h"
//
//
//
//Obj_Enemy::Obj_Enemy(fPoint pos) : Object(pos)
//{
//}
//
//bool Obj_Enemy::Update(float dt)
//{
//	Movement(dt);
//	Attack();
//	if (damaged_sprite_timer.Read() > damaged_sprite_time)
//	{
//		curr_tex = tex;//extract to function with if(damaged) or something.
//	}
//
//	return true;
//}
//
//void Obj_Enemy::Attack()
//{
//	if (life > 0 && app->scene->stat_of_wave != WaveStat::NO_TYPE)
//	{
//		if (target != nullptr
//			&& target->coll->GetTag() == Collider::TAG::PLAYER
//			&& pos_map.DistanceNoSqrt(target->pos_map) < attack_range_squared
//			&& perf_timer.ReadMs() > (double)attack_frequency)
//		{
//			curr_anim = &attack;
//			target->SetLife(target->GetLife() - attack_damage);
//			perf_timer.Start();
//			app->audio->PlayFx(sfx_attack);
//		}
//
//		if (curr_anim == &attack
//			&& curr_anim->Finished())
//		{
//			curr_anim = &walk;
//			attack.Reset();
//		}
//	}
//}
//
//void Obj_Enemy::DrawDebug(const Camera* camera)
//{
//	if (path.size() >= 2)
//	{
//		for (std::vector<fPoint>::iterator iter = path.begin(); iter != path.end() - 1; ++iter)
//		{
//			fPoint point1 = { (*iter).x + 0.5F, (*iter).y + 0.5F };
//			fPoint point2 = { (*(iter + 1)).x + 0.5F, (*(iter + 1)).y + 0.5F };
//			app->render->DrawIsometricLine(point1, point2, { 255,255,255,255 }, camera);
//		}
//	}
//
//}
//
//void Obj_Enemy::OnTriggerEnter(Collider * collider)
//{
//	if (collider->GetTag() == Collider::TAG::BULLET_LASER)
//	{
//		Laser_Bullet* obj = (Laser_Bullet*)collider->GetObj();
//		if (obj->kill_counter < obj->kill_counter_max)		//sometimes in a frame does onCollision more times than it should if the enemies are together before the object is removed.
//		{
//			life -= collider->damage;
//
//			damaged_sprite_timer.Start();
//			curr_tex = tex_damaged;
//
//			if (life <= 0)
//			{
//				// DROP A PICK UP ITEM 
//				app->pick_manager->PickUpFromEnemy(pos_map);
//				state = ENEMY_STATE::DEAD;
//			}
//			else
//			{
//				app->audio->PlayFx(sfx_hit);
//			}
//
//
//			if (!obj->charged)
//			{
//				++obj->kill_counter;
//				if (obj->kill_counter >= obj->kill_counter_max)
//				{
//					obj->to_remove = true;
//
//				}
//			}
//		}
//
//	}
//}
//
//void Obj_Enemy::OnTrigger(Collider* collider)
//{
//	if ((collider->GetTag() == Collider::TAG::BULLET) || (collider->GetTag() == Collider::TAG::FRIENDLY_BULLET))
//	{
//		life -= collider->damage;
//		damaged_sprite_timer.Start();
//		curr_tex = tex_damaged;
//
//		if (life <= 0)
//		{
//			app->pick_manager->PickUpFromEnemy(pos_map);
//			state = ENEMY_STATE::DEAD;
//		}
//		else
//		{
//			app->audio->PlayFx(sfx_hit);
//		}
//	}
//}
//
//bool Obj_Enemy::IsOnGoal(fPoint goal)
//{
//	return range_pos.IsPointIn(goal);
//}