import imgui
using imgui
system.file.remove("./imgui.ini")
#var app=fullscreen_application(0,"CovScript ImGUI Sandbox")
var app=window_application(0.75*imgui.get_monitor_width(0),0.75*imgui.get_monitor_height(0),"CovScript ImGUI Sandbox")
style_color_light()
var window_opened=true
var col=vec4(0.5,0.5,0.5,1)
var radius=10
const var gravity=10
const var air_drop=1
const var delta_time=0.1
const var mass=0.5
const var size=0.01
var speed_x=50
var speed_y=0
var posit_x=radius
var posit_y=radius+20
function run()
	var ax=0
	var ay=0
	if(speed_x>0)
		ax=ax-(air_drop*size*speed_x)/mass
	end
	if(speed_y>0)
		ay=(mass*gravity-air_drop*size*speed_y)/mass
	end
	if(speed_y==0)
		ay=(mass*gravity)/mass
	end
	if(speed_y<0)
		ay=(mass*gravity+math.abs(air_drop*size*speed_y))/mass
	end
	posit_x=posit_x+speed_x*delta_time+0.5*ax*delta_time^2
	posit_y=posit_y+speed_y*delta_time+0.5*ay*delta_time^2
	speed_x=speed_x+ax*delta_time
	speed_y=speed_y+ay*delta_time
end
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
            text("X: "+posit_x)
            text("Y: "+posit_y)
            tree_pop()
        end
        if posit_x<=radius
            speed_x=math.abs(speed_x)
        end
        if posit_x>=app.get_window_width()-radius
            speed_x=-math.abs(speed_x)
        end
        if posit_y<=radius+20
            speed_y=math.abs(speed_y)
        end
        if posit_y>=app.get_window_height()-radius
            speed_y=-math.abs(speed_y)
        end
        run()
        add_circle_filled(vec2(posit_x,posit_y),radius,col,360)
    end_window()
    app.render()
end