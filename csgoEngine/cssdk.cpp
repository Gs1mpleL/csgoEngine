#include "cssdk.h"
bool world2screen(vec3 pos, vec3& out, vec_view matrix,int gameX,int gameY) {

	out.x = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];
	out.y = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];
	float w = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];
	if (w < 0.01f)
	{
		return false;
	}
	float inv_w = 1.f / w;
	out.x *= inv_w;
	out.y *= inv_w;
	float screen_x = gameX * 0.5f;
	float screen_y = gameY * 0.5f;
	screen_x += 0.5 * out.x * gameX;
	screen_y -= 0.5 * out.y * gameY;
	out.x = screen_x;
	out.y = screen_y;
	return true;
}
vec3::vec3()
{
	x = 0;
	y = 0;
	z = 0;
}

vec3::vec3(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

vec3 vec3::operator+(const vec3& other)
{
	return {x + other.x ,y + other.y, z+other.z};
}

vec3 vec3::operator+=(const vec3& other)
{
	x = x + other.x;
	y = y + other.y;
	z = z + other.z;
	return *this;
}

player_info::player_info(memory_tool::memory_manager* mem, uintptr_t address)
{
	dormant = mem->read<bool>(address + offsets::dormant_offset);
	health = mem->read<int>(address + offsets::health_offset);
	pos = mem->read<vec3>(address + offsets::pos_offset);
	team = mem->read<int>(address + offsets::team_offset);
}