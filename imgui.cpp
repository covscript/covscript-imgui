/*
* Covariant Script ImGUI Extension
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/

// Covariant Script Header
#include <covscript/extension.hpp>
#include <covscript/cni.hpp>

// ImGUI Common Header
#include <imgui.h>

// ImGUI GLFW3 Implement
#include <imgui_impl_glfw_gl2.h>
#include <GLFW/glfw3.h>

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
			glfwMakeContextCurrent(window);
			glfwSwapInterval(1);
			ImGui::CreateContext();
			ImGui_ImplGlfwGL2_Init(window, true);
			ImGui::GetIO().NavFlags |= ImGuiNavFlags_EnableKeyboard;
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
			ImGui_ImplGlfwGL2_Shutdown();
			ImGui::DestroyContext();
			glfwDestroyWindow(window);
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
			ImGui_ImplGlfwGL2_NewFrame();
		}

		void render()
		{
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui::Render();
			ImGui_ImplGlfwGL2_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(window);
		}
	};
}

// GLFW Instance
static imgui_cs::glfw_instance glfw_instance;
// CNI Wrapper
static cs::extension imgui_ext;
static cs::extension imgui_app_ext;
static cs::extension_t imgui_app_ext_shared = cs::make_shared_extension(imgui_app_ext);

namespace imgui_cs_ext {
	using namespace cs;
	using application_t = std::shared_ptr<imgui_cs::application>;

// GLFW Functions
	number get_monitor_count()
	{
		int count = 0;
		glfwGetMonitors(&count);
		return count;
	}

	number get_monitor_width(number monitor_id)
	{
		int count = 0;
		GLFWmonitor **monitors = glfwGetMonitors(&count);
		if (monitor_id >= count)
			throw cs::lang_error("Monitor does not exist.");
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitors[static_cast<std::size_t>(monitor_id)]);
		return vidmode->width;
	}

	number get_monitor_height(number monitor_id)
	{
		int count = 0;
		GLFWmonitor **monitors = glfwGetMonitors(&count);
		if (monitor_id >= count)
			throw cs::lang_error("Monitor does not exist.");
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitors[static_cast<std::size_t>(monitor_id)]);
		return vidmode->height;
	}

// ImGui Application
	application_t fullscreen_application(number monitor_id, const string &title)
	{
		return std::make_shared<imgui_cs::application>(monitor_id, title);
	}

	application_t window_application(number width, number height, const string &title)
	{
		return std::make_shared<imgui_cs::application>(width, height, title);
	}

	number get_window_width(application_t &app)
	{
		return app->get_window_width();
	}

	number get_window_height(application_t &app)
	{
		return app->get_window_height();
	}

	void set_window_size(application_t &app, number width, number height)
	{
		app->set_window_size(width, height);
	}

	void set_window_title(application_t &app, const string &str)
	{
		app->set_window_title(str);
	}

	void set_bg_color(application_t &app, const ImVec4 &color)
	{
		app->set_bg_color(color);
	}

	bool is_closed(application_t &app)
	{
		return app->is_closed();
	}

	void prepare(application_t &app)
	{
		app->prepare();
	}

	void render(application_t &app)
	{
		app->render();
	}

// ImGui Functions
	ImVec2 vec2(number a, number b)
	{
		return ImVec2(a, b);
	}

	ImVec4 vec4(number a, number b, number c, number d)
	{
		return ImVec4(a, b, c, d);
	}

	void add_font(const string &str, number size)
	{
		ImGui::GetIO().Fonts->AddFontFromFileTTF(str.c_str(), size);
	}

	void style_color_classic()
	{
		ImGui::StyleColorsClassic();
	}

	void style_color_light()
	{
		ImGui::StyleColorsLight();
	}

	void style_color_dark()
	{
		ImGui::StyleColorsDark();
	}

	number get_framerate()
	{
		return ImGui::GetIO().Framerate;
	}

	void set_next_window_pos(const ImVec2 &pos)
	{
		ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);
	}

	void show_demo_window(bool &open)
	{
		ImGui::ShowDemoWindow(&open);
	}

	void begin(const string &str, bool &open)
	{
		ImGui::Begin(str.c_str(), &open);
	}

	void term()
	{
		ImGui::End();
	}

	void text(const string &str)
	{
		ImGui::Text("%s", str.c_str());
	}

	void slider_float(const string &str, number &n, number min, number max)
	{
		float f = static_cast<float>(n);
		ImGui::SliderFloat(str.c_str(), &f, min, max);
		n = f;
	}

	void color_edit3(const string &str, ImVec4 &color)
	{
		ImGui::ColorEdit3(str.c_str(), reinterpret_cast<float *>(&color));
	}

	void color_edit4(const string &str, ImVec4 &color)
	{
		ImGui::ColorEdit4(str.c_str(), reinterpret_cast<float *>(&color));
	}

	void check_box(const string &str, bool &val)
	{
		ImGui::Checkbox(str.c_str(), &val);
	}

	bool button(const string &str)
	{
		return ImGui::Button(str.c_str());
	}

	void same_line()
	{
		ImGui::SameLine();
	}

	void init()
	{
		// Namespaces
		imgui_ext.add_var("applicaion", var::make_protect<extension_t>(imgui_app_ext_shared));
		// Application
		imgui_app_ext.add_var("get_window_width", var::make_protect<callable>(cni(get_window_width)));
		imgui_app_ext.add_var("get_window_height", var::make_protect<callable>(cni(get_window_height)));
		imgui_app_ext.add_var("set_window_size", var::make_protect<callable>(cni(set_window_size)));
		imgui_app_ext.add_var("set_window_title", var::make_protect<callable>(cni(set_window_title)));
		imgui_app_ext.add_var("set_bg_color", var::make_protect<callable>(cni(set_bg_color)));
		imgui_app_ext.add_var("is_closed", var::make_protect<callable>(cni(is_closed)));
		imgui_app_ext.add_var("prepare", var::make_protect<callable>(cni(prepare)));
		imgui_app_ext.add_var("render", var::make_protect<callable>(cni(render)));
		// Main Function
		imgui_ext.add_var("get_monitor_count", var::make_protect<callable>(cni(get_monitor_count)));
		imgui_ext.add_var("get_monitor_width", var::make_protect<callable>(cni(get_monitor_width)));
		imgui_ext.add_var("get_monitor_height", var::make_protect<callable>(cni(get_monitor_height)));
		imgui_ext.add_var("fullscreen_application", var::make_protect<callable>(cni(fullscreen_application)));
		imgui_ext.add_var("window_application", var::make_protect<callable>(cni(window_application)));
		imgui_ext.add_var("vec2", var::make_protect<callable>(cni(vec2), true));
		imgui_ext.add_var("vec4", var::make_protect<callable>(cni(vec4), true));
		imgui_ext.add_var("add_font", var::make_protect<callable>(cni(add_font)));
		imgui_ext.add_var("style_color_classic", var::make_protect<callable>(cni(style_color_classic)));
		imgui_ext.add_var("style_color_light", var::make_protect<callable>(cni(style_color_light)));
		imgui_ext.add_var("style_color_dark", var::make_protect<callable>(cni(style_color_dark)));
		imgui_ext.add_var("get_framerate", var::make_protect<callable>(cni(get_framerate)));
		imgui_ext.add_var("set_next_window_pos", var::make_protect<callable>(cni(set_next_window_pos)));
		imgui_ext.add_var("show_demo_window", var::make_protect<callable>(cni(show_demo_window)));
		imgui_ext.add_var("begin", var::make_protect<callable>(cni(begin)));
		imgui_ext.add_var("term", var::make_protect<callable>(cni(term)));
		imgui_ext.add_var("text", var::make_protect<callable>(cni(text)));
		imgui_ext.add_var("slider_float", var::make_protect<callable>(cni(slider_float)));
		imgui_ext.add_var("color_edit3", var::make_protect<callable>(cni(color_edit3)));
		imgui_ext.add_var("color_edit4", var::make_protect<callable>(cni(color_edit4)));
		imgui_ext.add_var("check_box", var::make_protect<callable>(cni(check_box)));
		imgui_ext.add_var("button", var::make_protect<callable>(cni(button)));
		imgui_ext.add_var("same_line", var::make_protect<callable>(cni(same_line)));
	}
}

namespace cs_impl {
	template<>
	cs::extension_t &get_ext<imgui_cs_ext::application_t>()
	{
		return imgui_app_ext_shared;
	}

	template<>
	constexpr const char *get_name_of_type<imgui_cs_ext::application_t>()
	{
		return "cs::imgui::application";
	}
}

cs::extension *cs_extension()
{
	imgui_cs_ext::init();
	return &imgui_ext;
}