#include "Camera.h"
#include "Obj_Tank.h"
#include "M_Map.h"

Camera::Camera()
{
	max_shake_offset = 15.f;
	trauma_decay = 0.9f;
	random_generator.seed(random_device());
	aim_distance = 2.5f;
	lerp_factor = 3.75f;
}


void Camera::AddTrauma(float value)
{
	if (trauma + value >= 1.f)
	{
		trauma = 1.f;
	}
	else
	{
		trauma += value;
	}
}

void Camera::ResetCamera()
{
	rect.x = unaltered_pos.x;
	rect.y = unaltered_pos.y;
}

void Camera::ShakeCamera(float dt)
{
	//Keep the original position
	unaltered_pos = camera_pos;

	//Apply the camera shake
	if (trauma > 0.f)
	{
		float shake = GetShakeAmount();
		camera_pos.x += max_shake_offset * shake * GetRandomValue(-1.f, 1.f);
		camera_pos.y += max_shake_offset * shake * GetRandomValue(-1.f, 1.f);

		//Reduce trauma
		trauma -= trauma_decay * dt;
	}
}

//Shake amout is based on trauma
//It is not linearly dependant on trauma, and instead its trauma^2 because it feels better that with more trauma there is more shake
float Camera::GetShakeAmount() const
{
	return trauma * trauma;
}

void Camera::FollowPlayer(float dt, Obj_Tank * player)
{
	fPoint aim_pos = app->map->MapToScreenF(player->pos_map + player->GetShotDir() * aim_distance);
	fPoint target_pos (aim_pos.x - rect.w * 0.5f, aim_pos.y - rect.h * 0.5f);

	camera_pos = lerp(camera_pos, target_pos, dt * lerp_factor);

	rect.x = camera_pos.x;
	rect.y = camera_pos.y;
}

//Remove
	//float line_length = 5.f;
	////1-- Set a position in the isometric space
	//fPoint input_iso_pos(turr_pos.x + shot_dir.x * line_length, turr_pos.y + shot_dir.y * line_length);
	////2-- Transform that poin to screen coordinates
	//iPoint input_screen_pos = (iPoint)app->map->MapToScreenF(input_iso_pos);
	//app->render->DrawLineSplitScreen(
	//	pos_screen.x, pos_screen.y - cannon_height,
	//	input_screen_pos.x, input_screen_pos.y, 255, 255, 255, 123, camera);
//}

float Camera::GetRandomValue(float min_value, float max_value)
{
	std::uniform_real_distribution<float> dis(min_value, std::nextafter(max_value, FLT_MAX));

	return dis(random_generator);
}