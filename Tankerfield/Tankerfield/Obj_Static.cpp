#include "PugiXml/src/pugiconfig.hpp"
#include "PugiXml/src/pugixml.hpp"

#include "app.h"
#include "Obj_Static.h"
#include "M_Textures.h"
#include "M_Render.h"
#include "M_Collision.h"
#include "M_Input.h"
#include "Log.h"
#include "M_Map.h"
#include "M_ObjManager.h"

Obj_Static::Obj_Static() : Object()
{

}

Obj_Static::Obj_Static(fPoint pos) : Object(pos)
{
	
}

Obj_Static::~Obj_Static()
{
}

bool Obj_Static::Awake(pugi::xml_node & static_node)
{
	return true;
}

bool Obj_Static::Start()
{
	SetRect(0, 0, 180, 185);
	SetPivot(90.f, 92.f);

	size = iPoint(frame.w, frame.h);

	if (app->map->data.type == MAPTYPE_ISOMETRIC) { //If map is isometric, we have to transform orthogonal position to isometric position
		pos_map.x /= app->map->data.tile_width * 0.5f;
		pos_map.y /= app->map->data.tile_height;

		iPoint pos = app->map->MapToScreenI(pos_map.x + 1, pos_map.y + 1);
		pos_map.create(pos.x, pos.y);
	}

	data.tileset.texture = app->tex->Load(app->map->data.objects_path.data()); //Load object texture

	garage = app->tex->Load("Maps/Garage-Sheet.png");

	return true;
}

bool Obj_Static::PostUpdate(float dt)
{
	fPoint screen_pos = app->map->MapToScreenF(pos_map);
	app->render->Blit(data.tileset.texture, screen_pos.x, screen_pos.y, &frame);
	return true;
}

void Obj_Static::SetRect(int x, int y, int w, int h)
{
	frame.x = x;
	frame.y = y;
	frame.w = w;
	frame.h = h;
}
