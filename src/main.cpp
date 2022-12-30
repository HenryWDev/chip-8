// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
// GLFWwindow* window 

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include <src/shader.hpp>

#include "../imgui/imgui.h"
// #include "../imgui/imgui_impl"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
bool show_demo_window = true;
bool show_another_window = false;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}



int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
	// Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
	glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

	// setup dear ImGui context
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
    ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	// Main loop
    while (!glfwWindowShouldClose(window))
    {
		// Poll and handle events (inputs, window resize, etc.)
		glfwPollEvents();

		// Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		// ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

		if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

		ImGui::Begin( "Main" );
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImGui::Text( "(%.1f FPS)", ImGui::GetIO().Framerate );
        const ImVec2 pos = ImGui::GetCursorScreenPos();
		ImGui::End();


		// glUseProgram(programID);
		// glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// glViewport(0,0,1024,768); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		
		// horizontal top
		ImU32 color = ImColor(165,23,69);
        draw_list->AddLine( ImVec2( pos.x, pos.y ), ImVec2( pos.x  *  6.0f, pos.y ), color, 1.0f );

        // horizontal bottom
        draw_list->AddLine( ImVec2( pos.x, pos.y  *  6.0f ), ImVec2( pos.x  *  6.0f, pos.y  *  6.0f ), color, 1.0f );

        // vertical left
        draw_list->AddLine( ImVec2( pos.x, pos.y ), ImVec2( pos.x, pos.y  *  6.0f ), color, 1.0f );

        // vertical right
        draw_list->AddLine( ImVec2( pos.x  *  6.0f, pos.y ), ImVec2( pos.x  *  6.0f, pos.y  *  6.0f ), color, 1.0f );
        draw_list->AddRectFilled( ImVec2( pos.x  *  1.0f, pos.y *  1.0f ), ImVec2( pos.x *  1.5f , pos.y *  1.5f ), color);

		// Rendering
        ImGui::Render();
        int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

	}
	 // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
	return 0;
}

