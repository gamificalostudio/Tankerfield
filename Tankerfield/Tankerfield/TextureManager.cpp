#include "TextureManager.h"

SDL_Texture * TextureManager::LoadTexture(SDL_Texture * tex)
{
	return textures.push_back(tex);
}

bool TextureManager::RemoveTexture(SDL_Texture * tex)
{
	for (std::list<SDL_Texture*>::iterator iterator = textures.begin(); iterator != textures.end(); ++iterator)
	{
		if ((*iterator) != nullptr && (*iterator) != tex)
		{
			delete (*iterator);
			(*iterator) = nullptr;
			break;
		}
	}
}

bool TextureManager::CleanTextures()
{
	for (std::list<SDL_Texture*>::iterator iterator = textures.begin(); iterator != textures.end();)
	{
		//Delete textures
		//Point to nullptr
	}
}
