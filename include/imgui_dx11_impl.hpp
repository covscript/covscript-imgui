#pragma once
/*
* Covariant Script ImGUI Extension Windows DirectX Implement
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2017-2024 Michael Lee(李登淳)
*
* Email:   mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: https://covscript.org.cn
*/
#include <imgui_win32.hpp>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <d3d11.h>
#include <WICTextureLoader.h>

// Data
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace imgui_cs {
	class image final {
		int m_width;
		int m_height;
		ID3D11Resource *m_textureID;
	public:
		image()=delete;
		image(const image&)=delete;
		image(image&&) noexcept=delete;
		image(const std::string& path)
		{
			if (!stbi_info(path.c_str(), &m_width, &m_height, nullptr))
				throw cs::lang_error("Read image info error!");
			WCHAR strFileA[MAX_PATH];
			MultiByteToWideChar(CP_ACP, MB_COMPOSITE, path.c_str(), -1, strFileA, MAX_PATH);
			if (CreateWICTextureFromFile(g_pd3dDevice, NULL, strFileA, &m_textureID, NULL) != S_OK)
				throw cs::lang_error("Open image error!");
		}
		~image()
		{
			m_textureID->Release();
		}
		int get_width() const
		{
			return m_width;
		}
		int get_height() const
		{
			return m_height;
		}
		ImTextureID get_texture_id() const
		{
			return reinterpret_cast<ImTextureID>(m_textureID);
		}
	};

	class application final {
		// Application Parameters
		ImVec4 bg_color = {1.0f, 1.0f, 1.0f, 1.0f};
		WNDCLASSEX wc;
		HWND hwnd;

		void init()
		{
			// Initialize Direct3D
			if (!CreateDeviceD3D(hwnd)) {
				CleanupDeviceD3D();
				::UnregisterClass(wc.lpszClassName, wc.hInstance);
				throw cs::lang_error("Failed to initialize DX9 loader.");
			}

			// Show the window
			::ShowWindow(hwnd, SW_SHOWDEFAULT);
			::UpdateWindow(hwnd);

			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			// Setup Platform/Renderer backends
			ImGui_ImplWin32_Init(hwnd);
    		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
			ImFontConfig font_cfg = ImFontConfig();
			ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "DefaultFont, 14px");
			ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(
			                                 get_default_font_data(), 14, &font_cfg);
		}

	public:
		application() = delete;

		application(const application &) = delete;

		application(application &&) noexcept = delete;

		application(std::size_t monitor_id, const std::string &title)
		{
			int count = GetSystemMetrics(SM_CMONITORS);
			if (monitor_id >= count)
				throw cs::lang_error("Monitor does not exist.");
			RECT size = GetScreenRect(monitor_id);
			wc = {sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T(title.c_str()), NULL};
			::RegisterClassEx(&wc);
			hwnd = ::CreateWindow(wc.lpszClassName, _T(title.c_str()), WS_BORDER, size.left, size.top, size.right, size.bottom, NULL, NULL, wc.hInstance, NULL);
			init();
		}

		application(std::size_t width, std::size_t height, const std::string &title)
		{
			wc = {sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T(title.c_str()), NULL};
			::RegisterClassEx(&wc);
			hwnd = ::CreateWindow(wc.lpszClassName, _T(title.c_str()), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, wc.hInstance, NULL);
			init();
		}

		~application()
		{
			// Cleanup
    		ImGui_ImplDX11_Shutdown();
    		ImGui_ImplWin32_Shutdown();
    		ImGui::DestroyContext();
			CleanupDeviceD3D();
			::DestroyWindow(hwnd);
			::UnregisterClass(wc.lpszClassName, wc.hInstance);
		}

		int get_window_width()
		{
			RECT size;
			DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &size, sizeof(RECT));
			return size.right - size.left;
		}

		int get_window_height()
		{
			RECT size;
			DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &size, sizeof(RECT));
			return size.bottom - size.top;
		}

		void set_window_size(int width, int height)
		{
			SetWindowPos(hwnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
		}

		void set_window_title(const std::string &str)
		{
			SetWindowText(hwnd, str.c_str());
		}

		void set_bg_color(const ImVec4 &color)
		{
			bg_color = color;
		}

		bool is_closed()
		{
			bool done = false;
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					done = true;
			}
			return done;
		}

		void prepare()
		{
			// Start the Dear ImGui frame
			ImGui_ImplDX11_NewFrame();
        	ImGui_ImplWin32_NewFrame();
        	ImGui::NewFrame();
		}

		void render()
		{
			// Rendering
        	ImGui::Render();
        	const float clear_color_with_alpha[4] = { bg_color.x * bg_color.w, bg_color.y * bg_color.w, bg_color.z * bg_color.w, bg_color.w };
        	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        	g_pSwapChain->Present(1, 0); // Present with vsync
        	//g_pSwapChain->Present(0, 0); // Present without vsync
		}
	};
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
