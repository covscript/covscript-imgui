import imgui
using imgui
system.file.remove("./imgui.ini")
var app=window_application(0.75*imgui.get_monitor_width(0),0.75*imgui.get_monitor_height(0),"CovScript ImGUI Tests")
var window_opened=true
var window_resize=true
var combo_choice=1
@begin
constant default_text=
"Licensed under the Apache License, Version 2.0 (the \"License\");\n"+
"you may not use this file except in compliance with the License.\n"+
"You may obtain a copy of the License at\n"+
"\n"+
"http://www.apache.org/licenses/LICENSE-2.0\n"+
"\n"+
"Unless required by applicable law or agreed to in writing, software\n"+
"distributed under the License is distributed on an \"AS IS\" BASIS,\n"+
"WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"+
"See the License for the specific language governing permissions and\n"+
"limitations under the License.\n"
@end
struct tab_item
    var id=new string
    var title=new string
    var state=true
    var flags={}
    var text=new string
end
var tab_list={}
var id=0
var title="New tab"
var unsaved=false
var txt=default_text
while !app.is_closed()
    app.prepare()
    begin_window("Test Tab Bar", window_opened, {flags.no_resize, flags.no_title_bar, flags.no_move})
        if !window_opened
            break
        end
        set_window_pos(vec2(0,0))
        set_window_size(vec2(app.get_window_width(),app.get_window_height()))
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
        text("New tab item")
        input_text("Tab title", title, 16)
        check_box("Unsaved", unsaved)
        input_text_multiline("Text", txt, 10000)
        if button("Insert")
            var tab=new tab_item
            tab.id="TAB"+to_string(++id)
            tab.title=title
            foreach it in tab_list
                if title==it.title
                    tab.title+=tab_list.size
                end
            end
            if unsaved
                tab.flags={flags.unsaved_document}
            end
            tab.text=txt
            tab_list.push_back(tab)
        end
        begin_tab_bar("Tab Bar")
            foreach it in tab_list
                push_id(it.id)
                if begin_tab_item(it.title, it.state, it.flags)
                    text(it.text)
                    end_tab_item()
                end
                pop_id()
            end
        end_tab_bar()
    end_window()
    app.render()
end