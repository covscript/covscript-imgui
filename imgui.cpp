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

// ImGUI Wrapper
#include <cstdio>
namespace imgui_cs {
	class application final {
		static void error_callback(int error, const char *description)
		{
			throw cs::lang_error(description);
		}
		GLFWwindow *window = nullptr;
		ImVec4 bg_color = {1.0f, 1.0f, 1.0f, 1.0f};

	public:
		application() = delete;
		application(const application &) = delete;
		application(application &&) noexcept = delete;
		application(std::size_t width, std::size_t height, const std::string &title)
		{
			glfwSetErrorCallback(error_callback);
			if (!glfwInit())
				throw cs::lang_error("Init OpenGL Error.");
			window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
			glfwMakeContextCurrent(window);
			glfwSwapInterval(true);
			ImGui::CreateContext();
			ImGui_ImplGlfwGL2_Init(window, true);
			ImGui::GetIO().NavFlags |= ImGuiNavFlags_EnableKeyboard;
		}
		~application()
		{
			ImGui_ImplGlfwGL2_Shutdown();
			ImGui::DestroyContext();
			glfwTerminate();
		}
		void set_bg_color(const ImVec4 &color)
		{
			bg_color = color;
		}
		bool window_should_close()
		{
			return !glfwWindowShouldClose(window);
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

// CNI Wrapper
static cs::extension imgui_ext;
static cs::extension imgui_app_ext;
static cs::extension_t imgui_app_ext_shared = cs::make_shared_extension(imgui_app_ext);

namespace cs_impl {
	template <>
	cs::extension_t &get_ext<imgui_cs::application>()
	{
		return imgui_app_ext_shared;
	}

	template <>
	constexpr const char *get_name_of_type<imgui_cs::application>()
	{
		return "cs::imgui::application";
	}
}

namespace imgui_cs_ext {
	using namespace cs;
	using application_t = std::shared_ptr<imgui_cs::application>;
// ImGui Application
	application_t app(number width, number height, const string &title)
	{
		return std::make_shared<imgui_cs::application>(width, height, title);
	}

	void set_bg_color(application_t &app, const ImVec4 &color)
	{
		app->set_bg_color(color);
	}

	bool window_should_close(application_t &app)
	{
		return app->window_should_close();
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
		ImGui::Text(str.c_str());
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
		imgui_app_ext.add_var("set_bg_color", var::make_protect<callable>(cni(set_bg_color)));
		imgui_app_ext.add_var("window_should_close", var::make_protect<callable>(cni(window_should_close)));
		imgui_app_ext.add_var("prepare", var::make_protect<callable>(cni(prepare)));
		imgui_app_ext.add_var("render", var::make_protect<callable>(cni(render)));
		// Main Function
		imgui_ext.add_var("app", var::make_protect<callable>(cni(app)));
		imgui_ext.add_var("vec2", var::make_protect<callable>(cni(vec2)));
		imgui_ext.add_var("vec4", var::make_protect<callable>(cni(vec4)));
		imgui_ext.add_var("add_font", var::make_protect<callable>(cni(add_font)));
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

cs::extension *cs_extension()
{
	imgui_cs_ext::init();
	return &imgui_ext;
}