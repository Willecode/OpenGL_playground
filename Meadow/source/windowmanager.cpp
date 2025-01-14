#include "windowmanager.h"
#include "service_locator/loggerlocator.h"
#include "input/inputevents.h"

namespace WindowConf {
    const float DEFAULT_SCR_WIDTH = 1920.0f;
    const float DEFAULT_SCR_HEIGHT = 1080.0f;
}
float WindowManager::width = WindowConf::DEFAULT_SCR_WIDTH;
float WindowManager::height = WindowConf::DEFAULT_SCR_HEIGHT;
WindowManager::WindowManager(): m_window(nullptr), m_borderless(true),
m_maxWidth(WindowConf::DEFAULT_SCR_WIDTH), m_maxHeight(WindowConf::DEFAULT_SCR_HEIGHT)
{
    InputEvents::WindowBordersToggleEvent::subscribe(std::bind(&WindowManager::toggleWindowBorders, this));
}

WindowManager& WindowManager::getInstance()
{
    static WindowManager instance;
    return instance;
}

WindowManager::~WindowManager()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool WindowManager::createWindow(std::string title)
{
    // glfw: initialize and configure
// ------------------------------
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) {
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
   // --------------------

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    m_maxHeight = mode->height;
    m_maxWidth = mode->width;
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    if (m_borderless)
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // borderless window

    m_window = glfwCreateWindow(m_maxWidth, m_maxHeight, "Meadow", NULL, NULL);
    if (m_window == NULL)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    /*
    * Set cursor mode to unlocked
    */
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


    /*
    * Window resize callback
    */
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);

    glfwSwapInterval(0); // vsync off

    /*
    * Subscribe to windowclose event
    */
    std::function<void()> f = std::bind(&WindowManager::closeWindowEventHandler, this);
    //disp->subscribe(CloseWindowEvent::EVENT_TYPE, f);
    InputEvents::CloseWindowEvent::subscribe(f);


    /*
    * Subscribe to mouse lock/unlock event
    */
    std::function<void()> fml = std::bind(&WindowManager::MouseLockHandler, this);
    std::function<void()> fmu = std::bind(&WindowManager::MouseUnlockHandler, this);
    /*disp->subscribe(MouseLockEvent::EVENT_TYPE, fml);
    disp->subscribe(MouseUnlockEvent::EVENT_TYPE, fml);*/
    InputEvents::MouseLockEvent::subscribe(fml);
    InputEvents::MouseUnlockEvent::subscribe(fmu);

    // make borderless
    /*InputEvents::WindowBordersToggleEvent::notify();*/
    
    /*
    * Initialization succesful
    */
    return true;
}

bool WindowManager::shouldClose()
{
    return glfwWindowShouldClose(m_window);
}

void WindowManager::swapBuffers()
{
    glfwSwapBuffers(m_window);
}


WindowManager::GLProc WindowManager::getProcAddress()
{
    return (GLProc)glfwGetProcAddress;
}

GLFWwindow* WindowManager::getWindow()
{
    return m_window;
}

float WindowManager::getTime()
{
    return (float)glfwGetTime();
}

void WindowManager::pollEvents()
{
    glfwPollEvents();
}

void WindowManager::borderlessWindow()
{
    glfwSetWindowAttrib(m_window, GLFW_DECORATED, GLFW_FALSE);
    glfwSetWindowPos(m_window, 0, 0);
    glfwSetWindowSize(m_window, m_maxWidth, m_maxHeight);
    m_borderless = true;
}

void WindowManager::borderedWindow()
{
    glfwSetWindowAttrib(m_window, GLFW_DECORATED, GLFW_TRUE);
    glfwSetWindowPos(m_window, 0, 30);
    glfwSetWindowAttrib(m_window, GLFW_MAXIMIZED, GLFW_TRUE);
    m_borderless = false;
}

void WindowManager::toggleWindowBorders()
{
    if (m_borderless)
        borderedWindow();
    else
        borderlessWindow();
}

void WindowManager::framebufferResizeCallback(GLFWwindow* window, int w, int h)
{
    width = w;
    height = h;
    InputEvents::WindowDimensionsChangedEvent::notify(w, h);
}

void WindowManager::errorCallback(int error, const char* description)
{
    LoggerLocator::getLogger()->getLogger()->error("WindowManager: glfw error: {} {}", error, description);
}

void WindowManager::closeWindowEventHandler()
{
    glfwSetWindowShouldClose(m_window, true);
}

void WindowManager::MouseLockHandler()
{
    /*
    * Lock cursor to screen
    */
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowManager::MouseUnlockHandler()
{
   /*
   * Unlock cursor
   */
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
