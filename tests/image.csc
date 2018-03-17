import imgui
using imgui
system.file.remove("./imgui.ini")
#var app=fullscreen_application(0,"CovScript ImGUI Image")
var app=window_application(0.75*imgui.get_monitor_width(0),0.75*imgui.get_monitor_height(0),"CovScript ImGUI Image")
style_color_dark()
var window_opened=true
var show_confirm=false
var show_about=false
var img=load_bmp_image("./res/covariant_script_wide.bmp")
var progress=0
function menu()
    menu_item("Covariant Script GUI","",false)
    if begin_menu("Tools",true)
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
        if menu_item("Exit","ESC",true)
            system.exit(0)
        end
        end_menu()
    end
    if begin_menu("Help",true)
        if menu_item("About this software","",true)
            show_about=true
        end
        end_menu()
    end
end
while !app.is_closed()
    app.prepare()
    if 100-progress<0.1
        progress=0
    else
        progress+=(100-progress)/100
    end
    if is_key_pressed(get_key_index(keys.escape))
        show_confirm=true
    end
    if begin_main_menu_bar()
        menu()
        end_main_menu_bar()
    end
    if begin_popup_background()
        menu()
        end_popup()
    end
    begin_window("Main",window_opened,{flags.always_auto_resize,flags.no_collapse,flags.no_move,flags.no_title_bar})
        if !window_opened
            break
        end
        set_window_pos(vec2(0,20))
        if begin_popup_window()
            menu()
            end_popup()
        end
        push_id("cs_image")
        image(img,vec2(img.get_width()/2,img.get_height()/2))
        pop_id()
        if begin_popup_item("cs_image")
            menu_item("Covariant Script LOGO","",false)
            end_popup()
        end
        progress_bar(progress/100,"Loading..."+to_integer(progress)+"%")
    end_window()
    if show_about
        var about_opened=true
        begin_window("About",about_opened,{flags.always_auto_resize,flags.no_collapse})
            if !about_opened
                show_about=false
            end
            text("Covariant Script Programming Language")
            bullet()
            text("STD Version:"+runtime.std_version)
            bullet()
            text("Import Path:"+runtime.get_import_path())
            text("Dear ImGui Extension")
            if button("Ok")
                show_about=false
            end
        end_window()
    end
    if show_confirm
        var confirm_window=true
        begin_window("Confirm",confirm_window,{flags.always_auto_resize,flags.no_collapse})
            text("Do you want to exit?")
            spacing()
            if button("Yes") || is_key_pressed(to_integer('Y'))
                system.exit(0)
            end
            same_line()
            if button("No") || !confirm_window
                show_confirm=false
            end
        end_window()
    end
    app.render()
end