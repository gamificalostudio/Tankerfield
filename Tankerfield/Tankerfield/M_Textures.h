#ifndef __M_TEXTURES_H__
#define __M_TEXTURES_H__

#include "Module.h"
#include <map>
#include <string>

struct SDL_Texture;
struct SDL_Surface;

enum Textures
{
	TANK_BASE,
	TANK_TURR,


};

class M_Textures : public Module
{
public:

	M_Textures();

	// Destructor
	virtual ~M_Textures();

	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	// Called before the first frame
	bool Start() override;

	// Called before quitting
	bool CleanUp() override;

	// Load Texture
	SDL_Texture* const	Load(const char* path);
	bool				UnLoad(SDL_Texture* texture, bool is_text_texture = false);
	SDL_Texture * const LoadTextSurface(SDL_Surface* surface);
	void				GetSize(const SDL_Texture* texture, uint& width, uint& height) const;

private:
	SDL_Texture* const	LoadSurface(SDL_Surface* surface, std::string path);

public:
			//Path		  //Texture pointer
	std::map<std::string, SDL_Texture *> textures;
	std::list<SDL_Texture*> text_textures;
};


#endif // __j1TEXTURES_H__