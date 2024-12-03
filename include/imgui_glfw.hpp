#pragma once
/*
* Covariant Script ImGUI Extension GLFW Header
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

// GL3W/GLFW3
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace imgui_cs {
	class image final {
		int m_width;
		int m_height;
		GLuint m_textureID;
		unsigned char *m_data;
	public:
		image()=delete;
		image(const image&)=delete;
		image(image&&) noexcept=delete;
		image(const std::string& path)
		{
			m_data = stbi_load(path.c_str(), &m_width, &m_height, nullptr, 4);
			if (m_data == nullptr)
				throw cs::lang_error("Open image error!");
			glGenTextures(1, &m_textureID);
			glBindTexture(GL_TEXTURE_2D, m_textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data);
			stbi_image_free(m_data);
		}
		~image() {}
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
			return static_cast<ImTextureID>(m_textureID);
		}
	};

	int get_monitor_count()
	{
		int count = 0;
		glfwGetMonitors(&count);
		return count;
	}

	int get_monitor_width(int monitor_id)
	{
		int count = 0;
		GLFWmonitor **monitors = glfwGetMonitors(&count);
		if (monitor_id >= count)
			throw cs::lang_error("Monitor does not exist.");
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitors[static_cast<std::size_t>(monitor_id)]);
		return vidmode->width;
	}

	int get_monitor_height(int monitor_id)
	{
		int count = 0;
		GLFWmonitor **monitors = glfwGetMonitors(&count);
		if (monitor_id >= count)
			throw cs::lang_error("Monitor does not exist.");
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitors[static_cast<std::size_t>(monitor_id)]);
		return vidmode->height;
	}
}