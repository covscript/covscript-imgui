@mkdir build
@cd build
@mkdir imports
@gcc -c -I ..\include -static -fPIC -s -O3 ..\src\*.c -o imgui.o
@g++ -std=c++11 -I ..\include -shared -static -fPIC -s -O3 ..\src\*.cpp ..\imgui.cpp .\imgui.o -o .\imports\imgui.cse -lglfw3 -lgdi32 -lopengl32
@del /F /Q imgui.o