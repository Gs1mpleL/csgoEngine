#pragma once
#include"memory.h"
#include "offset.h"
struct vec3 {
public:
	vec3();
	float x = 0, y = 0, z = 0;
	vec3(float _x, float _y, float _z);
	vec3 operator+(const vec3& other);
	vec3 operator+=(const vec3& other);
};
class player_info {
public:
	player_info(memory_tool::memory_manager* mem, uintptr_t address);
	int health;   // 玩家血量
	bool dormant; 
	int team;  // 玩家阵营
	vec3 pos,top;
};

class player_struct {
public:
	int player_ptr;   // 玩家具体数据指针
	int id;
	int pre_player_ptr; // 上一个玩家的结构体指针
	int next_player_prt; // 下一个玩家的结构体指针
};

// 视角矩阵
struct vec_view {
	float vec_4[4][4];
	float * operator[](int index){
		return vec_4[index];
	}
};

// 
class esp_struct {
public:
	bool valid = false;
	int health = 0;
	vec3 pos;
	vec3 top;
};

bool world2screen(vec3 pos, vec3& out, vec_view matrix, int gameX, int gameY);