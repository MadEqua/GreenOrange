#include "GuiRoot.h"

#include <stdio.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "../Constants.h"


GuiRoot::GuiRoot(GreenOrange &greenOrange, GLFWwindow &glfwWindow) :
    greenOrange(greenOrange),
    glfwWindow(glfwWindow){
}

bool GuiRoot::init() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDockingWithShift = false;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(&glfwWindow, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("DroidSans.ttf", 16.0f);

    return true;
}

void GuiRoot::deinit() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiRoot::drawFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    drawGui();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiRoot::drawGui() {

    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("RootWindow", 0, window_flags);
    {
        ImGui::PopStyleVar(3);

        ImGuiID dockspace_id = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        float menuBarHeight;
        if(ImGui::BeginMainMenuBar()) {
            if(ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Open..", "Ctrl+O")) {
                    greenOrange.openProject("TODO");
                }
                if(ImGui::MenuItem("Save", "Ctrl+S")) {
                }
                if(ImGui::MenuItem("Close", "Ctrl+W")) {
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("About")) {
                ImGui::EndMenu();
            }

#ifndef GO_DIST
            static bool showDemoWindow = false;
            if(ImGui::BeginMenu("ImGui Demo")) {
                showDemoWindow = !showDemoWindow;
                ImGui::EndMenu();
            }
            if(showDemoWindow)
                ImGui::ShowDemoWindow(&showDemoWindow);
#endif
            
            menuBarHeight = ImGui::GetWindowHeight();
            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("GreenOrange", 0, ImGuiWindowFlags_AlwaysAutoResize);
        {
            if(!greenOrange.hasCurrentProject()) {
                ImGui::Text("No project open.");
                if(ImGui::Button("Open")) {
                    greenOrange.openProject("TODO");
                }
            }
            else {
                projectPanel.drawGui(*greenOrange.getCurrentProject());
                scenePanel.drawGui(greenOrange.getCurrentProject()->getSelectedScene());
                previewPanel.drawGui(*greenOrange.getCurrentProject());
                inspectorPanel.drawGui(greenOrange.getCurrentProject()->getSelectedScene().getSelectedEntity());
            }
        }
        ImGui::End();
    }
    ImGui::End();
}