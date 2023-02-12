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
	int health;   // ���Ѫ��
	bool dormant; 
	int team;  // �����Ӫ
	vec3 pos,top;
};

class player_struct {
public:
	int player_ptr;   // ��Ҿ�������ָ��
	int id;
	int pre_player_ptr; // ��һ����ҵĽṹ��ָ��
	int next_player_prt; // ��һ����ҵĽṹ��ָ��
};

// �ӽǾ���
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