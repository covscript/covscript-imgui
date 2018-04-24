import imgui
using imgui
system.file.remove("./imgui.ini")
#var app=fullscreen_application(0,"CovScript ImGUI Canvas")
var app=window_application(0.75*imgui.get_monitor_width(0),0.75*imgui.get_monitor_height(0),"CovScript ImGUI Canvas")
style_color_light()
var window_opened=true
var col=vec4(0.5,0.5,0.5,1)
var radius=50
var thickness=5
var delta=10
var pos_x=radius+thickness
var pos_y=radius+thickness+20
var x_add=true
var y_add=true
while !app.is_closed()
    app.prepare()
    begin_window("Main",window_opened,{flags.menu_bar,flags.no_collapse,flags.no_title_bar,flags.no_move,flags.no_resize})
        if !window_opened
            break
        end
        set_window_pos(vec2(0,0))
        set_window_size(vec2(app.get_window_width(),app.get_window_height()))
        if begin_menu_bar()
            if begin_menu("Styles",true)
                if menu_item("Classic","",true)
                    style_color_classic()
                end
                if menu_item("Light","",true)
                    style_color_light()
                end
                if menu_item("Dark","",true)
                    style_color_dark()
                end
                end_menu()
            end
            if menu_item("Exit","",true)
                system.exit(0)
            end
            end_menu_bar()
        end
        if tree_node("Property")
            color_edit4("Circle Color",col)
            slider_float("Radius",radius,0,100)
            slider_float("Thickness",thickness,0,100)
            slider_float("Speed",delta,0,100)
            delta=to_integer(delta)
            if arrow_button("x_arrow",x_add?dirs.right:dirs.left)
                x_add=!x_add
            end
            same_line()
            text("X: "+pos_x)
            if arrow_button("y_arrow",y_add?dirs.down:dirs.up)
                y_add=!y_add
            end
            same_line()
            text("Y: "+pos_y)
            tree_pop()
        end
        if is_mouse_clicked(1)
            pos_x=get_mouse_pos_x()
            pos_y=get_mouse_pos_y()
        end
        if x_add
            pos_x+=delta
        else
            pos_x-=delta
        end
        if y_add
            pos_y+=delta
        else
            pos_y-=delta
        end
        if pos_x<=radius+thickness
            x_add=true
        end
        if pos_x>=app.get_window_width()-radius-thickness
            x_add=false
        end
        if pos_y<=radius+thickness+20
            y_add=true
        end
        if pos_y>=app.get_window_height()-radius-thickness
            y_add=false
        end
        add_circle(vec2(pos_x,pos_y),radius,col,360,thickness)
    end_window()
    app.render()
end
