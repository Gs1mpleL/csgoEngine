#include "renderer.h"
#include<stdlib.h>
#pragma comment(lib,"dwmapi.lib")
#include<dwmapi.h>
#include<iostream>
using namespace std;


static HWND target_hwnd;
static HWND overlay_hwnd;
render::overlay::overlay(std::string_view window_name, std::string_view class_name)
{
	if (window_name.empty() || class_name.empty())
	{
		throw std::exception("窗口名称为空");
	}
	create_overlay(window_name, class_name);
}

render::overlay::~overlay()
{
}

void render::overlay::create_overlay(std::string_view window_name, std::string_view class_name)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof WNDCLASSEX;
	wc.lpfnWndProc = m_win_proc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = NULL;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = HBRUSH(RGB(0, 0, 0));
	wc.lpszMenuName = LPCSTR("");
	wc.lpszClassName = LPCSTR("overlay");
	wc.hIconSm = NULL;
	if (!RegisterClassExA(&wc))
	{
		throw std::exception("窗口注册失败");
	}
	target_hwnd = FindWindow(class_name.data(), window_name.data());
	cout << "3秒后获取屏幕参数" << endl;
	Sleep(3000);
	
	GetWindowRect(target_hwnd, &target_rect);
	cout << "获取屏幕参数: w = " << target_rect.width() << "  h = " << target_rect.height() << endl;
	overlay_hwnd = CreateWindowExA(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, LPCSTR("overlay"), LPCSTR(""), WS_VISIBLE | WS_POPUP, target_rect.left, target_rect.top, target_rect.width(), target_rect.height(), NULL, NULL, NULL, NULL);
	MARGINS m = {
		target_rect.left, target_rect.top, target_rect.width(), target_rect.height()
	};

	// 让窗口透明
	DwmExtendFrameIntoClientArea(overlay_hwnd, &m);
	SetLayeredWindowAttributes(overlay_hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

	// 窗口展示
	ShowWindow(overlay_hwnd, SW_SHOW);

	// 初始化d3d9
	init_dx9();

}

LRESULT render::overlay::m_win_proc(HWND hwmd, UINT msg, WPARAM vparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		exit(0);
		break;
	default:
		return DefWindowProc(hwmd, msg, vparam, lparam);
	}

}

void render::overlay::init_dx9()
{
	
	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp, sizeof(pp));
	pp.Windowed = true;
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp.BackBufferFormat = D3DFMT_A8R8G8B8;
	pp.BackBufferWidth = target_rect.width();
	pp.BackBufferHeight = target_rect.height();
	pp.hDeviceWindow = overlay_hwnd;
	pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	if (FAILED(m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, overlay_hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &m_device))){
		throw std::exception("m_device设备初始化失败");
	}
	m_initialized = true;
}


render::dx_renderer::dx_renderer(IDirect3DDevice9* mdevice)
{
	if (mdevice == NULL)
	{
		throw std::exception("mdevice为空");
	}


	d3d_device = mdevice;
	if (FAILED(D3DXCreateLine(d3d_device, &m_line))) {
		throw std::exception("线条初始化失败");
	}
	if (FAILED(D3DXCreateFontA(d3d_device,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,"Tahoma",&m_font)))
	{
		throw std::exception("字体初始化失败");
	}



}

render::dx_renderer::~dx_renderer()
{
	if (m_line)
	{
		m_line->Release();
	}
	if (m_font)
	{
		m_font->Release();
	}
}

void render::dx_renderer::begin_renderer()
{
	// 清除
	d3d_device->Clear(NULL, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 0), 1.f, 0);

	d3d_device->BeginScene();
}

void render::dx_renderer::end_renderer()
{
	d3d_device->EndScene();
	d3d_device->Present(NULL, NULL, NULL, NULL);
}

void render::dx_renderer::draw_rect(int x0, int y0, int w, int h, unsigned long color)
{
	draw_line(x0, y0, x0 + w, y0,color);
	draw_line(x0, y0, x0, y0 + h,color);
	draw_line(x0 + w, y0, x0 + w, y0 + h,color);
	draw_line(x0, y0+h, x0+w, y0 + h,color);
}

void render::dx_renderer::draw_line(int x0, int y0, int x1, int y1, unsigned long color)
{
	D3DXVECTOR2 line[2] = { D3DXVECTOR2(x0,y0),D3DXVECTOR2(x1,y1) };
	m_line->Begin();
	m_line->Draw(line, 2, color);
	m_line->End();
}

LPCWSTR string2LPCWSTR(std::string str)
{
	size_t size = str.length();
	int wLen = ::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t* buffer = new wchar_t[wLen + 1];
	memset(buffer, 0, (wLen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), size, (LPWSTR)buffer, wLen);
	return buffer;
}

void render::dx_renderer::draw_text(std::string_view text, int x, int y, unsigned long color)
{
	RECT r = { x,y,x,y };
	m_font->DrawTextW(NULL, string2LPCWSTR(text.data()), -1, &r, DT_NOCLIP, color);
}
