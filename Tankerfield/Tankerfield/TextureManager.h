#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

#include <list>

struct SDL_Texture;

class TextureManager
{
public:
	SDL_Texture * LoadTexture(SDL_Texture * tex);
	bool RemoveTexture(SDL_Texture * tex);
	bool CleanTextures();

private:
	std::list<SDL_Texture*> textures;
};

#endif // !__TEXTURE_MANAGER_H__
