#pragma once
/*
* Covariant Script ImGUI Extension OpenGL 3 Implement
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
#include <imgui_glfw.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace imgui_cs {
	class glfw_instance final {
		static void error_callback(int error, const char *description)
		{
			throw cs::lang_error(description);
		}

	public:
		glfw_instance()
		{
			glfwSetErrorCallback(error_callback);
			if (!glfwInit())
				throw cs::lang_error("Init OpenGL Error.");
#if __APPLE__
			// GL 3.2
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
			// GL 3.0
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
		}

		glfw_instance(const glfw_instance &) = delete;

		glfw_instance(glfw_instance &&) noexcept = delete;

		~glfw_instance()
		{
			glfwTerminate();
		}
	};

	class application final {
		GLFWwindow *window = nullptr;
		ImVec4 bg_color = {1.0f, 1.0f, 1.0f, 1.0f};

		void init()
		{
#if __APPLE__
			// GLSL 150
			const char* glsl_version = "#version 150";
#else
			// GLSL 130
			const char *glsl_version = "#version 130";
#endif
			glfwMakeContextCurrent(window);
			glfwSwapInterval(1);
			if (gl3wInit() != 0)
				throw cs::lang_error("Failed to initialize OpenGL loader.");
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init(glsl_version);
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
			int count = 0;
			GLFWmonitor **monitors = glfwGetMonitors(&count);
			if (monitor_id >= count)
				throw cs::lang_error("Monitor does not exist.");
			const GLFWvidmode *vidmode = glfwGetVideoMode(monitors[monitor_id]);
			window = glfwCreateWindow(vidmode->width, vidmode->height, title.c_str(), monitors[monitor_id], NULL);
			init();
		}

		application(std::size_t width, std::size_t height, const std::string &title) : window(
			    glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr))
		{
			init();
		}

		~application()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
			glfwDestroyWindow(window);
			glfwTerminate();
		}

		int get_window_width()
		{
			int width = 0;
			glfwGetWindowSize(window, &width, nullptr);
			return width;
		}

		int get_window_height()
		{
			int height = 0;
			glfwGetWindowSize(window, nullptr, &height);
			return height;
		}

		void set_window_size(int width, int height)
		{
			glfwSetWindowSize(window, width, height);
		}

		void set_window_title(const std::string &str)
		{
			glfwSetWindowTitle(window, str.c_str());
		}

		void set_bg_color(const ImVec4 &color)
		{
			bg_color = color;
		}

		bool is_closed()
		{
			return glfwWindowShouldClose(window);
		}

		void prepare()
		{
			glfwPollEvents();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		void render()
		{
			ImGui::Render();
			int display_w, display_h;
			glfwMakeContextCurrent(window);
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwMakeContextCurrent(window);
			glfwSwapBuffers(window);
		}
	};
}

// GLFW Instance
static imgui_cs::glfw_instance glfw_instance;