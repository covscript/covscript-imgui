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

// ImGUI GL3W/GLFW3 Implement
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw_gl3.h>

// Other Headers
#include <cstring>
#include <cstdio>

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
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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
			glfwMakeContextCurrent(window);
			glfwSwapInterval(1);
			gl3wInit();
			ImGui::CreateContext();
			ImGui_ImplGlfwGL3_Init(window, true);
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
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
			ImGui_ImplGlfwGL3_Shutdown();
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
			ImGui_ImplGlfwGL3_NewFrame();
		}

		void render()
		{
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(window);
		}
	};

	template<typename char_t = char>
	class buffer final {
		char_t *buff = nullptr;

	public:
		buffer() = delete;

		buffer(const buffer &) = delete;

		buffer(buffer &b) noexcept
		{
			std::swap(this->buff, b.buff);
		}

		buffer(std::size_t size) : buff(new char_t[size]) {}

		~buffer()
		{
			delete[] buff;
		}

		char_t *get() const
		{
			return buff;
		}
	};

	class image final {
		int width;
		int height;
		GLuint textureID;
		unsigned char *data;
	public:
		image() = delete;

		image(const image &) = delete;

		image(image &&) noexcept = delete;

		explicit image(const std::string &path)
		{
			unsigned char header[54];
			unsigned int image_size;
			FILE *file = fopen(path.c_str(), "rb");
			if (!file)
				throw cs::lang_error("Image could not be opened");
			if (fread(header, 1, 54, file) != 54) {
				fclose(file);
				throw cs::lang_error("Not a correct BMP file");
			}
			if (header[0] != 'B' || header[1] != 'M') {
				fclose(file);
				throw cs::lang_error("Not a correct BMP file");
			}
			if (*(int *) &(header[0x1C]) != 24) {
				fclose(file);
				throw cs::lang_error("Not a 24-bit BMP file");
			}
			width = *(int *) &(header[0x12]);
			height = *(int *) &(header[0x16]);
			image_size = *(int *) &(header[0x22]);
			if (image_size == 0)
				image_size = width * height * 3;
			data = new unsigned char[image_size];
			fread(data, 1, image_size, file);
			fclose(file);
			unsigned char *reversed_data = new unsigned char[image_size];
			int row_size = width * 3;
			if (row_size % 4 != 0)
				row_size += 4 - row_size % 4;
			for (int y = 0; y < height; ++y)
				for (int x = 0; x < row_size; ++x)
					reversed_data[y * row_size + x] = data[(height - y - 1) * row_size + x];
			std::swap(data, reversed_data);
			delete[] reversed_data;
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		~image()
		{
			delete[] data;
		}

		int get_width() const
		{
			return width;
		}

		int get_height() const
		{
			return height;
		}

		ImTextureID get_texture_id() const
		{
			return reinterpret_cast<ImTextureID>(textureID);
		}
	};
}

// GLFW Instance
static imgui_cs::glfw_instance glfw_instance;
// CNI Wrapper
static cs::extension imgui_ext;
static cs::extension imgui_app_ext;
static cs::extension imgui_img_ext;
static cs::extension imgui_keys_ext;
static cs::extension_t imgui_app_ext_shared = cs::make_shared_extension(imgui_app_ext);
static cs::extension_t imgui_img_ext_shared = cs::make_shared_extension(imgui_img_ext);
static cs::extension_t imgui_keys_ext_shared = cs::make_shared_extension(imgui_keys_ext);

class cni_register final {
public:
	template<typename _fT>
	cni_register(const char *name, _fT &&func, bool is_const)
	{
		using namespace cs;
		imgui_ext.add_var(name, var::make_protect<callable>(cni(func), is_const));
	}
};

#define CNI_NAME_MIXER(PREFIX, NAME) static cni_register PREFIX##NAME
#define CNI_REGISTER(NAME, ARGS) CNI_NAME_MIXER(_cni_register_, NAME) \
\
(#NAME,                                                               \
 NAME, ARGS);
#define CNI_NORMAL(name) CNI_REGISTER(name, false)
#define CNI_CONST(name) CNI_REGISTER(name, true)

namespace imgui_cs_ext {
	using namespace cs;
	using application_t = std::shared_ptr<imgui_cs::application>;
	using image_t=std::shared_ptr<imgui_cs::image>;

// GLFW Functions
	number get_monitor_count()
	{
		int count = 0;
		glfwGetMonitors(&count);
		return count;
	}

	CNI_NORMAL(get_monitor_count)

	number get_monitor_width(number monitor_id)
	{
		int count = 0;
		GLFWmonitor **monitors = glfwGetMonitors(&count);
		if (monitor_id >= count)
			throw cs::lang_error("Monitor does not exist.");
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitors[static_cast<std::size_t>(monitor_id)]);
		return vidmode->width;
	}

	CNI_NORMAL(get_monitor_width)

	number get_monitor_height(number monitor_id)
	{
		int count = 0;
		GLFWmonitor **monitors = glfwGetMonitors(&count);
		if (monitor_id >= count)
			throw cs::lang_error("Monitor does not exist.");
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitors[static_cast<std::size_t>(monitor_id)]);
		return vidmode->height;
	}

	CNI_NORMAL(get_monitor_height)

// ImGui Application
	application_t fullscreen_application(number monitor_id, const string &title)
	{
		return std::make_shared<imgui_cs::application>(monitor_id, title);
	}

	CNI_NORMAL(fullscreen_application)

	application_t window_application(number width, number height, const string &title)
	{
		return std::make_shared<imgui_cs::application>(width, height, title);
	}

	CNI_NORMAL(window_application)

	namespace applicaion_cs_ext {
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
	}

// ImGui Image
	image_t load_bmp_image(const string &path)
	{
		return std::make_shared<imgui_cs::image>(path);
	}

	CNI_NORMAL(load_bmp_image)

	namespace image_cs_ext {
		number get_width(const image_t &image)
		{
			return image->get_width();
		}

		number get_height(const image_t &image)
		{
			return image->get_height();
		}
	}

// ImGui Functions
	ImVec2 vec2(number a, number b)
	{
		return ImVec2(a, b);
	}

	CNI_CONST(vec2)

	ImVec4 vec4(number a, number b, number c, number d)
	{
		return ImVec4(a, b, c, d);
	}

	CNI_CONST(vec4)

	number get_framerate()
	{
		return ImGui::GetIO().Framerate;
	}

	CNI_NORMAL(get_framerate)

// Styles
	void add_font(const string &str, number size)
	{
		ImGui::GetIO().Fonts->AddFontFromFileTTF(str.c_str(), size);
	}

	CNI_NORMAL(add_font)

	void style_color_classic()
	{
		ImGui::StyleColorsClassic();
	}

	CNI_NORMAL(style_color_classic)

	void style_color_light()
	{
		ImGui::StyleColorsLight();
	}

	CNI_NORMAL(style_color_light)

	void style_color_dark()
	{
		ImGui::StyleColorsDark();
	}

	CNI_NORMAL(style_color_dark)

// Windows
	void set_next_window_pos(const ImVec2 &pos)
	{
		ImGui::SetNextWindowPos(pos);
	}

	CNI_NORMAL(set_next_window_pos)

	void set_window_pos(const ImVec2 &pos)
	{
		ImGui::SetWindowPos(pos);
	}

	CNI_NORMAL(set_window_pos)

	number get_window_pos_x()
	{
		return ImGui::GetWindowPos().x;
	}

	CNI_NORMAL(get_window_pos_x)

	number get_window_pos_y()
	{
		return ImGui::GetWindowPos().y;
	}

	CNI_NORMAL(get_window_pos_y)

	void set_next_window_size(const ImVec2 &size)
	{
		ImGui::SetNextWindowSize(size);
	}

	CNI_NORMAL(set_next_window_size)

	void set_window_size(const ImVec2 &size)
	{
		ImGui::SetWindowSize(size);
	}

	CNI_NORMAL(set_window_size)

	number get_window_width()
	{
		return ImGui::GetWindowWidth();
	}

	CNI_NORMAL(get_window_width)

	number get_window_height()
	{
		return ImGui::GetWindowHeight();
	}

	CNI_NORMAL(get_window_height)

	void show_demo_window(bool &open)
	{
		ImGui::ShowDemoWindow(&open);
	}

	CNI_NORMAL(show_demo_window)

	void begin_window(const string &str, bool &open)
	{
		ImGui::Begin(str.c_str(), &open);
	}

	CNI_NORMAL(begin_window)

	void end_window()
	{
		ImGui::End();
	}

	CNI_NORMAL(end_window)

	void begin_child(const string &str)
	{
		ImGui::BeginChild(str.c_str());
	}

	CNI_NORMAL(begin_child)

	void end_child()
	{
		ImGui::EndChild();
	}

	CNI_NORMAL(end_child)

// Layouts
	void begin_group()
	{
		ImGui::BeginGroup();
	}

	CNI_NORMAL(begin_group)

	void end_group()
	{
		ImGui::EndGroup();
	}

	CNI_NORMAL(end_group)

	void separator()
	{
		ImGui::Separator();
	}

	CNI_NORMAL(separator)

	void same_line()
	{
		ImGui::SameLine();
	}

	CNI_NORMAL(same_line)

	void spacing()
	{
		ImGui::Spacing();
	}

	CNI_NORMAL(spacing)

	void indent()
	{
		ImGui::Indent();
	}

	CNI_NORMAL(indent)

// Widgets
	void text(const string &str)
	{
		ImGui::TextUnformatted(str.c_str());
	}

	CNI_NORMAL(text)

	bool button(const string &str)
	{
		return ImGui::Button(str.c_str());
	}

	CNI_NORMAL(button)

	void image(const image_t &img, const ImVec2 &size)
	{
		ImGui::Image(img->get_texture_id(), size);
	}

	CNI_NORMAL(image)

	bool image_button(const image_t &img, const ImVec2 &size)
	{
		return ImGui::ImageButton(img->get_texture_id(), size);
	}

	CNI_NORMAL(image_button)

	void check_box(const string &str, bool &val)
	{
		ImGui::Checkbox(str.c_str(), &val);
	}

	CNI_NORMAL(check_box)

	void radio_button(const string &str, number &v, number v_button)
	{
		int _v = v;
		ImGui::RadioButton(str.c_str(), &_v, v_button);
		v = _v;
	}

	CNI_NORMAL(radio_button)

	void progress_bar(number fraction, const string &overlay)
	{
		ImGui::ProgressBar(fraction, ImVec2(-1, 0), overlay.c_str());
	}

	CNI_NORMAL(progress_bar)

	void combo_box(const string &str, number &current, const array &items)
	{
		const char **_items = new const char *[items.size()];
		for (std::size_t i = 0; i < items.size(); ++i)
			_items[i] = items[i].const_val<string>().c_str();
		int _current = current;
		ImGui::Combo(str.c_str(), &_current, _items, items.size());
		current = _current;
		delete[] _items;
	}

	CNI_NORMAL(combo_box)

	void slider_float(const string &str, number &n, number min, number max)
	{
		float f = static_cast<float>(n);
		ImGui::SliderFloat(str.c_str(), &f, min, max);
		n = f;
	}

	CNI_NORMAL(slider_float)

	void input_text(const string &str, string &text, number buff_size)
	{
		imgui_cs::buffer<> buff(buff_size);
		std::strcpy(buff.get(), text.c_str());
		ImGui::InputText(str.c_str(), buff.get(), buff_size);
		text = buff.get();
	}

	CNI_NORMAL(input_text)

	void input_text_multiline(const string &str, string &text, number buff_size)
	{
		imgui_cs::buffer<> buff(buff_size);
		std::strcpy(buff.get(), text.c_str());
		ImGui::InputTextMultiline(str.c_str(), buff.get(), buff_size);
		text = buff.get();
	}

	CNI_NORMAL(input_text_multiline)

	void color_edit3(const string &str, ImVec4 &color)
	{
		ImGui::ColorEdit3(str.c_str(), reinterpret_cast<float *>(&color));
	}

	CNI_NORMAL(color_edit3)

	void color_edit4(const string &str, ImVec4 &color)
	{
		ImGui::ColorEdit4(str.c_str(), reinterpret_cast<float *>(&color));
	}

	CNI_NORMAL(color_edit4)

// Trees
	bool tree_node(const string &label)
	{
		ImGui::TreeNode(label.c_str());
	}

	CNI_NORMAL(tree_node)

	void tree_pop()
	{
		ImGui::TreePop();
	}

	CNI_NORMAL(tree_pop)

// Selectable / Lists
	void selectable(const string &str, bool &selected)
	{
		ImGui::Selectable(str.c_str(), &selected);
	}

	CNI_NORMAL(selectable)

	void list_box(const string &str, number &current, const array &items)
	{
		const char **_items = new const char *[items.size()];
		for (std::size_t i = 0; i < items.size(); ++i)
			_items[i] = items[i].const_val<string>().c_str();
		int _current = current;
		ImGui::ListBox(str.c_str(), &_current, _items, items.size());
		delete[] _items;
	}

	CNI_NORMAL(list_box)

// Menus
	bool begin_main_menu_bar()
	{
		return ImGui::BeginMainMenuBar();
	}

	CNI_NORMAL(begin_main_menu_bar)

	void end_main_menu_bar()
	{
		ImGui::EndMainMenuBar();
	}

	CNI_NORMAL(end_main_menu_bar)

	bool begin_menu_bar()
	{
		return ImGui::BeginMenuBar();
	}

	CNI_NORMAL(begin_menu_bar)

	void end_menu_bar()
	{
		ImGui::EndMenuBar();
	}

	CNI_NORMAL(end_menu_bar)

	bool begin_menu(const string &str, bool enabled)
	{
		return ImGui::BeginMenu(str.c_str(), enabled);
	}

	CNI_NORMAL(begin_menu)

	void end_menu()
	{
		ImGui::EndMenu();
	}

	CNI_NORMAL(end_menu)

	bool menu_item(const string &str, const string &shortcut, bool enabled)
	{
		return ImGui::MenuItem(str.c_str(), shortcut.c_str(), false, enabled);
	}

	CNI_NORMAL(menu_item)

// Inputs
	number get_key_index(ImGuiKey key)
	{
		return ImGui::GetKeyIndex(key);
	}

	CNI_NORMAL(get_key_index)

	bool is_key_down(number key)
	{
		return ImGui::IsKeyDown(key);
	}

	CNI_NORMAL(is_key_down)

	bool is_key_pressed(number key)
	{
		return ImGui::IsKeyPressed(key);
	}

	CNI_NORMAL(is_key_pressed)

	bool is_key_released(number key)
	{
		return ImGui::IsKeyReleased(key);
	}

	CNI_NORMAL(is_key_released)

	bool is_mouse_clicked(number button)
	{
		return ImGui::IsMouseClicked(button);
	}

	CNI_NORMAL(is_mouse_clicked)

	bool is_mouse_double_clicked(number button)
	{
		return ImGui::IsMouseDoubleClicked(button);
	}

	CNI_NORMAL(is_mouse_double_clicked)

	bool is_mouse_dragging(number button)
	{
		return ImGui::IsMouseDragging(button);
	}

	CNI_NORMAL(is_mouse_dragging)

	number get_mouse_pos_x()
	{
		return ImGui::GetMousePos().x;
	}

	CNI_NORMAL(get_mouse_pos_x)

	number get_mouse_pos_y()
	{
		return ImGui::GetMousePos().y;
	}

	CNI_NORMAL(get_mouse_pos_y)

	number get_mouse_drag_delta_x()
	{
		return ImGui::GetMouseDragDelta().x;
	}

	CNI_NORMAL(get_mouse_drag_delta_x)

	number get_mouse_drag_delta_y()
	{
		return ImGui::GetMouseDragDelta().y;
	}

	CNI_NORMAL(get_mouse_drag_delta_y)

	string get_clipboard_text()
	{
		return ImGui::GetClipboardText();
	}

	CNI_NORMAL(get_clipboard_text)

	void set_clipboard_text(const string &str)
	{
		ImGui::SetClipboardText(str.c_str());
	}

	CNI_NORMAL(set_clipboard_text)

// Canvas
	void add_line(const ImVec2 &a, const ImVec2 &b, const ImVec4 &color, number thickness)
	{
		ImGui::GetWindowDrawList()->AddLine(a, b, ImColor(color), thickness);
	}

	CNI_NORMAL(add_line)

	void add_rect(const ImVec2 &a, const ImVec2 &b, const ImVec4 &color, number rounding, number thickness)
	{
		ImGui::GetWindowDrawList()->AddRect(a, b, ImColor(color), rounding, ImDrawCornerFlags_All,
		                                    thickness);
	}

	CNI_NORMAL(add_rect)

	void add_rect_filled(const ImVec2 &a, const ImVec2 &b, const ImVec4 &color, number rounding)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(a, b, ImColor(color), rounding, ImDrawCornerFlags_All);
	}

	CNI_NORMAL(add_rect_filled)

	void add_quad(const ImVec2 &a, const ImVec2 &b, const ImVec2 &c, const ImVec2 &d, const ImVec4 &color,
	              number thickness)
	{
		ImGui::GetWindowDrawList()->AddQuad(a, b, c, d, ImColor(color), thickness);
	}

	CNI_NORMAL(add_quad)

	void add_quad_filled(const ImVec2 &a, const ImVec2 &b, const ImVec2 &c, const ImVec2 &d, const ImVec4 &color)
	{
		ImGui::GetWindowDrawList()->AddQuadFilled(a, b, c, d, ImColor(color));
	}

	CNI_NORMAL(add_quad_filled)

	void add_triangle(const ImVec2 &a, const ImVec2 &b, const ImVec2 &c, const ImVec4 &color, number thickness)
	{
		ImGui::GetWindowDrawList()->AddTriangle(a, b, c, ImColor(color), thickness);
	}

	CNI_NORMAL(add_triangle)

	void add_triangle_filled(const ImVec2 &a, const ImVec2 &b, const ImVec2 &c, const ImVec4 &color)
	{
		ImGui::GetWindowDrawList()->AddTriangleFilled(a, b, c, ImColor(color));
	}

	CNI_NORMAL(add_triangle_filled)

	void add_circle(const ImVec2 &centre, number radius, const ImVec4 &color, number seg, number thickness)
	{
		ImGui::GetWindowDrawList()->AddCircle(centre, radius, ImColor(color), seg, thickness);
	}

	CNI_NORMAL(add_circle)

	void add_circle_filled(const ImVec2 &centre, number radius, const ImVec4 &color, number seg)
	{
		ImGui::GetWindowDrawList()->AddCircleFilled(centre, radius, ImColor(color), seg);
	}

	CNI_NORMAL(add_circle_filled)

	void add_text(const ImVec2 &pos, const ImVec4 &color, const string &text)
	{
		ImGui::GetWindowDrawList()->AddText(pos, ImColor(color), text.c_str());
	}

	CNI_NORMAL(add_text)

	void add_image(const image_t &image, const ImVec2 &a, const ImVec2 &b)
	{
		ImGui::GetWindowDrawList()->AddImage(image->get_texture_id(), a, b);
	}

	CNI_NORMAL(add_image)

	void init()
	{
		// Namespaces
		imgui_ext.add_var("applicaion", var::make_protect<extension_t>(imgui_app_ext_shared));
		imgui_ext.add_var("bmp_image", var::make_protect<extension_t>(imgui_img_ext_shared));
		imgui_ext.add_var("keys", var::make_protect<extension_t>(imgui_keys_ext_shared));
		// Application
		imgui_app_ext.add_var("get_window_width",
		                      var::make_protect<callable>(cni(applicaion_cs_ext::get_window_width)));
		imgui_app_ext.add_var("get_window_height",
		                      var::make_protect<callable>(cni(applicaion_cs_ext::get_window_height)));
		imgui_app_ext.add_var("set_window_size", var::make_protect<callable>(cni(applicaion_cs_ext::set_window_size)));
		imgui_app_ext.add_var("set_window_title",
		                      var::make_protect<callable>(cni(applicaion_cs_ext::set_window_title)));
		imgui_app_ext.add_var("set_bg_color", var::make_protect<callable>(cni(applicaion_cs_ext::set_bg_color)));
		imgui_app_ext.add_var("is_closed", var::make_protect<callable>(cni(applicaion_cs_ext::is_closed)));
		imgui_app_ext.add_var("prepare", var::make_protect<callable>(cni(applicaion_cs_ext::prepare)));
		imgui_app_ext.add_var("render", var::make_protect<callable>(cni(applicaion_cs_ext::render)));
		// Image
		imgui_img_ext.add_var("get_width", var::make_protect<callable>(cni(image_cs_ext::get_width)));
		imgui_img_ext.add_var("get_height", var::make_protect<callable>(cni(image_cs_ext::get_height)));
		// Keys
		imgui_keys_ext.add_var("tab", var::make_constant<ImGuiKey>(ImGuiKey_Tab));
		imgui_keys_ext.add_var("left", var::make_constant<ImGuiKey>(ImGuiKey_LeftArrow));
		imgui_keys_ext.add_var("right", var::make_constant<ImGuiKey>(ImGuiKey_RightArrow));
		imgui_keys_ext.add_var("up", var::make_constant<ImGuiKey>(ImGuiKey_UpArrow));
		imgui_keys_ext.add_var("down", var::make_constant<ImGuiKey>(ImGuiKey_DownArrow));
		imgui_keys_ext.add_var("page_up", var::make_constant<ImGuiKey>(ImGuiKey_PageUp));
		imgui_keys_ext.add_var("page_down", var::make_constant<ImGuiKey>(ImGuiKey_PageDown));
		imgui_keys_ext.add_var("home", var::make_constant<ImGuiKey>(ImGuiKey_Home));
		imgui_keys_ext.add_var("end_key", var::make_constant<ImGuiKey>(ImGuiKey_End));
		imgui_keys_ext.add_var("insert", var::make_constant<ImGuiKey>(ImGuiKey_Insert));
		imgui_keys_ext.add_var("delete", var::make_constant<ImGuiKey>(ImGuiKey_Delete));
		imgui_keys_ext.add_var("backspace", var::make_constant<ImGuiKey>(ImGuiKey_Backspace));
		imgui_keys_ext.add_var("space", var::make_constant<ImGuiKey>(ImGuiKey_Space));
		imgui_keys_ext.add_var("enter", var::make_constant<ImGuiKey>(ImGuiKey_Enter));
		imgui_keys_ext.add_var("escape", var::make_constant<ImGuiKey>(ImGuiKey_Escape));
		imgui_keys_ext.add_var("ctrl_a", var::make_constant<ImGuiKey>(ImGuiKey_A));
		imgui_keys_ext.add_var("ctrl_c", var::make_constant<ImGuiKey>(ImGuiKey_C));
		imgui_keys_ext.add_var("ctrl_v", var::make_constant<ImGuiKey>(ImGuiKey_V));
		imgui_keys_ext.add_var("ctrl_x", var::make_constant<ImGuiKey>(ImGuiKey_X));
		imgui_keys_ext.add_var("ctrl_y", var::make_constant<ImGuiKey>(ImGuiKey_Y));
		imgui_keys_ext.add_var("ctrl_z", var::make_constant<ImGuiKey>(ImGuiKey_Z));
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

	template<>
	cs::extension_t &get_ext<imgui_cs_ext::image_t>()
	{
		return imgui_img_ext_shared;
	}

	template<>
	constexpr const char *get_name_of_type<imgui_cs_ext::image_t>()
	{
		return "cs::imgui::image";
	}
}

cs::extension *cs_extension()
{
	imgui_cs_ext::init();
	return &imgui_ext;
}