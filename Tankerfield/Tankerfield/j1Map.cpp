#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"

#include <math.h>
#include "j1Scene.h"

#include "Brofiler/Brofiler.h"

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name = "map";
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
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
			data.numLevels++;

			data.levels.push_back(newLevel);
		}
	}

	
	return ret;
}

void j1Map::Draw()
{
	BROFILER_CATEGORY("MAP DRAW", Profiler::Color::DeepPink);

	if(map_loaded == false)
		return;

	int mapBlitOffset = 50;//TODO: Get this from the xml

	
	for (std::list<MapLayer*>::iterator layer = data.mapLayers.begin(); layer != data.mapLayers.end; ++layer)
	{
	
		if ((*layer)->name == "navigationLayer" && showNavLayer == false) {
			continue;
		}

		for (int y = 0; y < data.rows; ++y)
		{
			for (int x = 0; x < data.columns; ++x)
			{
				int tile_id = (*layer)->GetArrayPos(x, y);
				if (tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);
					if (tileset != nullptr)
					{
						SDL_Rect r = tileset->GetTileRect(tile_id);
						iPoint pos = MapToWorld(x, y);

						
							App->render->Blit(tileset->texture, pos.x, pos.y, &r, (*layer)->properties.parallaxSpeed);
						
					}
				}
			}
		}
	}
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	std::list<TileSet*>::const_reverse_iterator item = data.tilesets.rbegin();
	for (item; item != data.tilesets.rend() && id < (*item)->firstgid; ++item)
	{

	}

	return (*item);
}

iPoint j1Map::MapToWorld(int column, int row) const
{
	iPoint retVec(0,0);
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


iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint retVec(0,0);
	switch (data.type) {
	case MapTypes::MAPTYPE_ORTHOGONAL:
		retVec.x = x / data.tile_width;
		retVec.y = y / data.tile_height;
		break;
	case MapTypes::MAPTYPE_ISOMETRIC:
		retVec.y = y / data.tile_height - x / data.tile_width;
		retVec.x = (2 * x) / data.tile_width + retVec.y;
		break;
	default:
		LOG("ERROR: Map type not set.");
		break;
	}
	return retVec;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect = {0, 0, 0, 0};
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % columns));
	rect.y = margin + ((rect.h + spacing) * (relative_id / columns));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");
	// Remove all tilesets-----------------------------------------------------------------------------------
	for (std::list<TileSet*>::iterator item = data.tilesets.begin(); item != data.tilesets.end(); ++item)
	{
		if ((*item)->texture != nullptr)
		{
			App->tex->UnLoad((*item)->texture);
			(*item)->texture = nullptr;
		}
		RELEASE((*item));
	}

	data.tilesets.clear();

	// Remove all layers-----------------------------------------------------
	for (std::list<MapLayer*>::iterator item = data.mapLayers.begin(); item != data.mapLayers.end(); ++item)
	{
		RELEASE(*item);
	}
	data.mapLayers.clear();

	// remove all colliders/object data
	for (std::list<MapObjects*>::iterator objectItem = data.mapObjects.begin(); objectItem != data.mapObjects.end(); ++objectItem)
	{
		for (uint i = 0; i < MAX_OBJECTGROUP_COLLIDERS; ++i)
		{
			if ((*objectItem)->colliders[i] != nullptr)
			{
				//App->collision->DeleteCollider(objectItem->data->colliders[i]);
			}
		}
		RELEASE(*objectItem);	
	}
	data.mapObjects.clear();

	// TODO: search the way to load levelData outside awake
	// if we free leveldata list here called by new map load
	// levelData is lost forever
	// free leveldata
	/*p2List_item<Levels*>* levels;
	levels = data.levels.start;
	while (levels != NULL)
	{
		RELEASE(levels->data);
		levels = levels->next;
	}
	data.levels.clear();*/

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	std::string tmp = folder + file_name;

	pugi::xml_parse_result result = map_file.load_file(tmp.c_str());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		return ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		// stores the current level string name
		data.loadedLevel = file_name;
		// ------------------------------------
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for(layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if(ret == true)
			data.mapLayers.push_back(lay);
	}

	// Load objects/scene colliders -----------------------------------------
	pugi::xml_node objectGroup;
	for (objectGroup = map_file.child("map").child("group"); objectGroup && ret; objectGroup = objectGroup.next_sibling("group"))
	{
		std::string tmp = objectGroup.attribute("name").as_string();
		//MapObjects* obj = new MapObjects();

		if (tmp == "Colliders")
		{
			//for(pugi::xml_node collidersGroup = objectGroup.child("objectgroup"))
			ret = LoadMapColliders(objectGroup);//, obj);
			LOG("loading Map colliders");
		}

	}

	if(ret == true)
	{
		DebugMap();
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
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
		std::string bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.size() > 0)
		{
			std::string red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MapTypes::MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MapTypes::MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MapTypes::MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MapTypes::MAPTYPE_UNKNOWN;
		}

		// load custom properties from map data
		pugi::xml_node propertiesNode = map.child("properties");

		if (propertiesNode == NULL)
		{
			LOG("Map without custom properties");
		}
		else
		{
			LoadProperties(map, data.properties);
		}

	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;

	set->name = tileset_node.attribute("name").as_string();
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();

	pugi::xml_node offset = tileset_node.child("tileoffset");
	if(offset != NULL)
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

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->LoadTexture(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->columns = set->tex_width / set->tile_width;
		set->rows = set->tex_height / set->tile_height;
	}

	//Loading animation
	//Currently each tileset can only hold one animation - tiled map editor restriction
	if (tileset_node.child("tile").child("animation")) {
		set->anim = new Animation;
		
		for (pugi::xml_node frame_node = tileset_node.child("tile").child("animation").child("frame"); frame_node; frame_node = frame_node.next_sibling()) {
			set->anim->PushBack(set->GetTileRect(frame_node.attribute("tileid").as_int() + set->firstgid));
		}
		pugi::xml_node speed_node = tileset_node.child("tile").child("animation").child("frame");
		set->anim->speed = speed_node.attribute("duration").as_float() * set->animSpeedFactor;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->columns = node.attribute("width").as_int();
	layer->rows = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->tileArray = new uint[layer->columns*layer->rows];
		memset(layer->tileArray, 0, layer->columns*layer->rows);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->tileArray[i++] = tile.attribute("gid").as_int(0);
		}
	}

	// check custom properties from layer - test functionality // TODO, implement latest handout to do this
	/*pugi::xml_node properties = node.child("properties");
	if (properties == NULL)
	{
		LOG("This layer doesnt have custom properties defined");
	}
	else
	{
		layer->parallaxSpeed = properties.child("property").attribute("value").as_float();
		LOG("%f", layer->parallaxSpeed);
	}*/

	return ret;
}

bool j1Map::LoadMapColliders(pugi::xml_node& node)//, MapObjects* obj)
{
	bool ret = true;

	SDL_Rect colliderRect;
	
	// iterate all objectgroups
	for (pugi::xml_node objectGroup = node.child("objectgroup"); objectGroup && ret; objectGroup = objectGroup.next_sibling("objectgroup"))
	{
		p2SString tmp = objectGroup.attribute("name").as_string();
		MapObjects* newObject = new MapObjects();

		newObject->name = tmp.GetString();

		// Load custom properties
		LoadProperties(objectGroup, newObject->properties);

		//bool counted = false;

		// iterate all objects
		int i = 0; // to allocate i colliders at once on new MapObject pointer for map reference?, not implemented yet

		for (pugi::xml_node object = objectGroup.child("object"); object; object = object.next_sibling("object"))
		{
			// count the num of objects
			/*if (!counted)
			{
				for (pugi::xml_node objectCounter = objectGroup.child("object"); object; object = object.next_sibling("object"))
				{
					++i;
				}
				counted = true;
			}*/

			colliderRect.x = object.attribute("x").as_int(0);
			colliderRect.y = object.attribute("y").as_int(0);
			colliderRect.h = object.attribute("height").as_int(0);
			colliderRect.w = object.attribute("width").as_int(0);
			// create collider type of
			if (tmp == "Platforms")
				newObject->colliders[i] = App->collision->AddCollider(colliderRect, COLLIDER_WALL);
			else if (tmp == "Floor")
				newObject->colliders[i] = App->collision->AddCollider(colliderRect, COLLIDER_WALL); // reference collider type for test
			else if (tmp == "Death")
				newObject->colliders[i] = App->collision->AddCollider(colliderRect, COLLIDER_DEATH_ZONE);
			else if (tmp == "Win")
				newObject->colliders[i] = App->collision->AddCollider(colliderRect, COLLIDER_WIN_ZONE);
			else if (tmp == "Glass")
				newObject->colliders[i] = App->collision->AddCollider(colliderRect, COLLIDER_GLASS);

			// increments counter
			++i;
		}
		
		// add object to list
		data.mapObjects.add(newObject);
	}

	return ret;
}

bool j1Map::Reset()
{
	map_loaded = false;
	// TODO: maybe we need to search a less ugly workaround to restart scene
	//App->scene->Disable();
	//App->scene->Enable();

	if (CleanUp()) {
		return true;
	}

	return false;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = true;

	pugi::xml_node propertiesNode = node.child("properties");

	if (propertiesNode == NULL)
	{
		LOG("properties not found");
		ret = false;
	}
	else
	{
		properties.draw = propertiesNode.find_child_by_attribute("name", "Draw").attribute("value").as_bool(true);
		properties.navigation = propertiesNode.find_child_by_attribute("name", "Navigation").attribute("value").as_bool(false);
		properties.testValue = propertiesNode.find_child_by_attribute("name", "testValue").attribute("value").as_int(0);
		properties.parallaxSpeed = propertiesNode.find_child_by_attribute("name", "parallaxSpeed").attribute("value").as_float(1.0F);
		properties.music_name = propertiesNode.find_child_by_attribute("name", "background_music").attribute("value").as_string();
		//properties.fx_name = propertiesNode.find_child_by_attribute("name", "background_music").attribute("value").as_string();
		
		// associated gui xml with this map ----
		properties.gui_xml_path = propertiesNode.find_child_by_attribute("name", "associated_gui_xml").attribute("value").as_string();
	}

	return ret;
}

bool j1Map::LoadGameObjects(pugi::xml_node& node)
{
	// iterate all objectgroups
	for (pugi::xml_node objectGroup = node.child("objectgroup"); objectGroup; objectGroup = objectGroup.next_sibling("objectgroup"))
	{
		p2SString tmp = objectGroup.attribute("name").as_string();
		LOG("%s", tmp.GetString());
		// Load custom properties
		//LoadProperties(objectGroup, newObject->properties);

		if (tmp == "GameObjects") // gameobjects maybe needs load from save, check this
		{
			// DYNAMIC GAMEOBJECTS
			if (!App->readyToLoad) // if the new map is loaded from save game, load gameobjects from savegame file
			{
				// iterate all objects
				for (pugi::xml_node object = objectGroup.child("object"); object; object = object.next_sibling("object"))
				{

					p2SString gameobject_name = object.attribute("name").as_string();
					LOG("%s", gameobject_name.GetString());

					if (gameobject_name == "normalBox") {
						//Box have their pivot point on ther bottom - middle
						SDL_Rect boxRect;
						boxRect.x = 0;
						boxRect.y = 0;
						boxRect.w = object.attribute("width").as_float();
						boxRect.h = object.attribute("height").as_float();
						App->object->AddObjBox( { object.attribute("x").as_float() + object.attribute("width").as_float() * 0.5f, object.attribute("y").as_float() + object.attribute("height").as_float() }, 
							boxRect,
							object.attribute("id").as_int());
					}
					else if (gameobject_name == "flyingEnemy") {
						App->object->AddObjEnemyFlying({ object.attribute("x").as_float(), object.attribute("y").as_float() });
					}
					else if (gameobject_name == "landEnemy") {
						App->object->AddObjEnemyLand({ object.attribute("x").as_float(), object.attribute("y").as_float() });
					}
					else if (gameobject_name == "pizza") {
						App->object->AddObjPizza({ object.attribute("x").as_float(), object.attribute("y").as_float() }, object.attribute("id").as_int());
					}
					else if (gameobject_name == "nutCoin") {
						App->object->AddObjNutCoins({ object.attribute("x").as_float(), object.attribute("y").as_float() }, object.attribute("id").as_int());
					}
				}
			}

			// STATIC GAMEOBJECTS
			// iterate all objects
			for (pugi::xml_node object = objectGroup.child("object"); object; object = object.next_sibling("object"))
			{

				p2SString gameobject_name = object.attribute("name").as_string();
				LOG("%s", gameobject_name.GetString());

				if (gameobject_name == "Door") {
					//Box have their pivot point on ther bottom - middle
					App->object->AddObjDoor({ object.attribute("x").as_float(),
											 object.attribute("y").as_float() },
						object.attribute("id").as_int());
				}
			}
		}
		if (tmp == "Triggers")
		{
			LOG("Trigger found");

			// iterate all objects
			for (pugi::xml_node object = objectGroup.child("object"); object; object = object.next_sibling("object"))
			{
				p2SString gameobject_type = object.attribute("type").as_string();
				//LOG("%s", gameobject_type.GetString());
				if (gameobject_type == "standard")
				{
					if (object.child("properties"))
					{
						// creates trigger
						ObjTrigger* ot = App->object->AddObjTrigger({ object.attribute("x").as_float(), object.attribute("y").as_float() },
							{ object.attribute("width").as_int(), object.attribute("height").as_int() });
						LOG("trigger properties found");

						// adds all triggered id's
						for (pugi::xml_node properties = object.child("properties").child("property"); properties; properties = properties.next_sibling("property"))
						{
							p2SString propertyName = properties.attribute("name").as_string();
							LOG("property %s", propertyName.GetString());

							//if (propertyName == "triggerObjectID")
							//{
								int linkedID = properties.attribute("value").as_int();
								ot->objectsEventIDs.PushBack(linkedID);
							//}
						}
					}
				}
			}
		}
	}

	return true;
}

int j1Map::GetTileWidth() const
{
	return data.tile_width;
}

bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	p2List_item<MapLayer*>* item;
	item = data.mapLayers.start;

	for (item = data.mapLayers.start; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if (layer->properties.navigation == false)
			continue;

		uchar* map = new uchar[layer->columns*layer->rows];
		memset(map, 1, layer->columns*layer->rows);

		for (int y = 0; y < data.rows; ++y)
		{
			for (int x = 0; x < data.columns; ++x)
			{
				int i = (y*layer->columns) + x;

				int tile_id = layer->GetArrayPos(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

				if (tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 1 : 0;
					/*TileType* ts = tileset->GetTileType(tile_id);
					if(ts != NULL)
					{
						map[i] = ts->properties.Get("walkable", 1);
					}*/
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
void j1Map::DebugMap(){
	LOG("Successfully parsed map XML file: %s", data.loadedLevel);
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
