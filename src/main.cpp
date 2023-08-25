#include "APIGate/GlfwGlewGate.hpp"

/*Only for debugging*/
#include "utils/HkDrawDebugger.hpp"

#include "management/HkSceneManagement.hpp"
#include "elements/HkWindowFrame.hpp"
#include "elements/HkContainer.hpp"
#include "elements/HkButton.hpp"
#include "elements/HkImageView.hpp"

using namespace hkui;

//TODO: these shall be encapsulated inside HkSceneManagement
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

    HkWindowFramePtr windowFrame = std::make_shared<HkWindowFrame>("MyWindowFrame");

    // windowFrame->setConstraintPolicy(HkConstraintPolicy::AlignTopToBottom);
    windowFrame->setConstraintPolicy(HkConstraintPolicy::AlignLeftToRight);
    windowFrame->setWindowMode(HkWindowFrameMode::FullScreenFixed);
    // windowFrame->setWindowMode(HkWindowFrameMode::Grabbable);

    HkContainerPtr ctr = std::make_shared<HkContainer>("MyContainer");
    HkContainerPtr ctr2 = std::make_shared<HkContainer>("MyContainer2");
    HkContainerPtr ctr3 = std::make_shared<HkContainer>("MyContainer3");
    HkContainerPtr ctr4 = std::make_shared<HkContainer>("MyContainer4");
    HkContainerPtr ctr5 = std::make_shared<HkContainer>("MyContainer5");

    HkImageViewPtr imgView = std::make_shared<HkImageView>("MyImgView");
    HkImageViewPtr imgView2 = std::make_shared<HkImageView>("MyImgView2");
    imgView->loadImage("/home/hekapoo/container.jpg");
    imgView2->loadImage("/home/hekapoo/imeg.jpeg");

    bool isFs = false;
    ctr2->setOnClickListener([&windowFrame, &isFs, &imgView2]()
        {
            // isFs ? windowFrame->setWindowMode(HkWindowFrameMode::Grabbable)
                // : windowFrame->setWindowMode(HkWindowFrameMode::FullScreenFixed);
            isFs = !isFs;
            imgView2->loadImage(isFs ? "/home/hekapoo/imeg.jpeg" : "/home/hekapoo/container.jpg");
            std::cout << "eu sunt\n";
        });

    ctr->setColor({ 0.3f,0.3f,0.7f });
    ctr2->setColor({ 0.3f,0.4f,0.5f });
    ctr3->setColor({ 0.4f,0.5f,0.6f });
    ctr4->setColor({ 0.7f,0.8f,0.9f });
    ctr5->setColor({ 0.0f,0.1f,0.2f });
    ctr->setSize({ 800, 600 });
    ctr2->setSize({ 500, 700 });
    // ctr2->setSize({ 630, 200 });
    ctr3->setSize({ 430, 200 });
    ctr4->setSize({ 370, 400 });
    ctr5->setSize({ 200, 30 });
    imgView->setSize({ 1280, 720 });
    imgView2->setSize({ 1280 * 0.5f, 720 * 0.5f });

    // windowFrame->setPos({ 1280 * 0.5, 720 * 0.25 });
    // windowFrame->setSize({ 1280 * 0.5, 720 * 0.5 });

    windowFrame->setPos({ 300, 100 });
    windowFrame->setSize({ 1280 / 2, 720 / 2 });

    /*Fill screen hack for not, later we shall position stuff relative to top left corner, not center*/
    // windowFrame->setPos({ 1280 * 0.5f, 15 });
    // windowFrame->setSize({ 1280, 720 - 30 });

    windowFrame->pushChildren({ ctr,ctr2 });
    ctr->pushChildren({ imgView });
    ctr2->pushChildren({ imgView2 });
    // ctr->pushChildren({ ctr3 });
    // ctr2->pushChildren({ ctr4, ctr5 });

    // windowFrame->pushChildren({ ctr, ctr2, ctr3, ctr4, ctr5 });
    windowFrame->printTree();

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
        HkDrawDebugger::get().drawBuffer();
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
