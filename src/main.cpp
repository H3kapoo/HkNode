#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderer/HkShader.hpp"
#include "base/HkNode.hpp"
#include "management/HkSceneManagement.hpp"
#include "elements/HkWindowFrame.hpp"
#include "elements/HkContainer.hpp"
#include "elements/HkButton.hpp"

using namespace hkui;

void resizeCallback(GLFWwindow* window, int width, int height)
{
    // let the user call the functions
    HkSceneManagement::get().resizeWindowEvent(window, width, height);
}

void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
    // let the user call the functions
    HkSceneManagement::get().mouseMoveEvent(window, xpos, ypos);
}

void mouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
    // let the user call the functions
    HkSceneManagement::get().mouseClickEvent(window, button, action, mods);
}

void mouseEnterCallback(GLFWwindow* window, int entered)
{
    // let the user call the functions
    HkSceneManagement::get().mouseEnterEvent(window, entered);
}

void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // let the user call the functions
    HkSceneManagement::get().mouseScrollEvent(window, xoffset, yoffset);
}

void dropCallback(GLFWwindow* window, int count, const char** paths)
{
    // let the user call the functions
    HkSceneManagement::get().dropEvent(window, count, paths);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glEnable(GL_DEPTH_TEST); // this shows nothing now

    GLFWwindow* window = glfwCreateWindow(1280, 720, "HkUI", NULL, NULL);
    // GLFWwindow* window = glfwCreateWindow(1920, 1080, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    // glfwSwapInterval(20); // zero to disable Vsync
    glfwSetFramebufferSizeCallback(window, resizeCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);
    glfwSetMouseButtonCallback(window, mouseClickCallback);
    glfwSetCursorEnterCallback(window, mouseEnterCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    glfwSetDropCallback(window, dropCallback);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW failed to init\n";
        return -1;
    }


    HkWindowFramePtr windowFrame = std::make_shared<HkWindowFrame>("WindowFrameHaha");

    windowFrame->printTree();
    windowFrame->setPos({ 1280 / 2, 720 / 2 });
    windowFrame->setSize({ 800, 30 });

    HkSceneManagement::get().setRoot(windowFrame);
    HkSceneManagement::get().init(1280, 720);
    // HkSceneManagement::get().init(1920, 1080);

    double previousTime = glfwGetTime();
    int frameCount = 0;
    while (!glfwWindowShouldClose(window))
    {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //TODO: GLFWAPI void glfwPostEmptyEvent(void); could be used in the future in case of animations
        HkSceneManagement::get().update();
        glfwSwapBuffers(window);
        glfwWaitEvents();
        // glfwPollEvents();

        // Measure speed
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - previousTime >= 1.0)
        {
            glfwSetWindowTitle(window, std::to_string(frameCount).c_str());
            frameCount = 0;
            previousTime = currentTime;
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
