#include "platform/rlOpenXRAndroidWrapper.h"

#define XR_USE_PLATFORM_ANDROID
#define XR_USE_GRAPHICS_API_OPENGL_ES
#define XR_USE_TIMESPEC
#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"
#include <ctime>
#include <iostream>
//#include <windows.h>

#include <cassert>

//static_assert(STRICT == 1, "rlOpenXR only supports Windows in STRICT mode, check that NO_STRICT is not defined. This is required as we forward declare HDC and HGLRC.");

#ifdef __cplusplus
extern "C" {
#endif

//HDC wrapped_wglGetCurrentDC()
//{
//	return wglGetCurrentDC();
//}
//
//HGLRC wrapped_wglGetCurrentContext()
//{
//	return wglGetCurrentContext();
//}
//
//BOOL wrapped_wglMakeCurrent(HDC hDC, HGLRC hGLRC)
//{
//	return wglMakeCurrent(hDC, hGLRC);
//}

XrTime wrapped_XrTimeFromQueryPerformanceCounter(XrInstance instance, void* xrConvertTimespecTimeToTimeKHR_funcptr)
{
    timespec time;
    const int error = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time);
    if (error) {
        perror("clock_gettime error: ");
        assert(false);
    }

    auto xrConvertTimespecTimeToTimeKHR = (PFN_xrConvertTimespecTimeToTimeKHR)xrConvertTimespecTimeToTimeKHR_funcptr;
    XrTime time_xr = 0;
    XrResult result_xr = xrConvertTimespecTimeToTimeKHR(instance, &time, &time_xr);
    assert(XR_SUCCEEDED(result_xr));

    return time_xr;
//	LARGE_INTEGER time_win32{};
//	const BOOL success_win32 = QueryPerformanceCounter(&time_win32);
//	assert(success_win32);
//
//	auto xrConvertWin32PerformanceCounterToTimeKHR = (PFN_xrConvertWin32PerformanceCounterToTimeKHR)xrConvertWin32PerformanceCounterToTimeKHR_funcptr;
//	XrTime time_xr = 0;
//	XrResult result_xr = xrConvertWin32PerformanceCounterToTimeKHR(instance, &time_win32, &time_xr);
//	assert(XR_SUCCEEDED(result_xr));
//
//	return time_xr;
}

#define DEBUG_BREAK /* TODO */

void GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    std::cout << "OpenGL Debug message (" << id << "): " << message << std::endl;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            std::cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cout << "Source: Other";
            break;
    }
    std::cout << std::endl;

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cout << "Type: Other";
            break;
    }
    std::cout << std::endl;

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "Severity: high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "Severity: medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "Severity: low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "Severity: notification";
            break;
    }
    std::cout << std::endl;
    std::cout << std::endl;

    if (type == GL_DEBUG_TYPE_ERROR)
        DEBUG_BREAK;
}

void wrapped_initializeKsGpuWindow(ksGpuWindow *window) {
    ksDriverInstance driverInstance{};
    ksGpuQueueInfo queueInfo{};
    ksGpuSurfaceColorFormat colorFormat{KS_GPU_SURFACE_COLOR_FORMAT_B8G8R8A8};
    ksGpuSurfaceDepthFormat depthFormat{KS_GPU_SURFACE_DEPTH_FORMAT_D24};
    ksGpuSampleCount sampleCount{KS_GPU_SAMPLE_COUNT_1};
    if (!ksGpuWindow_Create(window, &driverInstance, &queueInfo, 0, colorFormat, depthFormat, sampleCount, 640, 480, false)) {
        std::cout << "ERROR: OPENGL ES: Failed to create Context." << std::endl;
    }

    GLint glMajorVersion = 0;
    GLint glMinorVersion = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLDebugCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
    glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, nullptr, GL_TRUE);

}

void wrapped_destroyKsGpuWindow(ksGpuWindow *window) {
    ksGpuWindow_Destroy(window);
}

void wrapped_populateGraphicsBinding(void *binding_ptr) {
    auto &binding = *static_cast<XrGraphicsBindingOpenGLESAndroidKHR*>(binding_ptr);

    auto context = eglGetCurrentContext();
    auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint configId;
    eglQueryContext(display, context, EGL_CONFIG_ID, &configId);
    EGLint attribList[] = {EGL_CONFIG_ID, configId, EGL_NONE};
    EGLConfig *configs;
    EGLint numConfigs;
    eglChooseConfig(display, attribList, configs, 1, &numConfigs);

    binding.type = XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR;
    binding.next = nullptr;
    binding.display = display;
    binding.config = configs[0];
    binding.context = context;
}

#ifdef __cplusplus
}
#endif