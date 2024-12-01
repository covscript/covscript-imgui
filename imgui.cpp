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
* Copyright (C) 2017-2024 Michael Lee(李登淳)
*
* Email:   mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: https://covscript.org.cn
*/

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
// #define IMGUI_IMPL_WIN32
#endif

#include <imgui.hpp>

// Common Header
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#ifdef IMGUI_IMPL_WIN32
#ifdef IMGUI_IMPL_DX9
#include <imgui_dx9_impl.hpp>
#else
#include <imgui_dx11_impl.hpp>
#endif
#else
#ifdef IMGUI_IMPL_GL2
#include <imgui_gl2_impl.hpp>
#else
#include <imgui_gl3_impl.hpp>
#endif
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
} // namespace imgui_cs

CNI_ROOT_NAMESPACE {
	using namespace cs;
	using namespace imgui_cs;
	using application_t = std::shared_ptr<application>;
	using image_t = std::shared_ptr<image>;

	CNI(get_monitor_count)

	CNI(get_monitor_width)

	CNI(get_monitor_height)

// ImGui Application
	application_t fullscreen_application(std::size_t monitor_id, const string &title)
	{
		return std::make_shared<application>(monitor_id, title);
	}

	CNI(fullscreen_application)

	application_t window_application(std::size_t width, std::size_t height, const string &title)
	{
		return std::make_shared<application>(width, height, title);
	}

	CNI(window_application)

	CNI_NAMESPACE(application)
	{
		int get_window_width(application_t &app) {
			return app->get_window_width();
		}

		CNI(get_window_width)

		int get_window_height(application_t &app) {
			return app->get_window_height();
		}

		CNI(get_window_height)

		void set_window_size(application_t &app, int width, int height) {
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
	image_t load_image(const string &path)
	{
		return std::make_shared<image>(path);
	}

	CNI(load_image)

	CNI_NAMESPACE(image_type)
	{
		int get_width(const image_t &image) {
			return image->get_width();
		}

		CNI(get_width)

		int get_height(const image_t &image) {
			return image->get_height();
		}

		CNI(get_height)
	}

// ImGui Functions

	CNI_V(get_time, ImGui::GetTime)

	ImVec2 vec2(float a, float b)
	{
		return ImVec2(a, b);
	}

	CNI_CONST(vec2)

	CNI_NAMESPACE(vec2_type)
	{
		CNI_VISITOR_V(x, [](const ImVec2& v) {
			return v.x;
		})
		CNI_VISITOR_V(y, [](const ImVec2& v) {
			return v.y;
		})
	}

	ImVec4 vec4(float a, float b, float c, float d)
	{
		return ImVec4(a, b, c, d);
	}

	CNI_CONST(vec4)

	CNI_NAMESPACE(vec4_type)
	{
		CNI_VISITOR_V(x, [](const ImVec4& v) {
			return v.x;
		})
		CNI_VISITOR_V(y, [](const ImVec4& v) {
			return v.y;
		})
		CNI_VISITOR_V(z, [](const ImVec4& v) {
			return v.z;
		})
		CNI_VISITOR_V(w, [](const ImVec4& v) {
			return v.w;
		})
	}

	float get_framerate()
	{
		return ImGui::GetIO().Framerate;
	}

	CNI(get_framerate)

// Styles and Fonts
	ImFont *add_font(const string &str, float size)
	{
		return ImGui::GetIO().Fonts->AddFontFromFileTTF(str.c_str(), size);
	}

	CNI(add_font)

	ImFont *add_font_chinese(const string &str, float size)
	{
		ImFontConfig font_cfg = ImFontConfig();
		font_cfg.OversampleH = font_cfg.OversampleV = 1;
		return ImGui::GetIO().Fonts->AddFontFromFileTTF(str.c_str(), size, &font_cfg,
		        ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
	}

	CNI(add_font_chinese)

	ImFont *add_font_default(float size)
	{
		ImFontConfig font_cfg = ImFontConfig();
		ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "DefaultFont, %.0fpx", (float) size);
		return ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(get_default_font_data(), size,
		        &font_cfg);
	}

	CNI(add_font_default)

	ImFont *add_font_extend(const font &f, float size)
	{
		ImFontConfig font_cfg = ImFontConfig();
		ImFormatString(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "%s, %.0fpx", f.name, (float) size);
		return ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(f.data, size, &font_cfg);
	}

	CNI(add_font_extend)

	ImFont *add_font_extend_cn(const font &f, float size)
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

	float get_font_size()
	{
		return ImGui::GetFontSize();
	}

	CNI(get_font_size)

	void set_font_scale(float scale)
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

	CNI_V(push_item_width, &ImGui::PushItemWidth)

	CNI_V(pop_item_width, &ImGui::PopItemWidth)

	CNI_V(set_next_item_width, &ImGui::SetNextItemWidth)

	float get_item_width()
	{
		return ImGui::GetItemRectSize().x;
	}

	CNI(get_item_width)

	ImVec2 calc_text_size(const string &text)
	{
		const char* txt_beg = text.c_str();
		const char* txt_end = txt_beg + text.size();
		return ImGui::CalcTextSize(txt_beg, txt_end, true);
	}

	CNI(calc_text_size)

	CNI_V(get_window_content_region_width, []()
	{
		return ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
	})

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

	float get_window_pos_x()
	{
		return ImGui::GetWindowPos().x;
	}

	CNI(get_window_pos_x)

	float get_window_pos_y()
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

	void set_window_font_scale(float scale)
	{
		ImGui::SetWindowFontScale(scale);
	}

	CNI(set_window_font_scale)

	float get_window_width()
	{
		return ImGui::GetWindowWidth();
	}

	CNI(get_window_width)

	float get_window_height()
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

	void blank(float wid)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + wid);
	}

	CNI(blank)

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

	bool button_s(const string &str, const ImVec2 &size)
	{
		return ImGui::Button(str.c_str(), size);
	}

	CNI(button_s)

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

	bool image_button(const string &str, image_t &img, const ImVec2 &size)
	{
		return ImGui::ImageButton(str.c_str(), img->get_texture_id(), size);
	}

	CNI(image_button)

	void check_box(const string &str, bool &val)
	{
		ImGui::Checkbox(str.c_str(), &val);
	}

	CNI(check_box)

	void radio_button(const string &str, numeric &v, int v_button)
	{
		int _v = v.as_integer();
		ImGui::RadioButton(str.c_str(), &_v, v_button);
		v = _v;
	}

	CNI(radio_button)

	float plot_value_getter(void *data, int idx)
	{
		return (*reinterpret_cast<const array *>(data))[idx].const_val<numeric>().as_float();
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

	void progress_bar(float fraction, const string &overlay)
	{
		ImGui::ProgressBar(fraction, ImVec2(-1, 0), overlay.c_str());
	}

	CNI(progress_bar)

	void bullet()
	{
		ImGui::Bullet();
	}

	CNI(bullet)

	void combo_box(const string &str, numeric &current, const array &items)
	{
		const char **_items = new const char *[items.size()];
		for (std::size_t i = 0; i < items.size(); ++i)
			_items[i] = items[i].const_val<string>().c_str();
		int _current = current.as_integer();
		ImGui::Combo(str.c_str(), &_current, _items, items.size());
		current = _current;
		delete[] _items;
	}

	CNI(combo_box)

	void drag_float(const string &label, numeric &n)
	{
		float f = n.as_float();
		ImGui::DragFloat(label.c_str(), &f);
		n = f;
	}

	CNI(drag_float)

	void slider_float(const string &str, numeric &n, float min, float max)
	{
		float f = n.as_float();
		ImGui::SliderFloat(str.c_str(), &f, min, max);
		n = f;
	}

	CNI(slider_float)

	void input_text(const string &str, string &text, std::size_t buff_size)
	{
		ImGui::InputText(str.c_str(), &text);
	}

	CNI(input_text)

	void input_text_s(const string &str, string &text, std::size_t buff_size, const array &flags_arr)
	{
		ImGuiInputTextFlags flags = 0;
		for (auto &it : flags_arr)
			flags |= it.const_val<ImGuiInputTextFlags>();
		ImGui::InputText(str.c_str(), &text, flags);
	}

	CNI(input_text_s)

	void input_text_hint(const string &str, const string &hint, string &text, std::size_t buff_size)
	{
		ImGui::InputTextWithHint(str.c_str(), hint.c_str(), &text);
	}

	CNI(input_text_hint)

	void input_text_hint_s(const string &str, const string &hint, string &text, std::size_t buff_size, const array &flags_arr)
	{
		ImGuiInputTextFlags flags = 0;
		for (auto &it : flags_arr)
			flags |= it.const_val<ImGuiInputTextFlags>();
		ImGui::InputTextWithHint(str.c_str(), hint.c_str(), &text, flags);
	}

	CNI(input_text_hint_s)

	void input_text_multiline(const string &str, string &text, std::size_t buff_size)
	{
		ImGui::InputTextMultiline(str.c_str(), &text);
	}

	CNI(input_text_multiline)

	void input_text_multiline_s(const string &str, string &text, std::size_t buff_size, const array &flags_arr)
	{
		ImGuiInputTextFlags flags = 0;
		for (auto &it : flags_arr)
			flags |= it.const_val<ImGuiInputTextFlags>();
		ImGui::InputTextMultiline(str.c_str(), &text, ImVec2(0, 0), flags);
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

	void list_box(const string &str, numeric &current, const array &items)
	{
		const char **_items = new const char *[items.size()];
		for (std::size_t i = 0; i < items.size(); ++i)
			_items[i] = items[i].const_val<string>().c_str();
		int _current = current.as_integer();
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
	void columns(int count, const string &id, bool border)
	{
		ImGui::Columns(count, id.c_str(), border);
	}

	CNI(columns)

	void next_column()
	{
		ImGui::NextColumn();
	}

	CNI(next_column)

	int get_column_index()
	{
		return ImGui::GetColumnIndex();
	}

	CNI(get_column_index)

	float get_column_width(int index)
	{
		return ImGui::GetColumnWidth();
	}

	CNI(get_column_width)

	void set_column_width(int index, float width)
	{
		ImGui::SetColumnWidth(index, width);
	}

	CNI(set_column_width)

	int get_column_offset(int index)
	{
		return ImGui::GetColumnOffset(index);
	}

	CNI(get_column_offset)

	void set_column_offset(int index, int offset)
	{
		ImGui::SetColumnOffset(index, offset);
	}

	CNI(set_column_offset)

	int get_columns_count()
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

	bool is_item_clicked(int button)
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

	bool is_key_down(ImGuiKey key)
	{
		return ImGui::IsKeyDown(key);
	}

	CNI(is_key_down)

	bool is_key_pressed(ImGuiKey key)
	{
		return ImGui::IsKeyPressed(key);
	}

	CNI(is_key_pressed)

	bool is_key_released(ImGuiKey key)
	{
		return ImGui::IsKeyReleased(key);
	}

	CNI(is_key_released)

	bool is_mouse_down(int button)
	{
		return ImGui::IsMouseDown(button);
	}

	CNI(is_mouse_down)

	bool is_any_mouse_down()
	{
		return ImGui::IsAnyMouseDown();
	}

	CNI(is_any_mouse_down)

	bool is_mouse_clicked(int button)
	{
		return ImGui::IsMouseClicked(button);
	}

	CNI(is_mouse_clicked)

	bool is_mouse_double_clicked(int button)
	{
		return ImGui::IsMouseDoubleClicked(button);
	}

	CNI(is_mouse_double_clicked)

	bool is_mouse_released(int button)
	{
		return ImGui::IsMouseReleased(button);
	}

	CNI(is_mouse_released)

	bool is_mouse_dragging(int button)
	{
		return ImGui::IsMouseDragging(button);
	}

	CNI(is_mouse_dragging)

	float get_mouse_pos_x()
	{
		return ImGui::GetMousePos().x;
	}

	CNI(get_mouse_pos_x)

	float get_mouse_pos_y()
	{
		return ImGui::GetMousePos().y;
	}

	CNI(get_mouse_pos_y)

	float get_mouse_drag_delta_x()
	{
		return ImGui::GetMouseDragDelta().x;
	}

	CNI(get_mouse_drag_delta_x)

	float get_mouse_drag_delta_y()
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
	void add_line(const ImVec2 &a, const ImVec2 &b, const ImVec4 &color, float thickness)
	{
		ImGui::GetWindowDrawList()->AddLine(a, b, ImColor(color), thickness);
	}

	CNI(add_line)

	void add_rect(const ImVec2 &a, const ImVec2 &b, const ImVec4 &color, float rounding, float thickness)
	{
		ImGui::GetWindowDrawList()->AddRect(a, b, ImColor(color), rounding, ImDrawFlags_RoundCornersAll,
		                                    thickness);
	}

	CNI(add_rect)

	void add_rect_filled(const ImVec2 &a, const ImVec2 &b, const ImVec4 &color, float rounding)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(a, b, ImColor(color), rounding, ImDrawFlags_RoundCornersAll);
	}

	CNI(add_rect_filled)

	void add_quad(const ImVec2 &a, const ImVec2 &b, const ImVec2 &c, const ImVec2 &d, const ImVec4 &color,
	              float thickness)
	{
		ImGui::GetWindowDrawList()->AddQuad(a, b, c, d, ImColor(color), thickness);
	}

	CNI(add_quad)

	void add_quad_filled(const ImVec2 &a, const ImVec2 &b, const ImVec2 &c, const ImVec2 &d, const ImVec4 &color)
	{
		ImGui::GetWindowDrawList()->AddQuadFilled(a, b, c, d, ImColor(color));
	}

	CNI(add_quad_filled)

	void add_triangle(const ImVec2 &a, const ImVec2 &b, const ImVec2 &c, const ImVec4 &color, float thickness)
	{
		ImGui::GetWindowDrawList()->AddTriangle(a, b, c, ImColor(color), thickness);
	}

	CNI(add_triangle)

	void add_triangle_filled(const ImVec2 &a, const ImVec2 &b, const ImVec2 &c, const ImVec4 &color)
	{
		ImGui::GetWindowDrawList()->AddTriangleFilled(a, b, c, ImColor(color));
	}

	CNI(add_triangle_filled)

	void add_circle(const ImVec2 &centre, float radius, const ImVec4 &color, float seg, float thickness)
	{
		ImGui::GetWindowDrawList()->AddCircle(centre, radius, ImColor(color), seg, thickness);
	}

	CNI(add_circle)

	void add_circle_filled(const ImVec2 &centre, float radius, const ImVec4 &color, float seg)
	{
		ImGui::GetWindowDrawList()->AddCircleFilled(centre, radius, ImColor(color), seg);
	}

	CNI(add_circle_filled)

	void add_text(ImFont *font, float size, const ImVec2 &pos, const ImVec4 &color, const string &text)
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