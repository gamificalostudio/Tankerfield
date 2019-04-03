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
