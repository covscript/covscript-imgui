#pragma once
/*
* Covariant Script ImGUI Extension SDL2 Header
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

#include <imgui.hpp>

// STB Image
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// SDL2
#include <SDL.h>

namespace imgui_cs {
	// Global renderer pointer — set by application on init, used by image for texture creation.
	// SDL_Texture is bound to a specific SDL_Renderer, so we must use the application's renderer.
	extern SDL_Renderer *g_SDLRenderer;

	// Lazy SDL initialization — avoids calling SDL_Init during DLL loading
	// (which would deadlock on Windows due to the loader lock).
	inline void ensure_sdl_init()
	{
		if (!SDL_WasInit(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER)) {
			SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
			if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
				throw cs::lang_error("Init SDL2 Error.");
		}
	}

	// Called by ~application(). SDL_Quit only when all active windows are closed.
	inline void ensure_sdl_quit()
	{
		SDL_Quit();
	}

	class image final {
		int m_width = 0;
		int m_height = 0;
		mutable unsigned char *m_pixels = nullptr;
		mutable SDL_Texture *m_textureID = nullptr;

		void ensure_texture() const
		{
			if (m_textureID != nullptr || m_pixels == nullptr)
				return;
			// Guard against use after renderer destruction
			if (g_SDLRenderer == nullptr)
				return;
			SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(
			                           m_pixels, m_width, m_height, 32, m_width * 4, SDL_PIXELFORMAT_RGBA32);
			if (surface == nullptr)
				return;
			m_textureID = SDL_CreateTextureFromSurface(g_SDLRenderer, surface);
			SDL_FreeSurface(surface);
			if (m_textureID == nullptr)
				return; // Preserve pixel data for retry on failure
			SDL_SetTextureBlendMode(m_textureID, SDL_BLENDMODE_BLEND);
			SDL_SetTextureScaleMode(m_textureID, SDL_ScaleModeLinear);
			// Free pixel data once texture is created — no longer needed
			stbi_image_free(m_pixels);
			m_pixels = nullptr;
		}

	public:
		image() = delete;
		image(const image &) = delete;
		image(image &&) noexcept = delete;
		image(const std::string &path)
		{
			int channels;
			m_pixels = stbi_load(path.c_str(), &m_width, &m_height, &channels, 4);
			if (m_pixels == nullptr)
				throw cs::lang_error("Open image error!");
		}
		~image()
		{
			// Only destroy texture if the renderer that owns it still exists.
			// SDL_DestroyRenderer frees all associated textures; attempting to
			// destroy one afterward is a double-free.
			if (m_textureID && g_SDLRenderer)
				SDL_DestroyTexture(m_textureID);
			if (m_pixels)
				stbi_image_free(m_pixels);
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
			ensure_texture();
			return (ImTextureID)(intptr_t)m_textureID;
		}
	};

	int get_monitor_count()
	{
		ensure_sdl_init();
		return SDL_GetNumVideoDisplays();
	}

	int get_monitor_width(int monitor_id)
	{
		ensure_sdl_init();
		SDL_Rect bounds;
		if (SDL_GetDisplayBounds(monitor_id, &bounds) != 0)
			throw cs::lang_error("Monitor does not exist.");
		return bounds.w;
	}

	int get_monitor_height(int monitor_id)
	{
		ensure_sdl_init();
		SDL_Rect bounds;
		if (SDL_GetDisplayBounds(monitor_id, &bounds) != 0)
			throw cs::lang_error("Monitor does not exist.");
		return bounds.h;
	}
}
