/*
* Covariant Script ImGUI Extension
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
* Copyright (C) 2017-2021 Michael Lee(李登淳)
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/

#include <imgui.hpp>

// ImGUI Common Header
#include <imgui.h>
#include <imgui_internal.h>

// GL3W/GLFW3
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// Other Headers

#ifdef IMGUI_IMPL_GL2

#include <imgui_gl2.hpp>

#else

#include <imgui_gl3.hpp>

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
		int m_width;
		int m_height;
		GLuint m_textureID;
		unsigned char *m_data;
	public:
		image()=delete;
		image(const image&)=delete;
		image(image&&) noexcept=delete;
		image(unsigned char *data, int width, int height):m_width(width), m_height(height), m_data(data)
		{
			glGenTextures(1, &m_textureID);
			glBindTexture(GL_TEXTURE_2D, m_textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_BGR, GL_UNSIGNED_BYTE, m_data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		~image()
		{
			delete[] m_data;
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
} // namespace imgui_cs

// GLFW Instance
static imgui_cs::glfw_instance glfw_instance;

CNI_ROOT_NAMESPACE {
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

	CNI(get_monitor_count)

	number get_monitor_width(number monitor_id)
	{
		int count = 0;
		GLFWmonitor **monitors = glfwGetMonitors(&count);
		if (monitor_id >= count)
			throw cs::lang_error("Monitor does not exist.");
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitors[static_cast<std::size_t>(monitor_id)]);
		return vidmode->width;
	}

	CNI(get_monitor_width)

	number get_monitor_height(number monitor_id)
	{
		int count = 0;
		GLFWmonitor **monitors = glfwGetMonitors(&count);
		if (monitor_id >= count)
			throw cs::lang_error("Monitor does not exist.");
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitors[static_cast<std::size_t>(monitor_id)]);
		return vidmode->height;
	}

	CNI(get_monitor_height)

// ImGui Application
	application_t fullscreen_application(number monitor_id, const string &title)
	{
		return std::make_shared<imgui_cs::application>(monitor_id, title);
	}

	CNI(fullscreen_application)

	application_t window_application(number width, number height, const string &title)
	{
		return std::make_shared<imgui_cs::application>(width, height, title);
	}

	CNI(window_application)

	CNI_NAMESPACE(application)
	{
		number get_window_width(application_t &app) {
			return app->get_window_width();
		}

		CNI(get_window_width)

		number get_window_height(application_t &app) {
			return app->get_window_height();
		}

		CNI(get_window_height)

		void set_window_size(application_t &app, number width, number height) {
			app->set_window_size(width, height);
		}

		CNI(set_window_size)

		void set_window_title(application_t &app, const string &str) {
			app->set_window_title(str);
		}

		CNI(set_window_title)

		void set_bg_color(application_t &app, const ImVec4 &color) {
			app->set_bg_color(color);
		}

		CNI(set_bg_color)

		bool is_closed(application_t &app) {
			return app->is_closed();
		}

		CNI(is_closed)

		void prepare(application_t &app) {
			app->prepare();
		}

		CNI(prepare)

		void render(application_t &app) {
			app->render();
		}

		CNI(render)
	}

// ImGui Image
	image_t make_image(unsigned char* data, number width, number height)
	{
		return std::make_shared<imgui_cs::image>(data, width, height);
	}

	CNI_CONST(make_image)

	image_t load_bmp_image(const string &path)
	{
		unsigned char header[54];
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
		int width = *(int *) &(header[0x12]);
		int height = *(int *) &(header[0x16]);
		int image_size = *(int *) &(header[0x22]);
		if (image_size == 0)
			image_size = width * height * 3;
		unsigned char *data = new unsigned char[image_size];
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
		return std::make_shared<imgui_cs::image>(data, width, height);
	}

	CNI(load_bmp_image)

	CNI_NAMESPACE(image_type)
	{
		number get_width(const image_t &image) {
			return image->get_width();
		}

		CNI(get_width)

		number get_height(const image_t &image) {
			return image->get_height();
		}

		CNI(get_height)
	}

// ImGui Functions

	CNI_V(get_time, ImGui::GetTime)

	ImVec2 vec2(number a, number b)
	{
		return ImVec2(a, b);
	}

	CNI_CONST(vec2)

	CNI_NAMESPACE(vec2_type)
	{
		CNI_VISITOR_V(x, [](const ImVec2& v){ return v.x; })
		CNI_VISITOR_V(y, [](const ImVec2& v){ return v.y; })
	}

	ImVec4 vec4(number a, number b, number c, number d)
	{
		return ImVec4(a, b, c, d);
	}

	CNI_CONST(vec4)

	CNI_NAMESPACE(vec4_type)
	{
		CNI_VISITOR_V(x, [](const ImVec4& v){ return v.x; })
		CNI_VISITOR_V(y, [](const ImVec4& v){ return v.y; })
		CNI_VISITOR_V(z, [](const ImVec4& v){ return v.z; })
		CNI_VISITOR_V(w, [](const ImVec4& v){ return v.w; })
	}

	number get_framerate()
	{
		return ImGui::GetIO().Framerate;
	}

	CNI(get_framerate)

// Styles and Fonts
	ImFont *add_font(const string &str, number size)
	{
		return ImGui::GetIO().Fonts->AddFontFromFileTTF(str.c_str(), size);
	}

	CNI(add_font)

	ImFont *add_font_chinese(const string &str, number size)
	{
		ImFontConfig font_cfg = ImFontConfig();
		font_cfg.OversampleH = font_cfg.OversampleV = 1;
		return ImGui::GetIO().Fonts->AddFontFromFileTTF(str.c_str(), size, &font_cfg,
		        ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
	}

	CNI(add_font_chinese)

	ImFont *add_font_default(number size)
	{
		ImFontConfig font_cfg = ImFontConfig();
		ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "DefaultFont, %.0fpx", (float) size);
		return ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(imgui_cs::get_default_font_data(), size,
		        &font_cfg);
	}

	CNI(add_font_default)

	ImFont *add_font_extend(const imgui_cs::font &f, number size)
	{
		ImFontConfig font_cfg = ImFontConfig();
		ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "%s, %.0fpx", f.name, (float) size);
		return ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(f.data, size, &font_cfg);
	}

	CNI(add_font_extend)

	ImFont *add_font_extend_cn(const imgui_cs::font &f, number size)
	{
		ImFontConfig font_cfg = ImFontConfig();
		font_cfg.OversampleH = font_cfg.OversampleV = 1;
		ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "%s, %.0fpx", f.name, (float) size);
		return ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(f.data, size, &font_cfg,
		        ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
	}

	CNI(add_font_extend_cn)

	void push_font(ImFont *font)
	{
		ImGui::PushFont(font);
	}

	CNI(push_font)

	void pop_font()
	{
		ImGui::PopFont();
	}

	CNI(pop_font)

	ImFont *get_font()
	{
		return ImGui::GetFont();
	}

	CNI(get_font)

	number get_font_size()
	{
		return ImGui::GetFontSize();
	}

	CNI(get_font_size)

	void set_font_scale(number scale)
	{
		ImGui::GetIO().FontGlobalScale = scale;
	}

	CNI(set_font_scale)

	void style_color_classic()
	{
		ImGui::StyleColorsClassic();
	}

	CNI(style_color_classic)

	void style_color_light()
	{
		ImGui::StyleColorsLight();
	}

	CNI(style_color_light)

	void style_color_dark()
	{
		ImGui::StyleColorsDark();
	}

	CNI(style_color_dark)

// Windows
	void set_next_window_pos(const ImVec2 &pos)
	{
		ImGui::SetNextWindowPos(pos);
	}

	CNI(set_next_window_pos)

	void set_window_pos(const ImVec2 &pos)
	{
		ImGui::SetWindowPos(pos);
	}

	CNI(set_window_pos)

	number get_window_pos_x()
	{
		return ImGui::GetWindowPos().x;
	}

	CNI(get_window_pos_x)

	number get_window_pos_y()
	{
		return ImGui::GetWindowPos().y;
	}

	CNI(get_window_pos_y)

	void set_next_window_size(const ImVec2 &size)
	{
		ImGui::SetNextWindowSize(size);
	}

	CNI(set_next_window_size)

	void set_window_size(const ImVec2 &size)
	{
		ImGui::SetWindowSize(size);
	}

	CNI(set_window_size)

	void set_next_window_collapsed(bool collapsed)
	{
		ImGui::SetNextWindowCollapsed(collapsed);
	}

	CNI(set_next_window_collapsed)

	void set_window_collapsed(bool collapsed)
	{
		ImGui::SetWindowCollapsed(collapsed);
	}

	CNI(set_window_collapsed)

	void set_next_window_focus()
	{
		ImGui::SetNextWindowFocus();
	}

	CNI(set_next_window_focus)

	void set_window_focus()
	{
		ImGui::SetWindowFocus();
	}

	CNI(set_window_focus)

	void set_window_font_scale(number scale)
	{
		ImGui::SetWindowFontScale(scale);
	}

	CNI(set_window_font_scale)

	number get_window_width()
	{
		return ImGui::GetWindowWidth();
	}

	CNI(get_window_width)

	number get_window_height()
	{
		return ImGui::GetWindowHeight();
	}

	CNI(get_window_height)

	void show_demo_window(bool &open)
	{
		ImGui::ShowDemoWindow(&open);
	}

	CNI(show_demo_window)

	void show_about_window(bool &open)
	{
		ImGui::ShowAboutWindow(&open);
	}

	CNI(show_about_window)

	void show_metrics_window(bool &open)
	{
		ImGui::ShowMetricsWindow(&open);
	}

	CNI(show_metrics_window)

	void show_style_editor()
	{
		ImGui::ShowStyleEditor();
	}

	CNI(show_style_editor)

	bool show_style_selector(const string &label)
	{
		return ImGui::ShowStyleSelector(label.c_str());
	}

	CNI(show_style_selector)

	void show_font_selector(const string &label)
	{
		ImGui::ShowFontSelector(label.c_str());
	}

	CNI(show_font_selector)

	void show_user_guide()
	{
		ImGui::ShowUserGuide();
	}

	CNI(show_user_guide)

	void begin_window(const string &str, bool &open, const array &flags_arr)
	{
		ImGuiWindowFlags flags = 0;
		for (auto &it : flags_arr)
			flags |= it.const_val<ImGuiWindowFlags>();
		ImGui::Begin(str.c_str(), &open, flags);
	}

	CNI(begin_window)

	void end_window()
	{
		ImGui::End();
	}

	CNI(end_window)

	void begin_child(const string &str)
	{
		ImGui::BeginChild(str.c_str());
	}

	CNI(begin_child)

	void end_child()
	{
		ImGui::EndChild();
	}

	CNI(end_child)

// Layouts
	void begin_group()
	{
		ImGui::BeginGroup();
	}

	CNI(begin_group)

	void end_group()
	{
		ImGui::EndGroup();
	}

	CNI(end_group)

	void separator()
	{
		ImGui::Separator();
	}

	CNI(separator)

	void same_line()
	{
		ImGui::SameLine();
	}

	CNI(same_line)

	void spacing()
	{
		ImGui::Spacing();
	}

	CNI(spacing)

	void indent()
	{
		ImGui::Indent();
	}

	CNI(indent)

	void unindent()
	{
		ImGui::Unindent();
	}

	CNI(unindent)

// ID scopes
	void push_id(const string &id)
	{
		ImGui::PushID(id.c_str());
	}

	CNI(push_id)

	void pop_id()
	{
		ImGui::PopID();
	}

	CNI(pop_id)

// Widgets
	void text(const string &str)
	{
		ImGui::TextUnformatted(str.c_str());
	}

	CNI(text)

	void text_colored(const ImVec4 &col, const string &str)
	{
		ImGui::TextColored(col, "%s", str.c_str());
	}

	CNI(text_colored)

	void text_disabled(const string &str)
	{
		ImGui::TextDisabled("%s", str.c_str());
	}

	CNI(text_disabled)

	void text_wrappered(const string &str)
	{
		ImGui::TextWrapped("%s", str.c_str());
	}

	CNI(text_wrappered)

	void label_text(const string &label, const string &str)
	{
		ImGui::LabelText(label.c_str(), "%s", str.c_str());
	}

	CNI(label_text)

	void bullet_text(const string &str)
	{
		ImGui::BulletText("%s", str.c_str());
	}

	CNI(bullet_text)

	bool button(const string &str)
	{
		return ImGui::Button(str.c_str());
	}

	CNI(button)

	bool small_button(const string &str)
	{
		return ImGui::SmallButton(str.c_str());
	}

	CNI(small_button)

	bool arrow_button(const string &str, ImGuiDir dir)
	{
		return ImGui::ArrowButton(str.c_str(), dir);
	}

	CNI(arrow_button)

	void image(const image_t &img, const ImVec2 &size)
	{
		ImGui::Image(img->get_texture_id(), size);
	}

	CNI(image)

	bool image_button(const image_t &img, const ImVec2 &size)
	{
		return ImGui::ImageButton(img->get_texture_id(), size);
	}

	CNI(image_button)

	void check_box(const string &str, bool &val)
	{
		ImGui::Checkbox(str.c_str(), &val);
	}

	CNI(check_box)

	void radio_button(const string &str, number &v, number v_button)
	{
		int _v = v;
		ImGui::RadioButton(str.c_str(), &_v, v_button);
		v = _v;
	}

	CNI(radio_button)

	float plot_value_getter(void *data, int idx)
	{
		return (*reinterpret_cast<const array *>(data))[idx].const_val<number>();
	}

	void plot_lines(const string &label, const string &text, const array &data)
	{
		ImGui::PlotLines(label.c_str(), &plot_value_getter, reinterpret_cast<void *>(const_cast<array *>(&data)),
		                 data.size(), 0, text.c_str());
	}

	CNI(plot_lines)

	void plot_histogram(const string &label, const string &text, const array &data)
	{
		ImGui::PlotHistogram(label.c_str(), &plot_value_getter, reinterpret_cast<void *>(const_cast<array *>(&data)),
		                     data.size(), 0, text.c_str());
	}

	CNI(plot_histogram)

	void progress_bar(number fraction, const string &overlay)
	{
		ImGui::ProgressBar(fraction, ImVec2(-1, 0), overlay.c_str());
	}

	CNI(progress_bar)

	void bullet()
	{
		ImGui::Bullet();
	}

	CNI(bullet)

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

	CNI(combo_box)

	void drag_float(const string &label, number &n)
	{
		float f = static_cast<float>(n);
		ImGui::DragFloat(label.c_str(), &f);
		n = f;
	}

	CNI(drag_float)

	void slider_float(const string &str, number &n, number min, number max)
	{
		float f = static_cast<float>(n);
		ImGui::SliderFloat(str.c_str(), &f, min, max);
		n = f;
	}

	CNI(slider_float)

	void input_text(const string &str, string &text, number buff_size)
	{
		imgui_cs::buffer<> buff(buff_size);
		std::strcpy(buff.get(), text.c_str());
		ImGui::InputText(str.c_str(), buff.get(), buff_size);
		text = buff.get();
	}

	CNI(input_text)

	void input_text_s(const string &str, string &text, number buff_size, const array &flags_arr)
	{
		imgui_cs::buffer<> buff(buff_size);
		std::strcpy(buff.get(), text.c_str());
		ImGuiInputTextFlags flags = 0;
		for (auto &it : flags_arr)
			flags |= it.const_val<ImGuiInputTextFlags>();
		ImGui::InputText(str.c_str(), buff.get(), buff_size, flags);
		text = buff.get();
	}

	CNI(input_text_s)

	void input_text_hint(const string &str, const string &hint, string &text, number buff_size)
	{
		imgui_cs::buffer<> buff(buff_size);
		std::strcpy(buff.get(), text.c_str());
		ImGui::InputTextWithHint(str.c_str(), hint.c_str(), buff.get(), buff_size);
		text = buff.get();
	}

	CNI(input_text_hint)

	void input_text_hint_s(const string &str, const string &hint, string &text, number buff_size, const array &flags_arr)
	{
		imgui_cs::buffer<> buff(buff_size);
		std::strcpy(buff.get(), text.c_str());
		ImGuiInputTextFlags flags = 0;
		for (auto &it : flags_arr)
			flags |= it.const_val<ImGuiInputTextFlags>();
		ImGui::InputTextWithHint(str.c_str(), hint.c_str(), buff.get(), buff_size, flags);
		text = buff.get();
	}

	CNI(input_text_hint_s)

	void input_text_multiline(const string &str, string &text, number buff_size)
	{
		imgui_cs::buffer<> buff(buff_size);
		std::strcpy(buff.get(), text.c_str());
		ImGui::InputTextMultiline(str.c_str(), buff.get(), buff_size);
		text = buff.get();
	}

	CNI(input_text_multiline)

	void input_text_multiline_s(const string &str, string &text, number buff_size, const array &flags_arr)
	{
		imgui_cs::buffer<> buff(buff_size);
		std::strcpy(buff.get(), text.c_str());
		ImGuiInputTextFlags flags = 0;
		for (auto &it : flags_arr)
			flags |= it.const_val<ImGuiInputTextFlags>();
		ImGui::InputTextMultiline(str.c_str(), buff.get(), buff_size, ImVec2(0, 0), flags);
		text = buff.get();
	}

	CNI(input_text_multiline_s)

	void color_edit3(const string &str, ImVec4 &color)
	{
		ImGui::ColorEdit3(str.c_str(), reinterpret_cast<float *>(&color));
	}

	CNI(color_edit3)

	void color_edit4(const string &str, ImVec4 &color)
	{
		ImGui::ColorEdit4(str.c_str(), reinterpret_cast<float *>(&color));
	}

	CNI(color_edit4)

// Trees
	bool tree_node(const string &label)
	{
		return ImGui::TreeNode(label.c_str());
	}

	CNI(tree_node)

	void tree_pop()
	{
		ImGui::TreePop();
	}

	CNI(tree_pop)

// Selectable / Lists
	void selectable(const string &str, bool &selected)
	{
		ImGui::Selectable(str.c_str(), &selected);
	}

	CNI(selectable)

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

	CNI(list_box)

// Tooltips
	void set_tooltip(const string &str)
	{
		ImGui::SetTooltip("%s", str.c_str());
	}

	CNI(set_tooltip)

	void begin_tooltip()
	{
		ImGui::BeginTooltip();
	}

	CNI(begin_tooltip)

	void end_tooltip()
	{
		ImGui::EndTooltip();
	}

	CNI(end_tooltip)

// Menus
	bool begin_main_menu_bar()
	{
		return ImGui::BeginMainMenuBar();
	}

	CNI(begin_main_menu_bar)

	void end_main_menu_bar()
	{
		ImGui::EndMainMenuBar();
	}

	CNI(end_main_menu_bar)

	bool begin_menu_bar()
	{
		return ImGui::BeginMenuBar();
	}

	CNI(begin_menu_bar)

	void end_menu_bar()
	{
		ImGui::EndMenuBar();
	}

	CNI(end_menu_bar)

	bool begin_menu(const string &str, bool enabled)
	{
		return ImGui::BeginMenu(str.c_str(), enabled);
	}

	CNI(begin_menu)

	void end_menu()
	{
		ImGui::EndMenu();
	}

	CNI(end_menu)

	bool menu_item(const string &str, const string &shortcut, bool enabled)
	{
		return ImGui::MenuItem(str.c_str(), shortcut.c_str(), false, enabled);
	}

	CNI(menu_item)

// Popups
	void open_popup(const string &id)
	{
		ImGui::OpenPopup(id.c_str());
	}

	CNI(open_popup)

	bool begin_popup(const string &id)
	{
		return ImGui::BeginPopup(id.c_str());
	}

	CNI(begin_popup)

	bool begin_popup_item(const string &id)
	{
		return ImGui::BeginPopupContextItem(id.c_str());
	}

	CNI(begin_popup_item)

	bool begin_popup_window()
	{
		return ImGui::BeginPopupContextWindow();
	}

	CNI(begin_popup_window)

	bool begin_popup_background()
	{
		return ImGui::BeginPopupContextVoid();
	}

	CNI(begin_popup_background)

	bool begin_popup_modal(const string &str, bool &open, const array &flags_arr)
	{
		ImGuiWindowFlags flags = 0;
		for (auto &it : flags_arr)
			flags |= it.const_val<ImGuiWindowFlags>();
		return ImGui::BeginPopupModal(str.c_str(), &open, flags);
	}

	CNI(begin_popup_modal)

	void end_popup()
	{
		ImGui::EndPopup();
	}

	CNI(end_popup)

	void close_current_popup()
	{
		ImGui::CloseCurrentPopup();
	}

	CNI(close_current_popup)

// Tab Bars, Tabs
	bool begin_tab_bar(const string &str)
	{
		return ImGui::BeginTabBar(str.c_str());
	}

	CNI(begin_tab_bar)

	void end_tab_bar()
	{
		ImGui::EndTabBar();
	}

	CNI(end_tab_bar)

	bool begin_tab_item(const string &str, bool &open, const array &flags_arr)
	{
		ImGuiTabItemFlags flags = 0;
		for (auto &it : flags_arr)
			flags |= it.const_val<ImGuiTabItemFlags>();
		return ImGui::BeginTabItem(str.c_str(), &open, flags);
	}

	CNI(begin_tab_item)

	void end_tab_item()
	{
		ImGui::EndTabItem();
	}

	CNI(end_tab_item)

	void set_tab_item_closed(const string &str)
	{
		ImGui::SetTabItemClosed(str.c_str());
	}

	CNI(set_tab_item_closed)

// Columns
	void columns(number count, const string &id, bool border)
	{
		ImGui::Columns(count, id.c_str(), border);
	}

	CNI(columns)

	void next_column()
	{
		ImGui::NextColumn();
	}

	CNI(next_column)

	number get_column_index()
	{
		return ImGui::GetColumnIndex();
	}

	CNI(get_column_index)

	number get_column_width(number index)
	{
		return ImGui::GetColumnWidth();
	}

	CNI(get_column_width)

	void set_column_width(number index, number width)
	{
		ImGui::SetColumnWidth(index, width);
	}

	CNI(set_column_width)

	number get_column_offset(number index)
	{
		return ImGui::GetColumnOffset(index);
	}

	CNI(get_column_offset)

	void set_column_offset(number index, number offset)
	{
		ImGui::SetColumnOffset(index, offset);
	}

	CNI(set_column_offset)

	number get_columns_count()
	{
		return ImGui::GetColumnsCount();
	}

	CNI(get_columns_count)

// Focus, Activation
	void set_scroll_here()
	{
		ImGui::SetScrollHereY();
	}

	CNI(set_scroll_here)

	void set_keyboard_focus_here()
	{
		ImGui::SetKeyboardFocusHere();
	}

	CNI(set_keyboard_focus_here)

// Utilities
	bool is_item_hovered()
	{
		return ImGui::IsItemHovered();
	}

	CNI(is_item_hovered)

	bool is_item_active()
	{
		return ImGui::IsItemActive();
	}

	CNI(is_item_active)

	bool is_item_focused()
	{
		return ImGui::IsItemFocused();
	}

	CNI(is_item_focused)

	bool is_item_clicked(number button)
	{
		return ImGui::IsItemClicked(button);
	}

	CNI(is_item_clicked)

	bool is_item_visible()
	{
		return ImGui::IsItemVisible();
	}

	CNI(is_item_visible)

	bool is_any_item_hovered()
	{
		return ImGui::IsAnyItemHovered();
	}

	CNI(is_any_item_hovered)

	bool is_any_item_active()
	{
		return ImGui::IsAnyItemActive();
	}

	CNI(is_any_item_active)

	bool is_any_item_focused()
	{
		return ImGui::IsAnyItemFocused();
	}

	CNI(is_any_item_focused)

// Inputs
	number get_key_index(ImGuiKey key)
	{
		return ImGui::GetKeyIndex(key);
	}

	CNI(get_key_index)

	bool is_key_down(number key)
	{
		return ImGui::IsKeyDown(key);
	}

	CNI(is_key_down)

	bool is_key_pressed(number key)
	{
		return ImGui::IsKeyPressed(key);
	}

	CNI(is_key_pressed)

	bool is_key_released(number key)
	{
		return ImGui::IsKeyReleased(key);
	}

	CNI(is_key_released)

	bool is_mouse_down(number button)
	{
		return ImGui::IsMouseDown(button);
	}

	CNI(is_mouse_down)

	bool is_any_mouse_down()
	{
		return ImGui::IsAnyMouseDown();
	}

	CNI(is_any_mouse_down)

	bool is_mouse_clicked(number button)
	{
		return ImGui::IsMouseClicked(button);
	}

	CNI(is_mouse_clicked)

	bool is_mouse_double_clicked(number button)
	{
		return ImGui::IsMouseDoubleClicked(button);
	}

	CNI(is_mouse_double_clicked)

	bool is_mouse_released(number button)
	{
		return ImGui::IsMouseReleased(button);
	}

	CNI(is_mouse_released)

	bool is_mouse_dragging(number button)
	{
		return ImGui::IsMouseDragging(button);
	}

	CNI(is_mouse_dragging)

	number get_mouse_pos_x()
	{
		return ImGui::GetMousePos().x;
	}

	CNI(get_mouse_pos_x)

	number get_mouse_pos_y()
	{
		return ImGui::GetMousePos().y;
	}

	CNI(get_mouse_pos_y)

	number get_mouse_drag_delta_x()
	{
		return ImGui::GetMouseDragDelta().x;
	}

	CNI(get_mouse_drag_delta_x)

	number get_mouse_drag_delta_y()
	{
		return ImGui::GetMouseDragDelta().y;
	}

	CNI(get_mouse_drag_delta_y)

	string get_clipboard_text()
	{
		return ImGui::GetClipboardText();
	}

	CNI(get_clipboard_text)

	void set_clipboard_text(const string &str)
	{
		ImGui::SetClipboardText(str.c_str());
	}

	CNI(set_clipboard_text)

// Canvas
	void add_line(const ImVec2 &a, const ImVec2 &b, const ImVec4 &color, number thickness)
	{
		ImGui::GetWindowDrawList()->AddLine(a, b, ImColor(color), thickness);
	}

	CNI(add_line)

	void add_rect(const ImVec2 &a, const ImVec2 &b, const ImVec4 &color, number rounding, number thickness)
	{
		ImGui::GetWindowDrawList()->AddRect(a, b, ImColor(color), rounding, ImDrawCornerFlags_All,
		                                    thickness);
	}

	CNI(add_rect)

	void add_rect_filled(const ImVec2 &a, const ImVec2 &b, const ImVec4 &color, number rounding)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(a, b, ImColor(color), rounding, ImDrawCornerFlags_All);
	}

	CNI(add_rect_filled)

	void add_quad(const ImVec2 &a, const ImVec2 &b, const ImVec2 &c, const ImVec2 &d, const ImVec4 &color,
	              number thickness)
	{
		ImGui::GetWindowDrawList()->AddQuad(a, b, c, d, ImColor(color), thickness);
	}

	CNI(add_quad)

	void add_quad_filled(const ImVec2 &a, const ImVec2 &b, const ImVec2 &c, const ImVec2 &d, const ImVec4 &color)
	{
		ImGui::GetWindowDrawList()->AddQuadFilled(a, b, c, d, ImColor(color));
	}

	CNI(add_quad_filled)

	void add_triangle(const ImVec2 &a, const ImVec2 &b, const ImVec2 &c, const ImVec4 &color, number thickness)
	{
		ImGui::GetWindowDrawList()->AddTriangle(a, b, c, ImColor(color), thickness);
	}

	CNI(add_triangle)

	void add_triangle_filled(const ImVec2 &a, const ImVec2 &b, const ImVec2 &c, const ImVec4 &color)
	{
		ImGui::GetWindowDrawList()->AddTriangleFilled(a, b, c, ImColor(color));
	}

	CNI(add_triangle_filled)

	void add_circle(const ImVec2 &centre, number radius, const ImVec4 &color, number seg, number thickness)
	{
		ImGui::GetWindowDrawList()->AddCircle(centre, radius, ImColor(color), seg, thickness);
	}

	CNI(add_circle)

	void add_circle_filled(const ImVec2 &centre, number radius, const ImVec4 &color, number seg)
	{
		ImGui::GetWindowDrawList()->AddCircleFilled(centre, radius, ImColor(color), seg);
	}

	CNI(add_circle_filled)

	void add_text(ImFont *font, number size, const ImVec2 &pos, const ImVec4 &color, const string &text)
	{
		ImGui::GetWindowDrawList()->AddText(font, size, pos, ImColor(color), text.c_str());
	}

	CNI(add_text)

	void add_image(const image_t &image, const ImVec2 &a, const ImVec2 &b)
	{
		ImGui::GetWindowDrawList()->AddImage(image->get_texture_id(), a, b);
	}

	CNI(add_image)

	CNI_NAMESPACE(keys)
	{
		CNI_VALUE_CONST_V(tab, ImGuiKey, ImGuiKey_Tab)
		CNI_VALUE_CONST_V(left, ImGuiKey, ImGuiKey_LeftArrow)
		CNI_VALUE_CONST_V(right, ImGuiKey, ImGuiKey_RightArrow)
		CNI_VALUE_CONST_V(up, ImGuiKey, ImGuiKey_UpArrow)
		CNI_VALUE_CONST_V(down, ImGuiKey, ImGuiKey_DownArrow)
		CNI_VALUE_CONST_V(page_up, ImGuiKey, ImGuiKey_PageUp)
		CNI_VALUE_CONST_V(page_down, ImGuiKey, ImGuiKey_PageDown)
		CNI_VALUE_CONST_V(home, ImGuiKey, ImGuiKey_Home)
		CNI_VALUE_CONST_V(end_key, ImGuiKey, ImGuiKey_End)
		CNI_VALUE_CONST_V(insert, ImGuiKey, ImGuiKey_Insert)
		CNI_VALUE_CONST_V(delete, ImGuiKey, ImGuiKey_Delete)
		CNI_VALUE_CONST_V(backspace, ImGuiKey, ImGuiKey_Backspace)
		CNI_VALUE_CONST_V(space, ImGuiKey, ImGuiKey_Space)
		CNI_VALUE_CONST_V(enter, ImGuiKey, ImGuiKey_Enter)
		CNI_VALUE_CONST_V(escape, ImGuiKey, ImGuiKey_Escape)
		CNI_VALUE_CONST_V(ctrl_a, ImGuiKey, ImGuiKey_A)
		CNI_VALUE_CONST_V(ctrl_c, ImGuiKey, ImGuiKey_C)
		CNI_VALUE_CONST_V(ctrl_v, ImGuiKey, ImGuiKey_V)
		CNI_VALUE_CONST_V(ctrl_x, ImGuiKey, ImGuiKey_X)
		CNI_VALUE_CONST_V(ctrl_y, ImGuiKey, ImGuiKey_Y)
		CNI_VALUE_CONST_V(ctrl_z, ImGuiKey, ImGuiKey_Z)
	}

	CNI_NAMESPACE(dirs)
	{
		CNI_VALUE_CONST_V(left, ImGuiDir, ImGuiDir_Left)
		CNI_VALUE_CONST_V(right, ImGuiDir, ImGuiDir_Right)
		CNI_VALUE_CONST_V(up, ImGuiDir, ImGuiDir_Up)
		CNI_VALUE_CONST_V(down, ImGuiDir, ImGuiDir_Down)
	}

	CNI_NAMESPACE(flags)
	{
		CNI_VALUE_CONST_V(no_title_bar, ImGuiWindowFlags, ImGuiWindowFlags_NoTitleBar)
		CNI_VALUE_CONST_V(no_resize, ImGuiWindowFlags, ImGuiWindowFlags_NoResize)
		CNI_VALUE_CONST_V(no_move, ImGuiWindowFlags, ImGuiWindowFlags_NoMove)
		CNI_VALUE_CONST_V(no_scroll_bar, ImGuiWindowFlags, ImGuiWindowFlags_NoScrollbar)
		CNI_VALUE_CONST_V(no_collapse, ImGuiWindowFlags, ImGuiWindowFlags_NoCollapse)
		CNI_VALUE_CONST_V(always_auto_resize, ImGuiWindowFlags, ImGuiWindowFlags_AlwaysAutoResize)
		CNI_VALUE_CONST_V(no_saved_settings, ImGuiWindowFlags, ImGuiWindowFlags_NoSavedSettings)
		CNI_VALUE_CONST_V(menu_bar, ImGuiWindowFlags, ImGuiWindowFlags_MenuBar)
		CNI_VALUE_CONST_V(horizontal_scroll_bar, ImGuiWindowFlags, ImGuiWindowFlags_HorizontalScrollbar)
		CNI_VALUE_CONST_V(unsaved_document, ImGuiTabItemFlags, ImGuiTabItemFlags_UnsavedDocument)
		CNI_VALUE_CONST_V(set_selected, ImGuiTabItemFlags, ImGuiTabItemFlags_SetSelected)
		CNI_VALUE_CONST_V(allow_tab, ImGuiInputTextFlags, ImGuiInputTextFlags_AllowTabInput)
		CNI_VALUE_CONST_V(read_only, ImGuiInputTextFlags, ImGuiInputTextFlags_ReadOnly)
		CNI_VALUE_CONST_V(password, ImGuiInputTextFlags, ImGuiInputTextFlags_Password)
	}
}

CNI_ENABLE_TYPE_EXT_V(application, cni_root_namespace::application_t, cs::imgui::application)
CNI_ENABLE_TYPE_EXT_V(image_type, cni_root_namespace::image_t, cs::imgui::image)
CNI_ENABLE_TYPE_EXT_V(vec2_type, ImVec2, cs::imgui::vec2)
CNI_ENABLE_TYPE_EXT_V(vec4_type, ImVec4, cs::imgui::vec4)