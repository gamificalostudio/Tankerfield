#include "Obj_ElectroShotAnimation.h"
#include "M_Render.h"
#include "App.h"
#include "M_Textures.h"
#include "M_AnimationBank.h"
#include "M_Map.h"

Eletro_Shot_Animation::Eletro_Shot_Animation(fPoint pos) :Object(pos)
{
	pugi::xml_node electro_shot_node = app->config.child("object").child("tank").child("electro_shot").child("electro_shot_animation");

	tex = app->tex->Load(electro_shot_node.child("tex_electro_shot").text().as_string());
	curr_tex = tex;

	anim.frames = app->anim_bank->LoadFrames(electro_shot_node.child("animations").child("anim_electro_shot"));
	curr_anim = &anim;

	draw_offset = { 0,0 };
	draw_offset.x = curr_anim->GetFrame(0).w * 0.5f;
	//	electro_offset.y = 30;
}

Eletro_Shot_Animation::~Eletro_Shot_Animation()
{
}

bool Eletro_Shot_Animation::Update(float dt)
{
	pos_map = tank->pos_map;
	pos_screen = tank->pos_screen;
	if (!hit_no_enemie)
	{
		player_enemy_distance_point = app->map->MapToScreenF(enemy_pos_map - pos_map);
		distance = pos_screen.DistanceTo(enemy_pos_screen);
	}
	else
	{
		distance = 25.f;
		player_enemy_distance_point = app->map->MapToScreenF(tank->GetShotDir());
	}
	if (!curr_anim->Finished())
	{
		curr_anim->NextFrame(dt);
	}
	else
	{
		curr_anim->Reset();
		to_remove = true;
	}

	return true;
}

bool Eletro_Shot_Animation::Draw(Camera* camera)
{
	app->render->BlitScaledAndRotated(
		tex,
		pos_screen.x - draw_offset.x,
		pos_screen.y - draw_offset.y,
		camera,
		&curr_anim->GetFrame(0),
		1,
		distance / curr_anim->GetFrame(0).h,
		SDL_Point{ (int)(curr_anim->GetFrame(0).w * 0.5f) , 0 },
		atan2(player_enemy_distance_point.y, player_enemy_distance_point.x) * RADTODEG - 2 * ISO_COMPENSATION
	);

	return true;
}
