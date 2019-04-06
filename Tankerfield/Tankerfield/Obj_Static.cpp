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
	SetRect(286, 448, 286, 224);
	SetPivot(0, 0);

	size = iPoint(frame.w, frame.h);

	data.tileset.texture = app->tex->Load(app->map->data.objects_path.data()); //Load object texture

	return true;
}

bool Obj_Static::PostUpdate(float dt)
{
	app->render->Blit(data.tileset.texture, pos_screen.x - 142, pos_screen.y - 167, &frame);

	//fPoint pivot_pos = app->map->MapToScreenF(pivot);

	//SDL_Rect debug_pivot = { pivot.x - 3, pivot.y - 3, 6, 6 };
	//debug_pivot.y += 87;
	//app->render->DrawQuad(debug_pivot, 0, 255, 150);

	return true;
}
