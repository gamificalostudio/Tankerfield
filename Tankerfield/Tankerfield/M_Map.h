#ifndef __M_MAP_H__
#define __M_MAP_H__

#include <list>

#include "Log.h"
#include "Rect.h"
#include "Quadtree_Map.h"

#include "Module.h"
#include "M_Render.h"
#include "M_Textures.h"


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

	std::string GetAsString(const char* name, std::string default_value = "") const;
	
	int  GetAsInt(const char* name, int default_value = 0) const;
	
	float GetAsFloat(const char* name, float default_value = 0) const;

	
	bool GetAsBool(const char* name, bool default_value = false) const;

	void LoadProperties(pugi::xml_node propertie_node);

	bool draw = true;
	void UnloadProperties();
};

// ----------------------------------------------------
struct MapLayer
{
	std::string	name;
	int			columns = NULL;
	int			rows = NULL;
	uint*		data = nullptr;
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
struct ObjectGroup
{
	std::string name;
	Properties	properties;
	uint size			= 0;
	Rect<float, float>* objects	= nullptr;
	~ObjectGroup()
	{
		if (objects != nullptr)
		{
			delete[] objects;
			objects = nullptr;
		}
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
	int					firstgid = NULL;
	int					margin = NULL;
	int					spacing = NULL;
	int					tile_width = NULL;
	int					tile_height = NULL;
	SDL_Texture*		texture = nullptr;
	int					tex_width = NULL;
	int					tex_height = NULL;
	int					columns = NULL;
	int					rows = NULL;
	int					offset_x = NULL;
	int					offset_y = NULL;



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
	std::string			objects_path;
	int					columns = NULL,	rows = NULL;
	int					tile_width = NULL,	tile_height = NULL;
	int					offset_x = NULL, offset_y = NULL;
  
	MapTypes			type;
	SDL_Color			background_color;

	std::list<TileSet*>		tilesets;
	std::list<MapLayer*>	map_layers;
	std::list<Collider*>    colliders_list;
	std::list<ObjectGroup*> object_layers;
	
	Properties				map_properties;

	
	iRect*		screen_tile_rect = nullptr;
	Quadtree_Map*				qt = nullptr;

	~MapData()
	{
		if (screen_tile_rect != nullptr)
		{
			delete[] screen_tile_rect;
			screen_tile_rect = nullptr;
		}
	}
	
};

class M_Map : public Module
{
public:
	M_Map();

	~M_Map();

	bool Awake(pugi::xml_node&) override;

	bool Update(float dt) override;

	bool PostUpdate(float dt) override;
	
	bool CleanUp() override;

	bool Load(const std::string & file_name);

	bool Unload();

	iPoint MapToScreenI(int column, int row) const;

	fPoint MapToScreenF(const fPoint & map_pos);

	iPoint ScreenToMapI(int x, int y) const;

	fPoint ScreenToMapF(float x, float y);

	TileSet* GetTilesetFromTileId(int id) const;

	uint GetMaxLevels();
  
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

public:

	MapData							data;
	std::list<Levels*>				levels;
	
private:

	bool					map_loaded = false;
	bool					show_grid = false;
	std::string				folder;
	uint					numLevels = 0; // counter for num levels
	pugi::xml_document		map_file;

	bool LoadMap();

	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);

	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);

	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);

	bool LoadObjectGroup(const pugi::xml_node& object_group_node, ObjectGroup* object_group);

	void DebugMap();

};
#endif // __j1MAP_H__