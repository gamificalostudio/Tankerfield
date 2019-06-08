
#include "Log.h"
#include "j1ParticleSystem.h"
#include "App.h"
#include "M_Textures.h"



j1ParticleSystem::j1ParticleSystem() : Module()
{
	name = "psystem";

	LOG("Loading Languages");
	
}

j1ParticleSystem::~j1ParticleSystem()
{
	
}

bool j1ParticleSystem::Awake(pugi::xml_node& config)
{
	bool ret = true;

	pugi::xml_document	psystem_config;
	pugi::xml_node* node = &app->LoadEmitters(psystem_config);
	nameParticleAtlas = node->child("particleAtlas").attribute("name").as_string();
	
	for (pugi::xml_node emitters = node->child("particleAtlas").child("emitter"); emitters && ret; emitters = emitters.next_sibling("emitter"))
	{
		std::string emitterType = emitters.attribute("type").as_string();

		if (emitterType == "fire") {
			LoadEmitterData(emitters, EmitterType::EMITTER_TYPE_FIRE);
		}
	}
	return ret;
}

bool j1ParticleSystem::Start()
{
	particleAtlas = app->tex->Load(nameParticleAtlas.c_str());

	return true;
}

bool j1ParticleSystem::PreUpdate()
{
	return true;
}

bool j1ParticleSystem::Update(float dt)
{
	for (std::list<Emitter*>::const_iterator it = emitters_list.begin(); it != emitters_list.end(); ++it)
	{
		if ((*it) != nullptr)
		{
			(*it)->Update(dt);
		}
	}

	return true;
}

bool j1ParticleSystem::PostUpdate()
{
	for (std::list<Emitter*>::const_iterator it = emitters_list.begin(); it != emitters_list.end();)
	{
		if ((*it)->to_destroy)
		{
			delete (*it);
			it = emitters_list.erase(it);
		}
		else
		{
			++it;
		}
	}

	return true;
}

bool j1ParticleSystem::CleanUp()
{
	LOG("Freeing emitters from the system.");

	for (std::list<Emitter*>::const_iterator it = emitters_list.begin(); it != emitters_list.end(); ++it)
	{
		if ((*it) != nullptr)
		{
			delete (*it);
		}
	}

	emitters_list.clear();
	app->tex->UnLoad(particleAtlas);

	return true;
}

Emitter* j1ParticleSystem::AddEmiter(fPoint pos, EmitterType type)
{
	Emitter* tmp_emitter = new Emitter(pos, vecEmitterData[type]);

	emitters_list.push_back(tmp_emitter);
	
	return tmp_emitter;
}

bool j1ParticleSystem::RemoveEmitter(Emitter & emitter)
{
	for (std::list<Emitter*>::const_iterator it = emitters_list.begin(); it != emitters_list.end(); ++it)
	{
		if ((*it) == &emitter)
		{
			(*it)->to_destroy = true;
			return true;
		}
	}

	return false;
}

bool j1ParticleSystem::RemoveAllEmitters()
{
	bool ret = false;

	std::list<Emitter*>::const_iterator it;

	for (it = emitters_list.begin(); it != emitters_list.end(); ++it)
	{
		if ((*it) != nullptr) (*it)->to_destroy = true;
		ret = true;
	}

	return ret;
}
 
SDL_Texture* j1ParticleSystem::GetParticleAtlas() const
{
	return particleAtlas;
}

void j1ParticleSystem::LoadEmitterData(pugi::xml_node & emitter, EmitterType type)
{
	EmitterData tmp;

	// Angle range
	tmp.angleRange.x = emitter.child("angleRange").attribute("min").as_float();
	tmp.angleRange.y = emitter.child("angleRange").attribute("max").as_float();

	// Particle speeds
	tmp.rotSpeed = emitter.child("rotSpeed").attribute("value").as_double();
	tmp.startSpeed = emitter.child("startSpeed").attribute("value").as_float();
	tmp.endSpeed = emitter.child("endSpeed").attribute("value").as_float();

	// Random controls
	tmp.startSpeedRand.x = emitter.child("startSpeed").attribute("randMin").as_float();
	tmp.startSpeedRand.y = emitter.child("startSpeed").attribute("randMax").as_float();

	tmp.endSpeedRand.x = emitter.child("endSpeed").attribute("randMin").as_float();
	tmp.endSpeedRand.y = emitter.child("endSpeed").attribute("randMax").as_float();

	tmp.rotSpeedRand.x = emitter.child("rotSpeed").attribute("randMin").as_float();
	tmp.rotSpeedRand.y = emitter.child("rotSpeed").attribute("randMax").as_float();

	tmp.emitVarianceRand.x = emitter.child("emitVariance").attribute("randMin").as_float();
	tmp.emitVarianceRand.y = emitter.child("emitVariance").attribute("randMax").as_float();

	tmp.lifeRand.x = emitter.child("maxParticleLife").attribute("randMin").as_float();
	tmp.lifeRand.y = emitter.child("maxParticleLife").attribute("randMax").as_float();

	tmp.startSizeRand.x = emitter.child("startSize").attribute("randMin").as_float();
	tmp.startSizeRand.y = emitter.child("startSize").attribute("randMax").as_float();

	tmp.endSizeRand.x = emitter.child("endSizeRand").attribute("randMin").as_float();
	tmp.endSizeRand.y = emitter.child("endSizeRand").attribute("randMax").as_float();

	// Particle size
	tmp.startSize = emitter.child("startSize").attribute("value").as_float();
	tmp.endSize = emitter.child("endSize").attribute("value").as_float();

	// Emission properties
	tmp.emitNumber = emitter.child("emitNumber").attribute("value").as_uint();
	tmp.emitVariance = emitter.child("emitVariance").attribute("value").as_uint();
	
	// Particle life
	tmp.maxParticleLife = emitter.child("maxParticleLife").attribute("value").as_uint();
	
	// Rect from particle atlas
	tmp.textureRect.x = emitter.child("textureRect").attribute("x").as_int();
	tmp.textureRect.y = emitter.child("textureRect").attribute("y").as_int();
	tmp.textureRect.w = emitter.child("textureRect").attribute("w").as_int();
	tmp.textureRect.h = emitter.child("textureRect").attribute("h").as_int();
	
	// Lifetime of emitter
	tmp.lifetime = emitter.child("lifetime").attribute("value").as_double();

	// Start color
	tmp.startColor.r = emitter.child("startColor").attribute("r").as_uint();
	tmp.startColor.g = emitter.child("startColor").attribute("g").as_uint();
	tmp.startColor.b = emitter.child("startColor").attribute("b").as_uint();
	tmp.startColor.a = emitter.child("startColor").attribute("a").as_uint();

	// End color
	tmp.endColor.r = emitter.child("endColor").attribute("r").as_uint();
	tmp.endColor.g = emitter.child("endColor").attribute("g").as_uint();
	tmp.endColor.b = emitter.child("endColor").attribute("b").as_uint();
	tmp.endColor.a = emitter.child("endColor").attribute("a").as_uint();

	// Blend mode
	std::string blendModeString = emitter.child("blendMode").attribute("mode").as_string();

	if (blendModeString == "add")
		tmp.blendMode = SDL_BlendMode::SDL_BLENDMODE_ADD;
	else if (blendModeString == "blend")
		tmp.blendMode = SDL_BlendMode::SDL_BLENDMODE_BLEND;
	else if (blendModeString == "mod")
		tmp.blendMode = SDL_BlendMode::SDL_BLENDMODE_MOD;
	else if (blendModeString == "none")
		tmp.blendMode = SDL_BlendMode::SDL_BLENDMODE_NONE;

	vecEmitterData[type] = tmp;
}