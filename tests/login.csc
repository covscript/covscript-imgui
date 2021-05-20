import imgui
import imgui_font
using imgui
system.file.remove("./imgui.ini")
var app = window_application(0.75*imgui.get_monitor_width(0), 0.75*imgui.get_monitor_height(0), "CovScript ImGUI测试程序")
var font = add_font_extend_cn(imgui_font.source_han_sans, 16)
var img = load_bmp_image("./res/covariant_script_wide.bmp")
var window_opened = true
var username = new string
var password = new string
var title = "欢迎登陆XXXX系统"
var btn_txt = "登录"
var geometry = vec2(300, 300)
var text_size = 0
var padding = 10
while !app.is_closed()
	app.prepare()
	push_font(font)
	begin_window("登录", window_opened, {flags.no_resize, flags.no_collapse})
	if !window_opened
		break
	end
    style_color_light()
    set_window_size(geometry)
    var region_width = get_window_content_region_width()
    var align_width = region_width - padding
    blank((region_width - align_width)/2)
    image(img, vec2(align_width, (align_width/img.get_width())*img.get_height()))
    text_size = calc_text_size(title)
    blank((region_width - text_size.x)/2)
    text(title)
    push_item_width(align_width)
    blank((region_width - align_width)/2)
	input_text_hint("##TXT1", "用户名", username, 512)
	blank((region_width - align_width)/2)
    input_text_hint_s("##TXT2", "密码", password, 512, {flags.password})
    blank((region_width - align_width)/2)
    slider_float("##PADDING", padding, 0, 100)
    pop_item_width()
    text_size = calc_text_size(btn_txt)
    blank((region_width - align_width)/2)
    if button_s(btn_txt, vec2(align_width, text_size.y + 10)) || is_key_pressed(get_key_index(keys.enter))
        title = "欢迎，" + username
    end
	end_window()
	pop_font()
	app.render()
end
