#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Scene.h"

j1Scene::j1Scene() : j1Module()
{
	name.assign("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	texture = App->tex->Load("Tesla trooper/shk-sheet.png");

	tesla_trooper_rects = new SDL_Rect[8];
	tesla_trooper_rects[0] = {   0, 0, 66, 76 };
	tesla_trooper_rects[1] = {  66, 0, 66, 76 };
	tesla_trooper_rects[2] = { 132, 0, 66, 76 };
	tesla_trooper_rects[3] = { 198, 0, 66, 76 };
	tesla_trooper_rects[4] = { 264, 0, 66, 76 };
	tesla_trooper_rects[5] = { 330, 0, 66, 76 };
	tesla_trooper_rects[6] = { 396, 0, 66, 76 };
	tesla_trooper_rects[7] = { 462, 0, 66, 76 };

	player = {0,0,76,66};
	
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{


	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= floor(200.0f * dt);

	// Draw ------------------
	App->render->Blit(texture, 20, 20, GetRotatedSprite(tesla_trooper_rects,8,90));

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");



	return true;
}

//The angle should be in degrees and rect_num is the number of rectangles that are inside rect
SDL_Rect* j1Scene::GetRotatedSprite(SDL_Rect* rect, int rect_num, float angle, float fist_rect_dir)
{
	angle -= fist_rect_dir;

	if (angle > 360) {
		angle = fmod(angle, 360);
	}
	else if (angle < -360) {
		angle = fmod(angle, -360);
	}

	if (angle < 0)
		angle += 360;

	float angle_part = 360 / rect_num;

	float num_pos = angle / angle_part;

	float remainder = fmod(num_pos, 1);

	if (remainder >= 0.5f)
		num_pos = num_pos - remainder + 1;
	else
		num_pos = num_pos - remainder;

	if (num_pos == rect_num)
		num_pos = 0;

	return &rect[(int)num_pos];
}
