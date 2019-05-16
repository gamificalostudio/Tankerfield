#include <list>
#include <assert.h>
#include <vector>

#include "Brofiler/Brofiler.h"
#include "PugiXml\src\pugixml.hpp"

#include "Point.h"
#include "Log.h"
#include "Animation.h"
#include "App.h"
#include "Object.h"
#include "Obj_RocketLauncher.h"
#include "M_Textures.h"
#include "M_ObjManager.h"
#include "M_Render.h"
#include "M_Scene.h"
#include "M_Pathfinding.h"
#include "M_Input.h"
#include "Animation.h"
#include "M_Map.h"
#include "M_Collision.h"
#include "WeaponInfo.h"
#include "M_AnimationBank.h"
#include "Obj_Tank.h"
#include "M_PickManager.h"
#include "Bullet_Laser.h"
#include "Obj_Bullet.h"
#include "M_Audio.h"

Obj_RocketLauncher::Obj_RocketLauncher(fPoint pos) : Obj_Enemy(pos)
{
	pugi::xml_node rocket_launcher_node = app->config.child("object").child("rocket_launcher");
	pugi::xml_node anim_node = app->anim_bank->animations_xml_node.child("rocketlauncher");

	tex = app->tex->Load(rocket_launcher_node.child("tex_path").child_value());
	curr_tex = tex;

	//Loading animations ------------------------------------------------------------------------------------------------------------------------
	idle.frames = app->anim_bank->LoadFrames(anim_node.child("idle"));
	walk.frames = app->anim_bank->LoadFrames(anim_node.child("walk"));
	attack.frames = app->anim_bank->LoadFrames(anim_node.child("attack"));
	death.frames = app->anim_bank->LoadFrames(anim_node.child("death"));

	curr_anim = &idle;
	
	state = ENEMY_STATE::IDLE;
	detection_range = 7.0f;

	spawn_draw_offset = { 49, 47 };
	normal_draw_offset = { 49, 47 };
	draw_offset = spawn_draw_offset;

	attack_damage = rocket_launcher_node.child("attack_damage").attribute("num").as_float();
	attack_range = rocket_launcher_node.child("attack_range").attribute("num").as_float();
	attack_range_squared = attack_range * attack_range;
	attack_frequency = rocket_launcher_node.child("attack_frequency").attribute("num").as_float();
	life = pow(rocket_launcher_node.child("base_life").attribute("num").as_float(), app->scene->round);
	
	check_path_time = 2.0f;

	scale = 0.75f;
}

Obj_RocketLauncher::~Obj_RocketLauncher()
{

}

void Obj_RocketLauncher::Spawn(const float& dt)
{
	state = ENEMY_STATE::GET_PATH;
}
