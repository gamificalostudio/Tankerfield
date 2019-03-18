#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "j1Collision.h"
#include "j1App.h"
#include "j1Textures.h"
#include "Animation.h"

struct Collider;


struct Scenes
{
	bool active=false;
	uint levelnum=0;
	p2SString level_tmx;
	p2SString musicPath;
	
};


struct Object_Layer
{
	p2SString			name;
	p2List<Collider*>		col;
	~Object_Layer()
	{
		for (p2List_item<Collider*>* colItem = col.end; colItem; colItem = colItem->prev )
		{
			colItem->data->to_delete = true;
			RELEASE(colItem->data);
		}

	}

};

struct Properties
{
	struct Property
	{
		p2SString name;
		void* value=nullptr;
	};

	~Properties()
	{
		p2List_item<Property*>* item;
		item = list.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.clear();
	}

	float GetAsFloat(const char* name, float default_value = 0) const;

	p2List<Property*>	list;
};
struct tile
{
	uint id = 0;
	Animation* anim=nullptr ;

};



struct MapLayer
{
	p2SString name = "";
	uint width = 0u;
	uint height = 0u;
	float parallax_velocity = 0.0f;
	bool visible = true;

	tile*		arrayOfIds = nullptr;
	Properties	properties;
	~MapLayer()
	{
		if (arrayOfIds != nullptr)
		{
			delete arrayOfIds;
			arrayOfIds = nullptr;
		}
	}

	inline uint Get(int x, int y) const
	{
		return arrayOfIds[(y*width) + x].id;
	}
};


// ----------------------------------------------------
struct Patern
{
	

	SDL_Rect GetTileRect(int id) const;

	
	p2SString				name = "";
	SDL_Texture*			texture = nullptr;
	uint					tex_width = 0;
	uint					tex_height = 0;

	uint					firstgid = 0;
	uint					margin = 0;
	uint					spacing = 0;

	uint					tile_width = 0;
	uint					tile_height = 0;

	uint					num_tiles_width  = 0;
	uint					num_tiles_height = 0;
	p2List<tile*>			ListStructId;


	~Patern()
	{
		if (texture != nullptr)
		{
			App->tex->UnLoad(texture);
		}
	}
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	float gravity = 300.0f;
	uint					width;
	uint					height;
	uint					tile_width;
	uint					tile_height;

	MapTypes				type = MAPTYPE_UNKNOWN;

	p2List<Patern*>			list_Patterns;
	p2List<MapLayer*>		layers;
	p2List<Object_Layer*>   collition_layers;
	p2List<Scenes*>			scenes_List;
	Properties	properties;
	
};

// ----------------------------------------------------
class j1Map : public j1Module
{
private:
	p2List_item<Scenes*>* atualSceneItem=nullptr;
public:

	MapData level;
	
	

	inline uint Get(int x, int y) const
	{
		return  (y * level.width + x);
	}

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf)override;
	bool Start() override;
	bool PreUpdate(float dt) override;
	bool Update(float dt) override;
	bool PostUpdate() override;
	

	// Called before quitting
	bool CleanUp() override;



	// Load new map
	bool Load(const char* path);
	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;
	
	p2List_item<Scenes*>* activateScene(uint lvlnum)
	{
		p2List_item<Scenes*>* item_scene;
		
		for(item_scene= level.scenes_List.start; item_scene; item_scene= item_scene->next   )
			{
			item_scene->data->active = false;
		}
		for (item_scene = level.scenes_List.start; item_scene->data->levelnum!=lvlnum && item_scene !=nullptr ; item_scene = item_scene->next)
		{	}
		item_scene->data->active = true;
		atualSceneItem = item_scene;
		return item_scene;
	}
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	bool CreateWalkabilityMap(int & width, int & height, uchar ** buffer) const;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;

	uint i = 0;
	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, Patern* set);
	bool LoadPaternImage_tile(pugi::xml_node& tileset_node, Patern* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadCollision(pugi::xml_node& coll_node, Object_Layer* collision);
	Patern* GetTilesetFromTileId(int id) const;
	/*void LoadProperties(pugi::xml_node& node);*/
	void LoadLayerProperties(pugi::xml_node& node, Properties& properties);

	SDL_Texture* debug_tex;

};



#endif // __j1MAP_H__