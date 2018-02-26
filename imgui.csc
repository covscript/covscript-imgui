import imgui
var app=imgui.app(1280,720,"CovScript ImGUI Example")
imgui.add_font("./res/DroidSans.ttf",16)
var show_demo_window = true
var show_another_window = false
var clear_color = imgui.vec4(0.45, 0.55, 0.60, 1.00)
var f=0
var counter=0
while !app.window_should_close()
    app.prepare()
    imgui.text("Hello, wolrd!")
    app.render()
end