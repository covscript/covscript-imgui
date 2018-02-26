@mkdir build
@cd build
@mkdir imports
@g++ -std=c++11 -I ..\include -shared -static -fPIC -s -O3 ..\src\*.cpp ..\imgui.cpp -o .\imports\imgui.cse -lglfw3 -lgdi32 -lopengl32