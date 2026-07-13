#pragma once
/*
* Covariant Script ImGUI Extension SDL2 Implement
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
#include <imgui_sdl.hpp>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

namespace imgui_cs {
	class application final {
		SDL_Window *window = nullptr;
		SDL_Renderer *renderer = nullptr;
		ImVec4 bg_color = {0.25f, 0.25f, 0.25f, 1.0f};
		bool m_closed = false;

		void init()
		{
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			// Setup Platform/Renderer backends
			if (!ImGui_ImplSDL2_InitForSDLRenderer(window, renderer)) {
				ImGui::DestroyContext();
				throw cs::lang_error("Failed to init SDL2 platform backend!");
			}
			if (!ImGui_ImplSDLRenderer2_Init(renderer)) {
				ImGui_ImplSDL2_Shutdown();
				ImGui::DestroyContext();
				throw cs::lang_error("Failed to init SDL2 renderer backend!");
			}
			g_SDLRenderer = renderer;

			// Load default font
			ImFontConfig font_cfg = ImFontConfig();
			ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "DefaultFont, 14px");
			ImFont *font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(
			                   get_default_font_data(), 14, &font_cfg);
			if (font == nullptr) {
				g_SDLRenderer = nullptr;
				ImGui_ImplSDLRenderer2_Shutdown();
				ImGui_ImplSDL2_Shutdown();
				ImGui::DestroyContext();
				throw cs::lang_error("Failed to load default font!");
			}
			ImGui::GetIO().FontDefault = font;
		}

	public:
		application() = delete;

		application(const application &) = delete;

		application(application &&) noexcept = delete;

		application(std::size_t monitor_id, const std::string &title)
		{
			ensure_sdl_init();
			int count = SDL_GetNumVideoDisplays();
			int mid = static_cast<int>(monitor_id);
			if (mid < 0 || mid >= count)
				throw cs::lang_error("Monitor does not exist.");
			SDL_Rect bounds;
			if (SDL_GetDisplayBounds(mid, &bounds) != 0)
				throw cs::lang_error("Monitor does not exist.");
			window = SDL_CreateWindow(title.c_str(),
			                          bounds.x, bounds.y, bounds.w, bounds.h,
			                          SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
			if (window == nullptr)
				throw cs::lang_error("Create SDL window error!");
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
			if (renderer == nullptr) {
				SDL_DestroyWindow(window);
				window = nullptr;
				throw cs::lang_error("Create SDL renderer error!");
			}
			try {
				init();
			}
			catch (...) {
				SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				throw;
			}
		}

		application(std::size_t width, std::size_t height, const std::string &title)
		{
			ensure_sdl_init();
			int w = static_cast<int>(width);
			int h = static_cast<int>(height);
			if (w < 0 || h < 0)
				throw cs::lang_error("Invalid window size.");
			window = SDL_CreateWindow(title.c_str(),
			                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			                          w, h,
			                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
			if (window == nullptr)
				throw cs::lang_error("Create SDL window error!");
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
			if (renderer == nullptr) {
				SDL_DestroyWindow(window);
				window = nullptr;
				throw cs::lang_error("Create SDL renderer error!");
			}
			try {
				init();
			}
			catch (...) {
				SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				throw;
			}
		}

		~application()
		{
			ImGui_ImplSDLRenderer2_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			ImGui::DestroyContext();
			g_SDLRenderer = nullptr; // Clear before destroying renderer so image destructors skip texture teardown
			if (renderer)
				SDL_DestroyRenderer(renderer);
			if (window)
				SDL_DestroyWindow(window);
			ensure_sdl_quit();
		}

		int get_window_width()
		{
			int w = 0;
			SDL_GetWindowSize(window, &w, nullptr);
			return w;
		}

		int get_window_height()
		{
			int h = 0;
			SDL_GetWindowSize(window, nullptr, &h);
			return h;
		}

		void set_window_size(int width, int height)
		{
			SDL_SetWindowSize(window, width, height);
		}

		void set_window_title(const std::string &str)
		{
			SDL_SetWindowTitle(window, str.c_str());
		}

		void set_bg_color(const ImVec4 &color)
		{
			bg_color = color;
		}

		bool is_closed() const
		{
			return m_closed;
		}

		void prepare()
		{
			// Pump SDL events to keep the window responsive
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				ImGui_ImplSDL2_ProcessEvent(&event);
				if (event.type == SDL_QUIT)
					m_closed = true;
				if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
					m_closed = true;
			}
			// Platform backend first, then renderer — matches official ImGui SDL2+SDLRenderer2 examples
			ImGui_ImplSDL2_NewFrame();
			ImGui_ImplSDLRenderer2_NewFrame();
			ImGui::NewFrame();
		}

		void render()
		{
			ImGui::Render();
			// On HiDPI displays (e.g. macOS Retina), the framebuffer is larger
			// than the logical window size. Set SDL_RenderSetScale so that
			// ImGui's logical-coordinate vertices fill the entire framebuffer.
			const ImVec2 &fb_scale = ImGui::GetIO().DisplayFramebufferScale;
			SDL_RenderSetScale(renderer, fb_scale.x, fb_scale.y);
			SDL_SetRenderDrawColor(renderer,
			                       static_cast<Uint8>(bg_color.x * 255),
			                       static_cast<Uint8>(bg_color.y * 255),
			                       static_cast<Uint8>(bg_color.z * 255),
			                       static_cast<Uint8>(bg_color.w * 255));
			SDL_RenderClear(renderer);
			ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
			SDL_RenderPresent(renderer);
		}
	};
}

// Global renderer for image texture creation
SDL_Renderer *imgui_cs::g_SDLRenderer = nullptr;
