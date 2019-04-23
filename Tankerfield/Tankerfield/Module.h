#ifndef __MODULE_H__
#define __MODULE_H__

#include <string>
#include "PugiXml/src/pugixml.hpp"

class App;
class Collider;

class Module
{

private:
	bool enabled = false;

public:
	Module() : active(true)
	{}

	virtual ~Module()
	{}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate(float dt)
	{
		return true;
	}

	virtual bool Draw()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

	// Reset module 

	virtual bool Reset() 
	{
		return true;
	}


// Enable and disable

	bool IsEnabled() const { return enabled; }

	void Enable()
	{
		if (enabled == false)
		{
			active = true;
			enabled = true;
			Start();
		}
	}

	void Disable()
	{
		if (enabled == true)
		{
			active = false;
			enabled = false;
			CleanUp();
		}
	}

public:

	std::string	name;

	bool		active = false;

	friend App;
};

#endif // __j1MODULE_H__