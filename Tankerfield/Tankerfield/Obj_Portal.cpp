#include "App.h"
#include "Object.h"

#include "Obj_Portal.h"
#include "M_AnimationBank.h"
#include "Animation.h"
#include "M_Textures.h"
#include "M_Collision.h"

Obj_Portal::Obj_Portal(fPoint pos):Object(pos)
{
	pugi::xml_node portal_node = app->config.child("object").child("portal_TP");

	anim.frames = app->anim_bank->LoadFrames(portal_node.child("animations").child("portal"));
	curr_anim = &anim;

	tex = app->tex->Load(portal_node.child("tex").attribute("path").as_string());
	curr_tex = tex;

	draw_offset.x = 20;
	draw_offset.y = 20;

	time.Start();

	coll=app->collision->AddCollider(pos_map, 1, 1, TAG::PORTAL, BODY_TYPE::DYNAMIC, 0.f, this);
	coll->is_sensor = true;

	pivot = {20,30};
}

Obj_Portal::~Obj_Portal()
{
}

bool Obj_Portal::Update(float dt)
{
	if (time.ReadMs() >= 10000 && coll != nullptr)
	{
		to_remove = true;
		coll->Destroy();
		coll = nullptr;
	}

	return true;
}
