#include "GuiRoot.h"

#include <stdio.h>

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "../Constants.h"


static void glfwErrorCallback(int error, const char* description) {
    printf("GLFW Error %d: %s\n", error, description);
}

static void GLAPIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity,
                                              GLsizei length, const GLchar *message, const void *userParam) {
    const char* sourceString;
    switch(source) {
    case GL_DEBUG_SOURCE_API:
        sourceString = "API";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        sourceString = "Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        sourceString = "Window System";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        sourceString = "Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        sourceString = "Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        sourceString = "Other";
        break;
    default:
        sourceString = "Unknown";
    }

    const char* typeString;
    switch(type) {
    case GL_DEBUG_TYPE_ERROR:
        typeString = "Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typeString = "Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typeString = "Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        typeString = "Performance";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        typeString = "Portablility";
        break;
    case GL_DEBUG_TYPE_MARKER:
        typeString = "Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        typeString = "Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        typeString = "Pop Group";
        break;
    case GL_DEBUG_TYPE_OTHER:
        typeString = "Other";
        break;
    default:
        typeString = "Unknown";
    }

    const char* severityString;
    switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        severityString = "High";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        severityString = "Medium";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        severityString = "Low";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severityString = "Notification";
        break;
    default:
        severityString = "Unknown";
    }

    printf("OpenGL Debug - Id: %d. Source: %s, Type: %s, Severity: %s.\n", id, sourceString, typeString, severityString);
}


GuiRoot::GuiRoot(GreenOrange &greenOrange) :
    greenOrange(greenOrange) {
}

bool GuiRoot::init() {
    return initGlfw() && initGlad() && initImGui();
}

void GuiRoot::deinit() {
    deinitImGui();
    deinitGlfw();
}

bool GuiRoot::initGlfw() {
    glfwSetErrorCallback(glfwErrorCallback);
    if(!glfwInit())
        return false;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
#ifndef GO_DIST
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

    glfwWindow = glfwCreateWindow(1280, 800, "GreenOrange", nullptr, nullptr);
    if(glfwWindow == nullptr) {
        printf("GLFW error creating window.\n");
        return false;
    }

    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1); //Enable vsync

    return true;
}

void GuiRoot::deinitGlfw() {
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

bool GuiRoot::initGlad() {
    if(!gladLoadGL()) {
        printf("Failed to initialize glad!\n");
        return false;
    }

    GLint majorV, minorV;
    glGetIntegerv(GL_MAJOR_VERSION, &majorV);
    glGetIntegerv(GL_MINOR_VERSION, &minorV);
    const GLubyte *versionString = glGetString(GL_VERSION);
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("OpenGL version: %d.%d. Version string: %s.\n", majorV, minorV, versionString);
    printf("Vendor: %s.\n", vendor);
    printf("Renderer: %s.\n", renderer);
    printf("GLSL Version: : %s.\n", glslVersion);

    GLint res;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING, &res);

    if(res == GL_LINEAR) {
        printf("Linear RGB Default Framebuffer (This reporting may be wrong on nVidia drivers).\n");

        //TODO This should be glDisable. But on nVidia drivers the return value is always GL_LINEAR.
        //https://devtalk.nvidia.com/default/topic/776591/opengl/gl_framebuffer_srgb-functions-incorrectly/
        glEnable(GL_FRAMEBUFFER_SRGB);
    }
    else if(res == GL_SRGB) {
        printf("sRGB Default Framebuffer.\n");
        //enable auto Linear RGB to sRGB conversion when writing to sRGB framebuffers
        glEnable(GL_FRAMEBUFFER_SRGB);
    }

    glGetIntegerv(GL_SAMPLES, &res);
    if(res > 0) {
        printf("Multisampled Default Framebuffer. Samples: %d.\n", res);
        glEnable(GL_MULTISAMPLE);
    }
    else {
        printf("Non-Multisampled Default Framebuffer.\n");
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

#ifndef GO_DIST
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openglCallbackFunction, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE); //Filter Notification messages
#else
    glDisable(GL_DEBUG_OUTPUT);
#endif

    return true;
}

bool GuiRoot::initImGui() {
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
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    return true;
}

void GuiRoot::deinitImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiRoot::run() {
    while(!glfwWindowShouldClose(glfwWindow)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        glfwMakeContextCurrent(glfwWindow);
        int width, height;
        glfwGetFramebufferSize(glfwWindow, &width, &height);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        drawGui(width, height);

        // Rendering
        ImGui::Render();
        glViewport(0, 0, width, height);
        /*const ImVec4 WINDOW_CLEAR_COLOR = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glClearColor(WINDOW_CLEAR_COLOR.x, WINDOW_CLEAR_COLOR.y, WINDOW_CLEAR_COLOR.z, WINDOW_CLEAR_COLOR.w);
        glClear(GL_COLOR_BUFFER_BIT);*/
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(glfwWindow);
        glfwSwapBuffers(glfwWindow);
    }
}

void GuiRoot::drawGui(int windowWidth, int windowHeight) {

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
                    greenOrange.openProject();
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
            menuBarHeight = ImGui::GetWindowHeight();
            ImGui::EndMainMenuBar();
        }
        ImGui::Begin("GreenOrange", 0, ImGuiWindowFlags_AlwaysAutoResize);
        {
            if(!greenOrange.hasCurrentProject()) {
                ImGui::Text("No project open.");
                if(ImGui::Button("Open")) {
                    greenOrange.openProject();
                }
            }
            else {
                projectPanel.drawGui(*greenOrange.getCurrentProject(), guiSharedData.selectedSceneIdx);
                scenePanel.drawGui(greenOrange.getCurrentProject()->getScene(guiSharedData.selectedSceneIdx));
                inspectorPanel.drawGui();
            }

            bool showDemoWindow = true;
            if(showDemoWindow)
                ImGui::ShowDemoWindow(&showDemoWindow);
        }
        ImGui::End();
    }
    ImGui::End();
}