#include <list>

#include "Brofiler\Brofiler.h"

#include "Log.h"
#include "App.h"
#include "M_Map.h"
#include "M_Window.h"
#include "M_Collision.h"
#include "M_Input.h"
#include "M_Pathfinding.h"

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
	for (std::list<MapLayer*>::iterator layer = data.map_layers.begin(); layer != data.map_layers.end(); ++layer)
	{

		if ((*layer)->visible && (*layer)->layer_properties.draw) {

			for (int y = 0; y < data.rows; ++y)
			{
				for (int x = 0; x < data.columns; ++x)
				{

					int tile_id = (*layer)->Get(x, y);
					if (tile_id > 0)
					{
						iPoint pos = MapToScreenI(x, y);
						if (app->render->IsOnCamera(pos.x + data.offset_x, pos.y + data.offset_y, data.tile_width, data.tile_height))
						{
							TileSet* tileset = GetTilesetFromTileId(tile_id);
							if (tileset != nullptr)
							{
								SDL_Rect r = tileset->GetTileRect(tile_id);
								app->render->Blit(tileset->texture, pos.x + data.offset_x, pos.y + data.offset_y, &r);
							}
						}
					}
				}
			}
		}
	}
	
	//// Draw Grid ==============================================
	if(show_grid)
	{
		iPoint point_1, point_2;
		for (int i = 0; i <= data.columns; ++i)
		{
			point_1 = MapToScreenI(i, 0);
			point_2 = MapToScreenI(i, data.rows);
			app->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 255, 255, 255, true);
		}

		for (int i = 0; i <= data.rows; ++i)
		{
			point_1 = MapToScreenI(0, i);
			point_2 = MapToScreenI(data.columns, i);
			app->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 255, 255, 255, true);
		}
	}

	return ret;
}

bool M_Map::CleanUp()
{
	Unload();

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

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true)
			data.map_layers.push_back(lay);
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
	int visible = node.attribute("visible").as_int(1);
	layer->visible = (visible == 0) ? false : true;
	
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

		int i = 0;
		for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i] = tile.attribute("gid").as_int(0);

			if (layer->name == "Colliders" && layer->data[i] != 0u)
			{
					fPoint pos = layer->GetTilePos(i);

					Collider* aux = app->collision->AddCollider(pos, 1.F, 1.F, Collider::TAG::WALL);
					data.colliders_list.push_back(aux);
			}

			if (layer->name == "Buildings")
			{
				layer->layer_properties.draw = node.child("properties").child("property").attribute("value").as_bool(true);
			}

			++i;
		}
	}

	return ret;
}

bool M_Map::LoadObjectGroup(const pugi::xml_node & object_group_node, ObjectGroup * object_group)
{
	bool ret = true;
	object_group->name = object_group_node.attribute("name").as_string();
	for (pugi::xml_node obj_node = object_group_node.child("object"); obj_node; obj_node = obj_node.next_sibling())
	{
		++object_group->size;
	}
	object_group->objects = new SDL_Rect[object_group->size];
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
		data.map_properties.draw = data.map_properties.GetAsBool("NoDraw");
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
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect = { 0, 0, 0, 0 };
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % columns));
	rect.y = margin + ((rect.h + spacing) * (relative_id / columns));
	return rect;
}

TileSet* M_Map::GetTilesetFromTileId(int id) const
{
	BROFILER_CATEGORY("GetTilesetFromTileId", Profiler::Color::DarkBlue)
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
