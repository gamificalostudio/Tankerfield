#include "Camera.h"
#include "Obj_Tank.h"
#include "M_Map.h"

Camera::Camera()
{
	max_offset = 16.f;
	trauma_decay = 0.9f;
	random_generator.seed(random_device());
	lerp_factor = 5.f;
}


void Camera::AddTrauma(float value)
{
	if (trauma + value >= 1.0f)
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
	unaltered_pos.x = rect.x;
	unaltered_pos.y = rect.y;

	//Apply the camera shake
	if (trauma > 0)
	{
		float shake = GetShakeAmount();
		rect.x += max_offset * shake * GetRandomValue(-1.f, 1.f);
		rect.y += max_offset * shake * GetRandomValue(-1.f, 1.f);

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
	fPoint source_pos ((float)rect.x, (float)rect.y);
	fPoint player_pos = app->map->MapToScreenF(player->pos_map);
	fPoint target_pos(player_pos.x - rect.w * 0.5f, player_pos.y - rect.h * 0.5f);

	fPoint lerp_pos = lerp(source_pos, target_pos, lerp_factor * dt);
	rect.x = lerp_pos.x;
	rect.y = lerp_pos.y;
}

float Camera::GetRandomValue(float min_value, float max_value)
{
	std::uniform_real_distribution<float> dis(min_value, std::nextafter(max_value, FLT_MAX));

	return dis(random_generator);
}