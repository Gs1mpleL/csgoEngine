#pragma once
#include<string>
#include<Windows.h>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#include<d3d9.h>
#include<d3dx9.h>
namespace render {
	class overlay {
		struct wnd_rec : RECT {
			int width() {
				return right - left;
			}
			int height() {
				return bottom - top;
			}
		};

	public:
		wnd_rec overlay_rect;
		wnd_rec target_rect;

		IDirect3DDevice9* m_device = nullptr;
		overlay(std::string_view window_name, std::string_view class_name);
		~overlay();
		void create_overlay(std::string_view window_name, std::string_view class_name);
	private:
		static LRESULT CALLBACK m_win_proc(HWND hwmd, UINT msg, WPARAM vparam, LPARAM lparam);

		void init_dx9();
		IDirect3D9* m_d3d = nullptr;
		bool m_initialized = false;
	};

	class dx_renderer {
	public:
		dx_renderer(IDirect3DDevice9* mdevice);
		~dx_renderer();
		void begin_renderer();
		void end_renderer();
		void draw_rect(int x0, int y0,int w,int h,unsigned long color);
		void draw_line(int x0, int y0,int x1,int y1,unsigned long color);
		void draw_text(std::string_view text,int x,int y,unsigned long color);
	private:
		IDirect3DDevice9* d3d_device;
		ID3DXFont* m_font = nullptr;
		ID3DXLine* m_line = nullptr;

	};
}