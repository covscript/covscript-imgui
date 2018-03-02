import imgui
using imgui
system.file.remove("./imgui.ini")
#var app=fullscreen_application(0,"CovScript ImGUI Image")
var app=window_application(0.75*imgui.get_monitor_width(0),0.75*imgui.get_monitor_height(0),"CovScript ImGUI Image")
add_font("./res/DroidSans.ttf",16)
style_color_dark()
var window_opened=true
var img=load_bmp_image("./res/covariant_script_wide.bmp")
var progress=0
while !app.is_closed()
    app.prepare()
    if 100-progress<0.1
        progress=0
    else
        progress+=(100-progress)/100
    end
    set_next_window_pos(vec2(0,0))
    begin_window("Welcome using CovScript",window_opened)
        if !window_opened
            break
        end
        image(img,vec2(img.get_width()/2,img.get_height()/2))
        progress_bar(progress/100,"Loading..."+to_integer(progress)+"%")
    end_window()
    app.render()
end