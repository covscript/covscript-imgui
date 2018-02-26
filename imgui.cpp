/*
* Covariant Script ImGUI Extension
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/

// Covariant Script Header
#include <covscript/extension.hpp>
#include <covscript/cni.hpp>

// ImGUI Common Header
#include <imgui/imgui.h>

// ImGUI GLFW3 Implement
#include <imgui/glfw_gl2.h>
#include <GLFW/glfw3.h>

// ImGUI Wrapper
#include <cstdio>
namespace imgui_cs {
    class application final{
        static void error_callback(int error, const char* description)
        {
            throw cs::lang_error(description);
        }
        GLFWwindow* window=nullptr;
    public:
        application()=delete;
        application(const application&)=delete;
        application(application&&) noexcept=delete;
        application(std::size_t width,std::size_t height,const std::string& title){
            glfwSetErrorCallback(error_callback);
            if (!glfwInit())
                throw cs::lang_error("Init OpenGL Error.");
            window = glfwCreateWindow(width,height,title.c_str(), NULL, NULL);
            glfwMakeContextCurrent(window);
            glfwSwapInterval(true);
            ImGui::CreateContext();
            ImGui_ImplGlfwGL2_Init(window, true);
            ImGui::GetIO().NavFlags |= ImGuiNavFlags_EnableKeyboard;
        }
        ~application(){
            ImGui_ImplGlfwGL2_Shutdown();
            ImGui::DestroyContext();
            glfwTerminate();
        }
        bool window_should_close()
        {
            return !glfwWindowShouldClose(window);
        }
        void prepare()
        {
            glfwPollEvents();
            ImGui_ImplGlfwGL2_NewFrame();
        }
        void render()
        {
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui::Render();
            ImGui_ImplGlfwGL2_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }
    };
}