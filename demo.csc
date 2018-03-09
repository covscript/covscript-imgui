import imgui
var app=imgui.window_application(1280,720,"CovScript ImGUI Demo")
imgui.style_color_dark()
var show_demo_window = true
var show_another_window = false
var clear_color = imgui.vec4(0.45, 0.55, 0.60, 1.00)
var f=0
var counter=0
iostream.setprecision(3)
while !app.is_closed()
    app.prepare()
    imgui.text("Hello, wolrd!")
    imgui.slider_float("float",f,0,1)
    imgui.color_edit3("clear color",clear_color)
    imgui.check_box("Demo Window",show_demo_window)
    imgui.check_box("Another Window",show_another_window)
    if imgui.button("Button")
        ++counter
    end
    imgui.same_line()
    imgui.text("counter = "+counter)
    imgui.text("Application average "+1000/imgui.get_framerate()+" ms/frame ("+imgui.get_framerate()+" FPS)")
    if show_another_window
        imgui.begin_window("Another Window",show_another_window,{})
        imgui.text("Hello from another window!")
        if imgui.button("Close Me")
            show_another_window=false
        end
        imgui.end_window()
    end
    if show_demo_window
        imgui.set_next_window_pos(imgui.vec2(650,20))
        imgui.show_demo_window(show_demo_window)
    end
    app.set_bg_color(clear_color)
    app.render()
end