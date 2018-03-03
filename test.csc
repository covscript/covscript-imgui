import imgui
using imgui
system.file.remove("./imgui.ini")
#var app=fullscreen_application(0,"CovScript ImGUI Tests")
var app=window_application(0.75*imgui.get_monitor_width(0),0.75*imgui.get_monitor_height(0),"CovScript ImGUI Tests")
add_font("./res/DroidSans.ttf",16)
var window_opened=true
var progress=0
var radio_choice=0
var combo_choice=1
var texts=new string
while !app.is_closed()
    app.prepare()
    begin_window("Test",window_opened,{flags.always_auto_resize})
        if !window_opened
            break
        end
        slider_float("Progress",progress,0,1)
        progress_bar(progress,"Progress")
        separator()
        radio_button("A",radio_choice,0)
        same_line()
        radio_button("B",radio_choice,1)
        same_line()
        radio_button("C",radio_choice,2)
        switch radio_choice
            case 0
                text("Your choice is A")
            end
            case 1
                text("Your choice is B")
            end
            case 2
                text("Your choice is C")
            end
        end
        separator()
        combo_box("Style Choice",combo_choice,{"Classic","Light","Dark"})
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
        input_text_multiline("",texts,512)
        var pos={get_window_pos_x(),get_window_pos_y()}
        var size={get_window_width(),get_window_height()}
    end_window()
    begin_window("Text",window_opened,{flags.always_auto_resize,flags.no_title_bar,flags.no_move})
        set_window_pos(vec2(pos[0]+size[0]+10,pos[1]+size[1]+10))
        set_window_size(vec2(0,0))
        text("Texts from main window:")
        separator()
        text(texts)
    end_window()
    window_opened=true
    app.render()
end