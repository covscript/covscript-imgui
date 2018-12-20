/*
* Covariant Script ImGUI Extension
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/

// Covariant Script Header
#include <covscript/dll.hpp>

// ImGUI Common Header
#include <imgui.h>
#include <imgui_internal.h>

// GL3W/GLFW3
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// Other Headers
#include <cstring>
#include <cstdio>
#include <vector>

#ifdef IMGUI_IMPL_GL2
#include "./imgui_gl2.hpp"
#else

#include "./imgui_gl3.hpp"

#endif

namespace imgui_cs {
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
			if (fread(data, 1, image_size, file) != image_size)
				throw cs::lang_error("Broken BMP file");
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
static cs::namespace_t imgui_ext=cs::make_shared_namespace<cs::name_space>();
static cs::namespace_t imgui_app_ext=cs::make_shared_namespace<cs::name_space>();
static cs::namespace_t imgui_img_ext=cs::make_shared_namespace<cs::name_space>();
static cs::namespace_t imgui_keys_ext=cs::make_shared_namespace<cs::name_space>();
static cs::namespace_t imgui_dirs_ext=cs::make_shared_namespace<cs::name_space>();
static cs::namespace_t imgui_flags_ext=cs::make_shared_namespace<cs::name_space>();

class cni_register final {
public:
	static std::vector<cni_register*> register_list;
	std::string name;
	cs::var func;
	template<typename _fT>
	cni_register(const char *str, _fT &&func, bool is_const):name(str), func(cs::make_cni(func, is_const)){
		register_list.push_back(this);
	}
};

std::vector<cni_register*> cni_register::register_list;

#define CNI_NAME_MIXER(PREFIX, NAME) static cni_register PREFIX##NAME
#define CNI_REGISTER(NAME, ARGS) CNI_NAME_MIXER(_cni_register_, NAME) \
\
\
(#NAME,                                                               \
 NAME, ARGS);
#define CNI_NORMAL(name) CNI_REGISTER(name, false)
#define CNI_CONST(name) CNI_REGISTER(name, true)

namespace imgui_cs_ext {
	using namespace cs;
	using application_t = std::shared_ptr<imgui_cs::application>;
	using image_t = std::shared_ptr<imgui_cs::image>;

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
		ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->AddFontFromFileTTF(str.c_str(), size);
	}

	CNI_NORMAL(add_font)

	void add_font_default(number size)
	{
		ImFontConfig font_cfg = ImFontConfig();
        ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "DefaultFont, %.0fpx", (float)size);
		ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(imgui_cs::get_droidsans_ttf_data(), size, &font_cfg);
	}

	CNI_NORMAL(add_font_default)

	void set_font_scale(number scale)
	{
		ImGui::GetIO().FontGlobalScale = scale;
	}

	CNI_NORMAL(set_font_scale)

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

	void set_next_window_focus()
	{
		ImGui::SetNextWindowFocus();
	}

	CNI_NORMAL(set_next_window_focus)

	void set_window_focus()
	{
		ImGui::SetWindowFocus();
	}

	CNI_NORMAL(set_window_focus)

	void set_window_font_scale(number scale)
	{
		ImGui::SetWindowFontScale(scale);
	}

	CNI_NORMAL(set_window_font_scale)

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

	void show_about_window(bool &open)
	{
		ImGui::ShowAboutWindow(&open);
	}

	CNI_NORMAL(show_about_window)

	void show_metrics_window(bool &open)
	{
		ImGui::ShowMetricsWindow(&open);
	}

	CNI_NORMAL(show_metrics_window)

	void show_style_editor()
	{
		ImGui::ShowStyleEditor();
	}

	CNI_NORMAL(show_style_editor)

	bool show_style_selector(const string &label)
	{
		return ImGui::ShowStyleSelector(label.c_str());
	}

	CNI_NORMAL(show_style_selector)

	void show_font_selector(const string &label)
	{
		ImGui::ShowFontSelector(label.c_str());
	}

	CNI_NORMAL(show_font_selector)

	void show_user_guide()
	{
		ImGui::ShowUserGuide();
	}

	CNI_NORMAL(show_user_guide)

	void begin_window(const string &str, bool &open, const array &flags_arr)
	{
		ImGuiWindowFlags flags = 0;
		for (auto &it : flags_arr)
			flags |= it.const_val<ImGuiWindowFlags>();
		ImGui::Begin(str.c_str(), &open, flags);
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

	void unindent()
	{
		ImGui::Unindent();
	}

	CNI_NORMAL(unindent)

// ID scopes
	void push_id(const string &id)
	{
		ImGui::PushID(id.c_str());
	}

	CNI_NORMAL(push_id)

	void pop_id()
	{
		ImGui::PopID();
	}

	CNI_NORMAL(pop_id)

// Widgets
	void text(const string &str)
	{
		ImGui::TextUnformatted(str.c_str());
	}

	CNI_NORMAL(text)

	void text_colored(const ImVec4 &col, const string &str)
	{
		ImGui::TextColored(col, "%s", str.c_str());
	}

	CNI_NORMAL(text_colored)

	void text_disabled(const string &str)
	{
		ImGui::TextDisabled("%s", str.c_str());
	}

	CNI_NORMAL(text_disabled)

	void text_wrappered(const string &str)
	{
		ImGui::TextWrapped("%s", str.c_str());
	}

	CNI_NORMAL(text_wrappered)

	void label_text(const string &label, const string &str)
	{
		ImGui::LabelText(label.c_str(), "%s", str.c_str());
	}

	CNI_NORMAL(label_text)

	void bullet_text(const string &str)
	{
		ImGui::BulletText("%s", str.c_str());
	}

	CNI_NORMAL(bullet_text)

	bool button(const string &str)
	{
		return ImGui::Button(str.c_str());
	}

	CNI_NORMAL(button)

	bool small_button(const string &str)
	{
		return ImGui::SmallButton(str.c_str());
	}

	CNI_NORMAL(small_button)

	bool arrow_button(const string &str, ImGuiDir dir)
	{
		return ImGui::ArrowButton(str.c_str(), dir);
	}

	CNI_NORMAL(arrow_button)

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

	float plot_value_getter(void *data, int idx)
	{
		return (*reinterpret_cast<const array *>(data))[idx].const_val<number>();
	}

	void plot_lines(const string &label, const string &text, const array &data)
	{
		ImGui::PlotLines(label.c_str(), &plot_value_getter, reinterpret_cast<void *>(const_cast<array *>(&data)),
		                 data.size(), 0, text.c_str());
	}

	CNI_NORMAL(plot_lines)

	void plot_histogram(const string &label, const string &text, const array &data)
	{
		ImGui::PlotHistogram(label.c_str(), &plot_value_getter, reinterpret_cast<void *>(const_cast<array *>(&data)),
		                     data.size(), 0, text.c_str());
	}

	CNI_NORMAL(plot_histogram)

	void progress_bar(number fraction, const string &overlay)
	{
		ImGui::ProgressBar(fraction, ImVec2(-1, 0), overlay.c_str());
	}

	CNI_NORMAL(progress_bar)

	void bullet()
	{
		ImGui::Bullet();
	}

	CNI_NORMAL(bullet)

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

	void drag_float(const string &label, number &n)
	{
		float f = static_cast<float>(n);
		ImGui::DragFloat(label.c_str(), &f);
		n = f;
	}

	CNI_NORMAL(drag_float)

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
		return ImGui::TreeNode(label.c_str());
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
		current = _current;
		delete[] _items;
	}

	CNI_NORMAL(list_box)

// Tooltips
	void set_tooltip(const string &str)
	{
		ImGui::SetTooltip("%s", str.c_str());
	}

	CNI_NORMAL(set_tooltip)

	void begin_tooltip()
	{
		ImGui::BeginTooltip();
	}

	CNI_NORMAL(begin_tooltip)

	void end_tooltip()
	{
		ImGui::EndTooltip();
	}

	CNI_NORMAL(end_tooltip)

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

// Popups
	void open_popup(const string &id)
	{
		ImGui::OpenPopup(id.c_str());
	}

	CNI_NORMAL(open_popup)

	bool begin_popup(const string &id)
	{
		return ImGui::BeginPopup(id.c_str());
	}

	CNI_NORMAL(begin_popup)

	bool begin_popup_item(const string &id)
	{
		return ImGui::BeginPopupContextItem(id.c_str());
	}

	CNI_NORMAL(begin_popup_item)

	bool begin_popup_window()
	{
		return ImGui::BeginPopupContextWindow();
	}

	CNI_NORMAL(begin_popup_window)

	bool begin_popup_background()
	{
		return ImGui::BeginPopupContextVoid();
	}

	CNI_NORMAL(begin_popup_background)

	bool begin_popup_modal(const string &str, bool &open, const array &flags_arr)
	{
		ImGuiWindowFlags flags = 0;
		for (auto &it : flags_arr)
			flags |= it.const_val<ImGuiWindowFlags>();
		return ImGui::BeginPopupModal(str.c_str(), &open, flags);
	}

	CNI_NORMAL(begin_popup_modal)

	void end_popup()
	{
		ImGui::EndPopup();
	}

	CNI_NORMAL(end_popup)

	void close_current_popup()
	{
		ImGui::CloseCurrentPopup();
	}

	CNI_NORMAL(close_current_popup);

// Columns
	void columns(number count, const string &id, bool border)
	{
		ImGui::Columns(count, id.c_str(), border);
	}

	CNI_NORMAL(columns)

	void next_column()
	{
		ImGui::NextColumn();
	}

	CNI_NORMAL(next_column)

	number get_column_index()
	{
		return ImGui::GetColumnIndex();
	}

	CNI_NORMAL(get_column_index)

	number get_column_width(number index)
	{
		return ImGui::GetColumnWidth();
	}

	CNI_NORMAL(get_column_width)

	void set_column_width(number index, number width)
	{
		ImGui::SetColumnWidth(index, width);
	}

	CNI_NORMAL(set_column_width)

	number get_column_offset(number index)
	{
		return ImGui::GetColumnOffset(index);
	}

	CNI_NORMAL(get_column_offset)

	void set_column_offset(number index, number offset)
	{
		ImGui::SetColumnOffset(index, offset);
	}

	CNI_NORMAL(set_column_offset)

	number get_columns_count()
	{
		return ImGui::GetColumnsCount();
	}

	CNI_NORMAL(get_columns_count)

// Focus, Activation
	void set_scroll_here()
	{
		ImGui::SetScrollHere();
	}

	CNI_NORMAL(set_scroll_here)

	void set_keyboard_focus_here()
	{
		ImGui::SetKeyboardFocusHere();
	}

	CNI_NORMAL(set_keyboard_focus_here)

// Utilities
	bool is_item_hovered()
	{
		return ImGui::IsItemHovered();
	}

	CNI_NORMAL(is_item_hovered)

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

	void init(name_space *imgui_ext)
	{
		// Functions
		for(auto& reg:cni_register::register_list)
			imgui_ext->add_var(reg->name, reg->func);
		// Namespaces
		(*imgui_ext)
		.add_var("applicaion", make_namespace(imgui_app_ext))
		.add_var("bmp_image", make_namespace(imgui_img_ext))
		.add_var("keys", make_namespace(imgui_keys_ext))
		.add_var("dirs", make_namespace(imgui_dirs_ext))
		.add_var("flags", make_namespace(imgui_flags_ext));
		// Application
		(*imgui_app_ext)
		.add_var("get_window_width", make_cni(applicaion_cs_ext::get_window_width))
		.add_var("get_window_height", make_cni(applicaion_cs_ext::get_window_height))
		.add_var("set_window_size", make_cni(applicaion_cs_ext::set_window_size))
		.add_var("set_window_title", make_cni(applicaion_cs_ext::set_window_title))
		.add_var("set_bg_color", make_cni(applicaion_cs_ext::set_bg_color))
		.add_var("is_closed", make_cni(applicaion_cs_ext::is_closed))
		.add_var("prepare", make_cni(applicaion_cs_ext::prepare))
		.add_var("render", make_cni(applicaion_cs_ext::render));
		// Image
		(*imgui_img_ext)
		.add_var("get_width", make_cni(image_cs_ext::get_width))
		.add_var("get_height", make_cni(image_cs_ext::get_height));
		// Keys
		(*imgui_keys_ext)
		.add_var("tab", var::make_constant<ImGuiKey>(ImGuiKey_Tab))
		.add_var("left", var::make_constant<ImGuiKey>(ImGuiKey_LeftArrow))
		.add_var("right", var::make_constant<ImGuiKey>(ImGuiKey_RightArrow))
		.add_var("up", var::make_constant<ImGuiKey>(ImGuiKey_UpArrow))
		.add_var("down", var::make_constant<ImGuiKey>(ImGuiKey_DownArrow))
		.add_var("page_up", var::make_constant<ImGuiKey>(ImGuiKey_PageUp))
		.add_var("page_down", var::make_constant<ImGuiKey>(ImGuiKey_PageDown))
		.add_var("home", var::make_constant<ImGuiKey>(ImGuiKey_Home))
		.add_var("end_key", var::make_constant<ImGuiKey>(ImGuiKey_End))
		.add_var("insert", var::make_constant<ImGuiKey>(ImGuiKey_Insert))
		.add_var("delete", var::make_constant<ImGuiKey>(ImGuiKey_Delete))
		.add_var("backspace", var::make_constant<ImGuiKey>(ImGuiKey_Backspace))
		.add_var("space", var::make_constant<ImGuiKey>(ImGuiKey_Space))
		.add_var("enter", var::make_constant<ImGuiKey>(ImGuiKey_Enter))
		.add_var("escape", var::make_constant<ImGuiKey>(ImGuiKey_Escape))
		.add_var("ctrl_a", var::make_constant<ImGuiKey>(ImGuiKey_A))
		.add_var("ctrl_c", var::make_constant<ImGuiKey>(ImGuiKey_C))
		.add_var("ctrl_v", var::make_constant<ImGuiKey>(ImGuiKey_V))
		.add_var("ctrl_x", var::make_constant<ImGuiKey>(ImGuiKey_X))
		.add_var("ctrl_y", var::make_constant<ImGuiKey>(ImGuiKey_Y))
		.add_var("ctrl_z", var::make_constant<ImGuiKey>(ImGuiKey_Z));
		// Dirs
		(*imgui_dirs_ext)
		.add_var("left", var::make_constant<ImGuiDir>(ImGuiDir_Left))
		.add_var("right", var::make_constant<ImGuiDir>(ImGuiDir_Right))
		.add_var("up", var::make_constant<ImGuiDir>(ImGuiDir_Up))
		.add_var("down", var::make_constant<ImGuiDir>(ImGuiDir_Down));
		// Flags
		(*imgui_flags_ext)
		.add_var("no_title_bar", var::make_constant<ImGuiWindowFlags>(ImGuiWindowFlags_NoTitleBar))
		.add_var("no_resize", var::make_constant<ImGuiWindowFlags>(ImGuiWindowFlags_NoResize))
		.add_var("no_move", var::make_constant<ImGuiWindowFlags>(ImGuiWindowFlags_NoMove))
		.add_var("no_scroll_bar", var::make_constant<ImGuiWindowFlags>(ImGuiWindowFlags_NoScrollbar))
		.add_var("no_collapse", var::make_constant<ImGuiWindowFlags>(ImGuiWindowFlags_NoCollapse))
		.add_var("always_auto_resize", var::make_constant<ImGuiWindowFlags>(ImGuiWindowFlags_AlwaysAutoResize))
		.add_var("no_saved_settings", var::make_constant<ImGuiWindowFlags>(ImGuiWindowFlags_NoSavedSettings))
		.add_var("menu_bar", var::make_constant<ImGuiWindowFlags>(ImGuiWindowFlags_MenuBar))
		.add_var("horizontal_scroll_bar", var::make_constant<ImGuiWindowFlags>(ImGuiWindowFlags_HorizontalScrollbar));
	}
}

namespace cs_impl {
	template<>
	cs::namespace_t &get_ext<imgui_cs_ext::application_t>()
	{
		return imgui_app_ext;
	}

	template<>
	constexpr const char *get_name_of_type<imgui_cs_ext::application_t>()
	{
		return "cs::imgui::application";
	}

	template<>
	cs::namespace_t &get_ext<imgui_cs_ext::image_t>()
	{
		return imgui_img_ext;
	}

	template<>
	constexpr const char *get_name_of_type<imgui_cs_ext::image_t>()
	{
		return "cs::imgui::image";
	}
}

void cs_extension_main(cs::name_space* ns)
{
	imgui_cs_ext::init(ns);
}