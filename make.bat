@mkdir build
@cd build
@mkdir imports
@for /R ..\src\ %%i in (*.c) do @gcc -c -I ..\include -static -fPIC -s -O3 %%i -o %%i.o
@for /R ..\src\ %%i in (*.cpp) do @g++ -c -I ..\include -static -fPIC -s -O3 %%i -o %%i.o
@g++ -std=c++11 -I%CS_DEV_PATH%\include -I..\include -shared -static -fPIC -s -O3 ..\src\*.o ..\imgui.cpp ..\impl\imgui_impl_glfw.cpp ..\impl\imgui_impl_opengl3.cpp -o .\imports\imgui.cse -L%CS_DEV_PATH%\lib -lcovscript -lglfw3 -lgdi32 -lopengl32
@g++ -std=c++11 -I%CS_DEV_PATH%\include -I..\include -shared -static -fPIC -s -O3 -DIMGUI_IMPL_GL2 ..\src\*.o ..\imgui.cpp ..\impl\imgui_impl_glfw.cpp ..\impl\imgui_impl_opengl2.cpp -o .\imports\imgui_gl2.cse -L%CS_DEV_PATH%\lib -lcovscript -lglfw3 -lgdi32 -lopengl32
@del /F /Q ..\src\*.o