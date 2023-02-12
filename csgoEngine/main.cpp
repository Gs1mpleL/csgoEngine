#include "memory.h"
#include "cssdk.h"
#include "renderer.h"
#include<iostream>
using namespace std;
using namespace memory_tool;
vector<player_struct> player_struct_list(64);
vector<esp_struct> esp_struct_list(64);
memory_tool::memory_manager* mem_ptr;

// 多线程执行信息更新
void read_thread() {
	cout << "寻找数据开始！" << endl;
	while (true)
	{
		// 更新本地玩家信息
		vector<uintptr_t> my_info_vec = { offsets::local_player_offset };
		int my_info_address;
		mem_ptr->read_chain<int>(mem_ptr->client_dll, my_info_vec, my_info_address);
		player_info my_info(mem_ptr, my_info_address);

		// 更新所有敌方玩家信息
		mem_ptr->read_list(mem_ptr->client_dll + offsets::entity_list_offset, player_struct_list.data(), player_struct_list.size());
		for (int i = 0; i < player_struct_list.size(); i++)
		{
			esp_struct_list[i].valid = false;
			uintptr_t address = player_struct_list[i].player_ptr;
			player_info player_info(mem_ptr, address);
			if (player_info.dormant == 1)
			{
				continue;
			}

			
			if (player_info.health <= 0 || player_info.health > 100)
			{
				continue;
			}
			
			if (my_info.team == 0 ||  player_info.team == my_info.team)
			{
				continue;
			}
			esp_struct esp;
			esp.health = player_info.health;
			esp.valid = true;
			esp.pos = player_info.pos;
			esp.top = player_info.pos + vec3(0.f, 0.f, 70.f);
			esp_struct_list[i] = esp;
		}
	}
}


int main() {	

	// 内存管理初始化
	memory_tool::memory_manager mem("csgo.exe");    // 进程绑定，基址确定
	mem_ptr = &mem;
	cout << "内存管理初始化成功！" << endl;
	// 渲染设备初始化
	HWND target_hwnd = FindWindow("Valve001", "Counter-Strike: Global Offensive - Direct3D 9"); // 窗口初始化
	render::overlay overlay("Counter-Strike: Global Offensive - Direct3D 9", "Valve001");
	render::dx_renderer dx(overlay.m_device);
	cout << "渲染设备初始化成功！" << endl;
	// 开始更新游戏信息
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)read_thread, NULL, 0, NULL);

	// 绘制敌方方框
	cout << "绘制方框开始！" << endl;
	while (true)
	{
		dx.begin_renderer();

		for (esp_struct esp : esp_struct_list) {
			
			if (!esp.valid) {
				continue;
			}
			if (esp.health <= 0 || esp.health > 100) {
				continue;
			}
			// 矩阵
			vec_view view = mem.read<vec_view>(mem.client_dll + offsets::view_matrix_offset);
			vec3 v1, v2;
			
			if (!world2screen(esp.pos,v1,view, overlay.target_rect.width(),overlay.target_rect.height() ) || !world2screen(esp.top, v2, view, overlay.target_rect.width(), overlay.target_rect.height()))
			{
				continue;
			}
			int box_height = v1.y - v2.y;
			int box_weight = box_height / 2;
			// 绘制方框
			 dx.draw_rect(v2.x - box_weight / 2, v2.y, box_weight, box_height, D3DCOLOR_RGBA(229,36, 36, 255));
			// 绘制血量
			dx.draw_text(to_string(esp.health), v1.x, v1.y, D3DCOLOR_RGBA(229, 36, 36, 255));
		}
		dx.end_renderer();
	}
	return 0;
}
