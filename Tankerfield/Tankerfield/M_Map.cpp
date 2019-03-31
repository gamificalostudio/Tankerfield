#include "Brofiler\Brofiler.h"

#include "Log.h"

#include "App.h"
#include "M_Map.h"
#include "M_Window.h"
#include "M_Collision.h"
#include "M_Input.h"

M_Map::M_Map()
{
	name = "map";
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
	
	if (app->input->GetKey(SDL_SCANCODE_F1) == KeyState::KEY_DOWN)
		show_grid = !show_grid;

	return true;
}

bool M_Map::PostUpdate()
{
	BROFILER_CATEGORY("MAP DRAW", Profiler::Color::DeepPink);
	bool ret = true;

	if (map_loaded == false)
		return ret;


	for (std::list<MapLayer*>::iterator layer = data.mapLayers.begin(); layer != data.mapLayers.end(); ++layer)
	{

		if ((*layer)->visible == false) {
			continue;
		}

		for (int y = 0; y < data.rows; ++y)
		{
			for (int x = 0; x < data.columns; ++x)
			{
				int tile_id = (*layer)->Get(x, y);
				if (tile_id > 0)
				{
					iPoint pos = MapToWorld(x, y);
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

	//// Draw Grid ==============================================
	if(show_grid)
	{
		iPoint point_1, point_2;
		for (int i = 0; i <= data.columns; ++i)
		{
			point_1 = MapToWorld(i, 0);
			point_2 = MapToWorld(i, data.rows);
			app->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 255, 255, 255, true);
		}

		for (int i = 0; i <= data.rows; ++i)
		{
			point_1 = MapToWorld(0, i);
			point_2 = MapToWorld(data.columns, i);
			app->render->DrawLine(point_1.x, point_1.y, point_2.x, point_2.y, 255, 255, 255, 255, true);
		}
	}

	return ret;
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
			data.mapLayers.push_back(lay);
	}


	if (ret == true)
	{
		DebugMap();
	}

	map_loaded = ret;

	return ret;
}

void M_Map::DebugMap() 
{
	//LOG("Successfully parsed map XML file: %s", data.loadedLevel);
	LOG("width: %d height: %d", data.columns, data.rows);
	LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

	for (std::list<TileSet*>::iterator item = data.tilesets.begin(); item != data.tilesets.end(); ++item)
	{
		TileSet* s = (*item);
		LOG("Tileset ----");
		LOG("name: %s firstgid: %d", (*s).name, (*s).firstgid);
		LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
		LOG("spacing: %d margin: %d", s->spacing, s->margin);
	};

	for (std::list<MapLayer*>::iterator item_layer = data.mapLayers.begin(); item_layer != data.mapLayers.end(); ++item_layer)
	{
		MapLayer* l = (*item_layer);
		LOG("Layer ----");
		LOG("name: %s", l->name);
		LOG("tile width: %d tile height: %d", l->columns, l->rows);
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
					app->collision->AddCollider(pos, 1.F, 1.F, Collider::TAG::WALL);
				}

			++i;
		}
	}

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

	//Loading animation
	//Currently each tileset can only hold one animation - tiled map editor restriction
	//if (tileset_node.child("tile").child("animation")) {
	//	set->anim = new Animation;

	//	for (pugi::xml_node frame_node = tileset_node.child("tile").child("animation").child("frame"); frame_node; frame_node = frame_node.next_sibling()) {
	//		set->anim->PushBack(set->GetTileRect(frame_node.attribute("tileid").as_int() + set->firstgid));
	//	}
	//	pugi::xml_node speed_node = tileset_node.child("tile").child("animation").child("frame");
	//	set->anim->speed = speed_node.attribute("duration").as_float() * set->animSpeedFactor;
	//}

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
	std::list<TileSet*>::const_reverse_iterator item = data.tilesets.rbegin();
	for (item; item != data.tilesets.rend() && id < (*item)->firstgid; ++item)
	{
	}

	return (*item);
}

iPoint M_Map::MapToWorld(int column, int row) const
{
	iPoint retVec(0, 0);
	switch (data.type) {
	case MapTypes::MAPTYPE_ORTHOGONAL:
		retVec.x = column * data.tile_width;
		retVec.y = row * data.tile_height;
		break;
	case MapTypes::MAPTYPE_ISOMETRIC:
		retVec.x = (column - row) * data.tile_width * 0.5f;
		retVec.y = (column + row) * data.tile_height * 0.5f;
		break;
	default:
		LOG("ERROR: Map type not identified.");
		break;
	}

	return retVec;
}

fPoint M_Map::MapToWorldF(float x, float y)
{
	fPoint retVec(0.0F, 0.0F);
	switch (data.type) {
	case MapTypes::MAPTYPE_ORTHOGONAL:
		retVec.x = x * data.tile_width;
		retVec.y = y * data.tile_height;
		break;
	case MapTypes::MAPTYPE_ISOMETRIC:
		retVec.x = (x - y) * (data.tile_width * 0.5f);
		retVec.y = (x + y) * (data.tile_height * 0.5f);
		break;
	default:
		LOG("ERROR: Map type not identified.");
		break;
	}

	return retVec;
}


iPoint M_Map::WorldToMap(int x, int y) const
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
		ret.x = int((x / half_width + y / half_height) * 0.5f) - 1;
		ret.y = int((y / half_height - (x / half_width)) * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}
	return ret;
}

fPoint M_Map::WorldToMapF(float x, float y)
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






