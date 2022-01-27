import imgui
import imgui_font
using imgui
system.file.remove("./imgui.ini")
var app=window_application(0.75*imgui.get_monitor_width(0),0.75*imgui.get_monitor_height(0),"CovScript ImGUI测试程序")
var font=add_font_extend_cn(imgui_font.source_han_sans, 32)
set_font_scale(0.5)
var window_opened=true
var progress=0
var radio_choice=0
var combo_choice=1
var texts=new string
while !app.is_closed()
    app.prepare()
    push_font(font)
    begin_window("测试窗口",window_opened,{flags.always_auto_resize})
        if !window_opened
            break
        end
        slider_float("进度条",progress,0,1)
        progress_bar(progress,"进度")
        separator()
        plot_lines("折线图","",{1,3,2,3,1})
        separator()
        radio_button("A",radio_choice,0)
        same_line()
        radio_button("B",radio_choice,1)
        same_line()
        radio_button("C",radio_choice,2)
        switch radio_choice
            case 0
                text("你选择了A")
            end
            case 1
                text("你选择了B")
            end
            case 2
                text("你选择了C")
            end
        end
        separator()
        combo_box("选择主题",combo_choice,{"经典","亮色","暗色"})
        switch combo_choice
            case 0
                style_color_classic()
            end
            case 1
                style_color_light()
            end
            case 2
                style_color_dark()
            end
        end
        separator()
        input_text_multiline_s("##input",texts,512,{flags.password, flags.allow_tab})
        var pos={get_window_pos_x(),get_window_pos_y()}
        var size={get_window_width(),get_window_height()}
    end_window()
    begin_window("文字窗口",window_opened,{flags.always_auto_resize,flags.no_title_bar,flags.no_move})
        set_window_pos(vec2(pos[0]+size[0]+10,pos[1]+size[1]+10))
        set_window_size(vec2(0,0))
        text("主窗口中输入的文字：")
        separator()
        text(texts)
    end_window()
    pop_font()
    window_opened=true
    app.render()
end