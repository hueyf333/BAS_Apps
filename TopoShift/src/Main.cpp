#include "Application.h"
#include "UI/MainUI.h"
#include "UI/LandmarksPanel.h"
#include "UI/AlignmentPanel.h"
#include <Windows.h>

// Note: In a complete implementation, ImGui would be included and initialized here
// #include "imgui.h"
// #include "imgui_impl_win32.h"
// #include "imgui_impl_dx11.h"

using namespace TopoShift;

// Global application instance
static Application* g_app = nullptr;
static LandmarksPanel* g_landmarksPanel = nullptr;
static AlignmentPanel* g_alignmentPanel = nullptr;

// Forward declare message handler from imgui_impl_win32.cpp
// extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    //     return true;

    switch (message) {
    case WM_SIZE:
        if (g_app && wParam != SIZE_MINIMIZED) {
            g_app->resize(LOWORD(lParam), HIWORD(lParam));
        }
        return 0;

    case WM_MOUSEMOVE:
        if (g_app) {
            g_app->handleMouseMove(LOWORD(lParam), HIWORD(lParam));
        }
        return 0;

    case WM_LBUTTONDOWN:
        if (g_app) {
            g_app->handleMouseButton(0, true, LOWORD(lParam), HIWORD(lParam));
        }
        return 0;

    case WM_LBUTTONUP:
        if (g_app) {
            g_app->handleMouseButton(0, false, LOWORD(lParam), HIWORD(lParam));
        }
        return 0;

    case WM_MBUTTONDOWN:
        if (g_app) {
            g_app->handleMouseButton(1, true, LOWORD(lParam), HIWORD(lParam));
        }
        return 0;

    case WM_MBUTTONUP:
        if (g_app) {
            g_app->handleMouseButton(1, false, LOWORD(lParam), HIWORD(lParam));
        }
        return 0;

    case WM_RBUTTONDOWN:
        if (g_app) {
            g_app->handleMouseButton(2, true, LOWORD(lParam), HIWORD(lParam));
        }
        return 0;

    case WM_RBUTTONUP:
        if (g_app) {
            g_app->handleMouseButton(2, false, LOWORD(lParam), HIWORD(lParam));
        }
        return 0;

    case WM_MOUSEWHEEL:
        if (g_app) {
            float delta = GET_WHEEL_DELTA_WPARAM(wParam) / 120.0f;
            g_app->handleMouseWheel(delta);
        }
        return 0;

    case WM_KEYDOWN:
        if (g_app) {
            g_app->handleKeyboard(static_cast<int>(wParam), true);
        }
        return 0;

    case WM_KEYUP:
        if (g_app) {
            g_app->handleKeyboard(static_cast<int>(wParam), false);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

// WinMain entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"TopoShiftWindowClass";

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create window
    HWND hwnd = CreateWindowEx(
        0,
        L"TopoShiftWindowClass",
        L"TopoShift - Topology Transfer Tool",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Initialize application
    g_app = new Application();
    if (!g_app->initialize(hwnd, 1280, 720)) {
        MessageBox(NULL, L"Application Initialization Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        delete g_app;
        return 0;
    }

    // Initialize ImGui (stub - needs actual ImGui setup)
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO(); (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // ImGui::StyleColorsDark();
    // ImGui_ImplWin32_Init(hwnd);
    // ImGui_ImplDX11_Init(g_app->getRenderer().getDevice(), g_app->getRenderer().getContext());

    // Initialize UI panels
    g_landmarksPanel = new LandmarksPanel();
    g_alignmentPanel = new AlignmentPanel();

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Main message loop
    MSG msg = {};
    LARGE_INTEGER frequency, lastTime, currentTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            // Calculate delta time
            QueryPerformanceCounter(&currentTime);
            float deltaTime = static_cast<float>(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
            lastTime = currentTime;

            // Update
            g_app->update(deltaTime);

            // Start ImGui frame (stub)
            // ImGui_ImplDX11_NewFrame();
            // ImGui_ImplWin32_NewFrame();
            // ImGui::NewFrame();

            // Render UI
            getMainUI().render(*g_app);
            g_landmarksPanel->render(*g_app);
            g_alignmentPanel->render(*g_app);

            // Render (stub for ImGui)
            // ImGui::Render();

            // Render 3D
            g_app->render();

            // Draw ImGui (stub)
            // ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }
    }

    // Cleanup
    // ImGui_ImplDX11_Shutdown();
    // ImGui_ImplWin32_Shutdown();
    // ImGui::DestroyContext();

    delete g_alignmentPanel;
    delete g_landmarksPanel;
    delete g_app;

    return static_cast<int>(msg.wParam);
}
