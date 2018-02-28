import imgui
using imgui
#var app=fullscreen_application(0,"CovScript ImGUI Tests")
var app=window_application(0.75*imgui.get_monitor_width(0),0.75*imgui.get_monitor_height(0),"CovScript ImGUI Tests")
add_font("./res/DroidSans.ttf",16)
var window_opened=true
var progress=0
var radio_choice=0
var combo_choice=1
while !app.is_closed()
    app.prepare()
    begin_window("Test",window_opened)
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
    combo_box("Style Choice",combo_choice,"Classic\0Light\0Dark\0")
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
    end_window()
    app.render()
end