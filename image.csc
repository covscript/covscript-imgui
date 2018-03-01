import imgui
using imgui
system.file.remove("./imgui.ini")
#var app=fullscreen_application(0,"CovScript ImGUI Image")
var app=window_application(0.75*imgui.get_monitor_width(0),0.75*imgui.get_monitor_height(0),"CovScript ImGUI Image")
var window_opened=true
var img=load_bmp_image("./res/covariant_script_wide.bmp")
while !app.is_closed()
    app.prepare()
    begin_window("CovScript",window_opened)
        text(to_string(img.get_width()))
        image(img,vec2(img.get_width(),img.get_height()))
    end_window()
    app.render()
end