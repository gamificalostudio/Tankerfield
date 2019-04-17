#include <list>


#include "Brofiler\Brofiler.h"

#include "Log.h"

#include "App.h"
#include "M_Map.h"
#include "M_Window.h"
#include "M_Collision.h"
#include "M_Input.h"
#include "M_Pathfinding.h"
#include "M_Scene.h"


M_Map::M_Map()
{
	name.assign("map");
}

M_Map::~M_Map()
{
}
bool M_Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder = config.child("folder").child_value();

	// create a list with total num of levels

	pugi::xml_node levelsNode = config.child("levels");
	if (levelsNode == NULL)
	{
		LOG("num of levels not found");
	}
	else
	{
		LOG("level data found");

		for (pugi::xml_node level = levelsNode.child("level"); level; level = level.next_sibling("level"))
		{
			Levels* newLevel = new Levels();
			newLevel->name = level.attribute("name").as_string();
			numLevels++;
			levels.push_back(newLevel);
		}
	}
	return true;
}

bool M_Map::Update(float dt)
{
	BROFILER_CATEGORY("MAP DRAW", Profiler::Color::DeepPink);
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_F1) == KeyState::KEY_DOWN)
		show_grid = !show_grid;

	if (!map_loaded)
		return ret;

	return true;
}

bool M_Map::PostUpdate(float dt)
{
	BROFILER_CATEGORY("MAP DRAW", Profiler::Color::DeepPink);
	bool ret = true;

	if (map_loaded == false)
		return ret;

	BROFILER_CATEGORY("MAP DRAW init", Profiler::Color::DeepPink);
		
	for (std::vector<Camera*>::iterator item_cam = app->render->camera.begin(); item_cam != app->render->camera.end(); ++item_cam)
	{
		SDL_RenderSetClipRect(app->render->renderer, &(*item_cam)->viewport);

		std::vector<Tile>aux = data.qt->GetTilesIntersection(*(*item_cam));
		std::sort(aux.begin(), aux.end(), [](Tile a, Tile b)
		{
			return a.sorting_value < b.sorting_value;
		});

		for (std::vector<Tile>::iterator sorted_tiles = aux.begin(); sorted_tiles != aux.end(); ++sorted_tiles)
		{
			TileSet* tileset = app->map->GetTilesetFromTileId((*sorted_tiles).id);

			app->render->Blit(tileset->texture,
				(*sorted_tiles).rect.x,
				(*sorted_tiles).rect.y,
				(*item_cam),
				&tileset->GetTileRect((*sorted_tiles).id));
		}
		SDL_RenderSetClipRect(app->render->renderer, nullptr);
	}

	//// Draw Grid ==============================================
	if(show_grid)
	{
		iPoint point_1, point_2;
		
		for (std::vector<Camera*>::iterator item_cam = app->render->camera.begin(); item_cam != app->render->camera.end(); ++item_cam)
		{
			SDL_RenderSetClipRect(app->render->renderer, &(*item_cam)->viewport);
			for (int i = 0; i <= data.columns; ++i)
			{
				point_1 = MapToScreenI(i, 0);
				point_2 = MapToScreenI(i, data.rows);
				app->render->DrawLineSplitScreen(point_1.x, point_1.y, point_2.x, point_2.y, 255, 255, 255, 255, (*item_cam));
			}
			for (int i = 0; i <= data.rows; ++i)
			{
				point_1 = MapToScreenI(0, i);
				point_2 = MapToScreenI(data.columns, i);
				app->render->DrawLineSplitScreen(point_1.x, point_1.y, point_2.x, point_2.y, 255, 255, 255, 255, (*item_cam));
			}
		}
		SDL_RenderSetClipRect(app->render->renderer, nullptr);
	}

	return ret = true;
}

		
bool M_Map::CleanUp()
{
	Unload();

	for (std::list<Levels*>::iterator level_item = levels.begin(); level_item != levels.end(); ++level_item)
	{
		delete (*level_item);
	}
	levels.clear();

		return true;
}

bool M_Map::Load(const std::string& file_name)
{
	bool ret = true;
	std::string tmp = folder + file_name;

	pugi::xml_parse_result result = map_file.load_file(tmp.c_str());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		return ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for (tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info --------------------------------------------------------------------------------------
	
	for (pugi::xml_node layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true)
			data.map_layers.push_back(lay);
	}
	// Load object layer info -----------------------------------------------------------------------------

	for (pugi::xml_node obj_layer = map_file.child("map").child("objectgroup"); obj_layer && ret; obj_layer = obj_layer.next_sibling("objectgroup"))
	{
		ObjectGroup* obj_lay = new ObjectGroup();

		ret = LoadObjectGroup(obj_layer, obj_lay);

		if (ret == true)
			data.object_layers.push_back(obj_lay);
	}

	if (ret == true)
	{
		DebugMap();
	}

	map_loaded = ret;

	if (map_loaded)
	{
		int w, h;
		uchar* data = NULL;
		if (CreateWalkabilityMap(w, h, &data))
			app->pathfinding->SetMap(w, h, data);
		LOG("Map's walkability successfuly created");
	}

	return ret;
}

bool M_Map::Unload()
{
	if (!map_loaded)
		return false;

	for (std::list<TileSet*>::iterator iter = data.tilesets.begin(); iter != data.tilesets.end(); ++iter)
	{
		if ((*iter != nullptr))
		{
			delete (*iter);

		}
	}
	data.tilesets.clear();

	for (std::list<MapLayer*>::iterator iter = data.map_layers.begin(); iter != data.map_layers.end(); ++iter)
	{
		if ((*iter != nullptr))
		{
			delete (*iter);

		}
	}
	data.map_layers.clear();

	for (std::list<ObjectGroup*>::iterator iter = data.object_layers.begin(); iter != data.object_layers.end(); ++iter)
	{
		if ((*iter) != nullptr)
		{
			delete (*iter);
		}
	}
	data.object_layers.clear();

	if (app->on_clean_up == false)
	{
		for (std::list<Collider*>::iterator iter = data.colliders_list.begin(); iter != data.colliders_list.end(); ++iter)
		{
			if ((*iter != nullptr))
			{
				(*iter)->Destroy();
			}
		}
	}

	data.colliders_list.clear();

	if (data.qt != nullptr)
		delete data.qt;
	if (data.screen_tile_rect != nullptr)
	{
		delete[] data.screen_tile_rect;
		data.screen_tile_rect = nullptr;
	}
	data.map_properties.UnloadProperties();


	return true;
}

void M_Map::DebugMap() 
{
	//LOG("Successfully parsed map XML file: %s", data.loadedLevel);
	LOG("width: %i height: %i", data.columns, data.rows);
	LOG("tile_width: %i tile_height: %i", data.tile_width, data.tile_height);

	for (std::list<TileSet*>::iterator item = data.tilesets.begin(); item != data.tilesets.end(); ++item)
	{
		TileSet* s = (*item);
		LOG("Tileset ----");
		LOG("name: %s firstgid: %i", (*s).name.data(), (*s).firstgid);
		LOG("tile width: %i tile height: %i", s->tile_width, s->tile_height);
		LOG("spacing: %i margin: %i", s->spacing, s->margin);
	};

	for (std::list<MapLayer*>::iterator item_layer = data.map_layers.begin(); item_layer != data.map_layers.end(); ++item_layer)
	{
		MapLayer* l = (*item_layer);
		LOG("Layer ----");
		LOG("name: %s", l->name.data());
		LOG("tile width: %i tile height: %i", l->columns, l->rows);
	}

}

bool M_Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();

	layer->columns = node.attribute("width").as_int();

	layer->rows = node.attribute("height").as_int();

	layer->layer_properties.LoadProperties(node.child("properties"));

	layer->visible = (node.attribute("visible").as_int(1) == 0) ? false : true;
	
	pugi::xml_node layer_data = node.child("data");


	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		

		layer->data = new uint[layer->columns*layer->rows];
		memset(layer->data, 0, layer->columns*layer->rows);

		if (layer->visible)
		{


			uint i = 0, layernum = data.map_layers.size() + 1;
			for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
			{
				Tile qtile;

				layer->data[i] = tile.attribute("gid").as_int(0);
				qtile.id = layer->data[i];
				if (qtile.id != 0)
				{
					qtile.rect = { data.screen_tile_rect[i].pos.x, data.screen_tile_rect[i].pos.y, data.tile_width, data.tile_height };
					qtile.layer = layernum;
					qtile.sorting_value = i * layernum;
					data.qt->InsertTile(qtile);
				}
				if (layer->name == "Buildings")
				{
					layer->visible = node.child("properties").child("property").attribute("value").as_bool(true);
				}

				++i;
			}
		}

		
	}

	return ret;
}

bool M_Map::LoadObjectGroup(const pugi::xml_node & object_group_node, ObjectGroup * object_group)
{
	bool ret = true;
	if (object_group_node == NULL)
	{
		RELEASE(object_group);
		return ret = false;
	}

	object_group->name = object_group_node.attribute("name").as_string();
	
	for (pugi::xml_node obj_node = object_group_node.child("object"); obj_node; obj_node = obj_node.next_sibling())
	{
		++object_group->size;
	}
	object_group->objects = new Rect<float, float>[object_group->size];
	
	uint i = 0;
	for (pugi::xml_node obj_node = object_group_node.child("object"); obj_node; obj_node = obj_node.next_sibling())
	{
		object_group->objects[i].create(
			obj_node.attribute("x").as_int(0),
			obj_node.attribute("y").as_int(0),
			obj_node.attribute("width").as_int(0),
			obj_node.attribute("height").as_int(0));
		
		++i;
	}
	
	if (object_group->name == "Colliders")
	{
		for (i = 0; i < object_group->size; ++i)
		{
			// To ortogonal tile pos-----------------
			fPoint pos = { (float)(object_group->objects[i].pos.x / data.tile_height),  (float)(object_group->objects[i].pos.y/ data.tile_height) };
			fPoint mesure = { (float)object_group->objects[i].w / data.tile_height, (float)object_group->objects[i].h / data.tile_height };
			app->collision->AddCollider(pos, mesure.x, mesure.y, Collider::TAG::WALL);
		}
	}
	
	object_group->properties.LoadProperties(object_group_node.child("properties"));

	return ret;
}



bool M_Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = app->tex->Load(PATH(folder.c_str(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if (set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if (set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->columns = tileset_node.attribute("columns").as_int(0);
		set->rows = set->tex_height / set->tile_height;
	}

	return ret;
}

bool M_Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;

	set->name.assign(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();

	pugi::xml_node offset = tileset_node.child("tileoffset");
	if (offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

// Load map general properties
bool M_Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.columns = map.attribute("width").as_int();
		data.rows = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();

		bool ret = false;

		data.map_properties.LoadProperties(map.child("properties"));
		data.objects_path = data.map_properties.GetAsString("object_texture");
		data.offset_x = data.map_properties.GetAsInt("offset_x");
		data.offset_y = data.map_properties.GetAsInt("offset_y");

		std::string orientation(map.attribute("orientation").as_string());
		if (orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;

		}
		else if (orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if (orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}

		data.screen_tile_rect = new iRect[data.columns*data.rows];
		for (int y = 0; y < data.rows; ++y)
		{
			for (int x = 0; x < data.columns; ++x)
			{
				iPoint pos = app->map->MapToScreenI(x, y);
				data.screen_tile_rect[(y*data.columns) + x].create(pos.x + data.offset_x, pos.y + data.offset_y, data.tile_width, data.tile_height);
			}
		}
		SDL_Rect area = { data.screen_tile_rect[(data.rows - 1)*(data.columns)].pos.x,
					 0,
					 (data.screen_tile_rect[(data.columns - 1)].GetRight() + abs(data.screen_tile_rect[(data.rows - 1)*(data.columns)].pos.x)),
					data.screen_tile_rect[((data.rows - 1)*data.columns) + (data.columns - 1)].pos.y + data.screen_tile_rect[((data.rows - 1)*data.columns) + (data.columns - 1)].h };
		;

		uint level = 1;
		data.qt->ReturnNumbreOfLevels(area.w, (*app->render->camera.begin())->viewport.w*0.25, level);
		data.qt = new Quadtree_Map(area, 0, level);

	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect = { 
		margin + ((tile_width + spacing) * (relative_id % columns)),
		margin + ((tile_height + spacing) * (relative_id / columns)),
		tile_width,
		tile_height,
		 };

	return rect;
}

inline TileSet* M_Map::GetTilesetFromTileId(int id) const
{
	std::list<TileSet*>::const_iterator item = data.tilesets.begin();
	TileSet* set = *item;
	
	while (item != data.tilesets.end())
	{
		if (id < (*item)->firstgid)
		{
			set = *prev(item);
			break;
		}

		set = *item;
		++item;
	}

	return set;
}

uint M_Map::GetMaxLevels()
{
	return numLevels;
}

bool M_Map::CreateWalkabilityMap(int& width, int &height, uchar** buffer) const
{
	bool ret = false;

	for (std::list<MapLayer*>::const_iterator item = data.map_layers.begin(); item != data.map_layers.end(); ++item)
	{
		MapLayer* layer = *item;

		if (layer->layer_properties.GetAsFloat("Navigation", 0) == 0)
			continue;

		uchar* map = new uchar[layer->columns * layer->rows];
		memset(map, 1, layer->columns*layer->rows);

		for (int y = 0; y < data.rows; ++y)
		{
			for (int x = 0; x < data.columns; ++x)
			{
				int i = (y*layer->rows) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

				if (tileset != NULL)
				{
					map[i] = ((tile_id - tileset->firstgid) > 0) ? 0 : 1;
				
				}
			}
		}
	
		*buffer = map;
		width = data.columns;
		height = data.rows;
		ret = true;

		break;
	}

	return ret;
}

iPoint M_Map::MapToScreenI(int column, int row) const
{
	
	iPoint screen_pos(0, 0);
	switch (data.type) {
	case MapTypes::MAPTYPE_ORTHOGONAL:
		screen_pos.x = column * data.tile_width;
		screen_pos.y = row * data.tile_height;
		break;
	case MapTypes::MAPTYPE_ISOMETRIC:
		screen_pos.x = (column - row) * data.tile_width * 0.5f;
		screen_pos.y = (column + row) * data.tile_height * 0.5f;
		break;
	default:
		LOG("ERROR: Map type not identified.");
		break;
	}

	return screen_pos;
}

fPoint M_Map::MapToScreenF(const fPoint & map_pos)
{
	fPoint screen_pos(0.0F, 0.0F);
	switch (data.type) {
	case MapTypes::MAPTYPE_ORTHOGONAL:
		screen_pos.x = map_pos.x * data.tile_width;
		screen_pos.y = map_pos.y * data.tile_height;
		break;
	case MapTypes::MAPTYPE_ISOMETRIC:
		screen_pos.x = (map_pos.x - map_pos.y) * (data.tile_width * 0.5f);
		screen_pos.y = (map_pos.x + map_pos.y) * (data.tile_height * 0.5f);
		break;
	default:
		LOG("ERROR: Map type not identified.");
		break;
	}

	return screen_pos;
}


iPoint M_Map::ScreenToMapI(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int((x / half_width + y / half_height) * 0.5f);
		ret.y = int((y / half_height - (x / half_width)) * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}
	return ret;
}

fPoint M_Map::ScreenToMapF(float x, float y)
{
	fPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = ((x / half_width) + y / half_height) * 0.5f;
		ret.y = ((y / half_height) - x / half_width) * 0.5f;
		return ret;
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}
	return ret;
}

std::string Properties::GetAsString(const char * name, std::string default_value) const
{
	std::string ret = default_value;
	for (std::list<Property*>::const_iterator item = list.begin(); item != list.end(); ++item)
	{
		if ((*item)->name == name)
		{
			return ret = *(std::string*)(*item)->value;
		}
	}
	return ret;
}

int Properties::GetAsInt(const char * name, int default_value) const
{
	int ret = default_value;
	for (std::list<Property*>::const_iterator item = list.begin(); item != list.end(); ++item)
	{
		if ((*item)->name == name)
		{
			return ret = *(int*)(*item)->value;
		}
	}
	return ret;
}

float Properties::GetAsFloat(const char * name, float default_value) const
{
	float ret = default_value;
	for (std::list<Property*>::const_iterator item = list.begin(); item != list.end(); ++item)
	{
		if ((*item)->name == name)
		{
			ret = *(float*)(*item)->value;
			return ret;
		}
	}
	return ret;
}

bool Properties::GetAsBool(const char * name, bool default_value) const
{
	bool ret = default_value;
	for (std::list<Property*>::const_iterator item = list.begin(); item != list.end(); ++item)
	{
		if ((*item)->name == name)
		{
			ret = *(bool*)(*item)->value;
			return ret;
		}
	}
	return ret;
}

void Properties::LoadProperties(pugi::xml_node propertie_node)
{
	for (pugi::xml_node iter = propertie_node.child("property"); iter; iter = iter.next_sibling("property"))
	{
		Property* p = new Property();
		p->name = iter.attribute("name").as_string();
		std::string type = iter.attribute("type").as_string();
		if (type == "int")
		{
			p->value = new int(iter.attribute("value").as_int());
		}
		else if (type == "float")
		{
			p->value = new float(iter.attribute("value").as_float());
		}
		else if (type == "bool")
		{
			p->value = new bool(iter.attribute("value").as_bool());
		}
		else
		{
			p->value = new std::string(iter.attribute("value").as_string());
		}
		list.push_back(p);

	}
}

void Properties::UnloadProperties()
{
	std::list<Property*>::iterator item = list.begin();

	while (item != list.end())
	{
		RELEASE(*item);
		++item;
	}

	list.clear();
}
