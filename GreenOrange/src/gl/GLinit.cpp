#include "GLinit.h"

#include <stdio.h>

#include "../Constants.h"


GLFWwindow *glfwWindow;

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

    printf("OpenGL Debug - Id: 0x%04X. Source: %s, Type: %s, Severity: %s. Message: %s\n", id, sourceString, typeString, severityString, message);
}

bool initGlfw() {
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

void deinitGlfw() {
    glfwDestroyWindow(glfwWindow);
    glfwWindow = nullptr;
    glfwTerminate();
}

bool initGlad() {
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

        //FIXME: This should be glDisable. But on nVidia drivers the return value is always GL_LINEAR.
        //https://devtalk.nvidia.com/default/topic/776591/opengl/gl_framebuffer_srgb-functions-incorrectly/
        //Not very bad because every system should return a srgb framebuffer anyway.
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

#ifndef GO_DIST
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openglCallbackFunction, nullptr);

    //Filter Notification messages. Disabling notifications, avoiding spam...
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#else
    glDisable(GL_DEBUG_OUTPUT);
#endif

    return true;
}
