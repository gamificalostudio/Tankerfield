#ifndef __M_MAP_H__
#define __M_MAP_H__
#include <list>
#include "Module.h"
#include "M_Render.h"
#include "M_Textures.h"
#include "Log.h"

struct Levels
{
	std::string name;
};

struct Properties
{

	struct Property
	{
		std::string name;
		void* value;
	};

	~Properties()
	{
		UnloadProperties();
	}

private:
	std::list<Property*>	list;
public:

	std::string GetAsString(const char* name, std::string default_value = "") const
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
	
	int         GetAsInt(const char* name, int default_value = 0) const
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
	
	float       GetAsFloat(const char* name, float default_value = 0) const
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
	
	bool       GetAsBool(const char* name, bool default_value = false) const
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

	void LoadProperties(pugi::xml_node propertie_node)
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

	void UnloadProperties();
};

// ----------------------------------------------------
struct MapLayer
{
	std::string	name;
	int			columns;
	int			rows;
	uint*		data;
	Properties	layer_properties;
	bool visible = true;
	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
		
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*columns) + x];
	}

	inline fPoint GetTilePos(int id) const
	{
		return fPoint(((id % columns)), ((id / columns)));
	}

};

// ----------------------------------------------------
struct TileSet
{
	~TileSet()
	{
		if(texture != nullptr)
			app->tex->UnLoad(texture);
	}

	SDL_Rect GetTileRect(int id) const;

	std::string			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					columns;
	int					rows;
	int					offset_x;
	int					offset_y;



};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

struct MapData
{
	int					columns, rows;
	int					tile_width,	tile_height;
	int					offset_x, offset_y;
	MapTypes			type;

	SDL_Color			background_color;

	std::list<TileSet*>	tilesets;
	std::list<MapLayer*>	mapLayers;
	std::list<Collider*>    colliders_list;

	Properties				map_properties;
};

class M_Map : public Module
{
public:
	M_Map();

	~M_Map();

	bool Awake(pugi::xml_node&) override;

	bool Update(float dt) override;
	
	bool CleanUp() override;

	bool Load(const std::string & file_name);

	bool Unload();

	iPoint MapToWorld(int column, int row) const;

	fPoint MapToWorldF(float x, float y);

	iPoint WorldToMap(int x, int y) const;

	fPoint WorldToMapF(float x, float y);

	TileSet* GetTilesetFromTileId(int id) const;

	uint GetMaxLevels();

public:

	MapData					data;
	std::list<Levels*>		levels;

private:

	bool					map_loaded;
	bool					show_grid = false;
	std::string				folder;
	uint					numLevels = 0; // counter for num levels
	pugi::xml_document		map_file;

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	void DebugMap();
};
#endif // __j1MAP_H__