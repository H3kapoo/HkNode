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
//TODO: Input throttling
double lastMoveTime = 0.0;
// double CALLBACK_THRESHOLD = 0.3;
double CALLBACK_THRESHOLD = 0.05;
void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
    double currentTime = glfwGetTime();

    if (currentTime - lastMoveTime < CALLBACK_THRESHOLD)
    {
        return;
    }
    // let the user call the functions
    HkSceneManagement::get().mouseMoveEvent(window, xpos, ypos);
    lastMoveTime = currentTime;
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

    // GLFWwindow* window = glfwCreateWindow(1280, 720, "HkUI", NULL, NULL);
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "HkUI", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // zero to disable Vsync
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

    windowFrame->setWindowMode(HkWindowFrameMode::FullScreenFixed);
    // windowFrame->setWindowMode(HkWindowFrameMode::Grabbable);

    HkContainerPtr ctr = std::make_shared<HkContainer>("MyContainer");
    HkContainerPtr ctr2 = std::make_shared<HkContainer>("MyContainer2");
    HkContainerPtr ctr3 = std::make_shared<HkContainer>("MyContainer3");
    HkContainerPtr ctr4 = std::make_shared<HkContainer>("MyContainer4");
    HkContainerPtr ctr5 = std::make_shared<HkContainer>("MyContainer5");
    std::vector<HkContainerPtr> all_ctrs{ ctr,ctr2, ctr3, ctr4, ctr5 };
    for (const auto& c : all_ctrs)
    {
        c->getStyle().setVAlignment(HkVAlignment::Center);
        // c->setHAlignment(HkAlignment::Bottom);
        // c->setMargins(HkStyleParams{ .marginLX = 10, .marginRX = 10, .marginTY = 10, .marginBY = 10 });
    }

    // uint32_t marginLX{ 0 }, marginRX{ 0 };
// uint32_t marginTY{ 0 }, marginBY{ 0 };
// ctr4->setMargins(HkStyleParams{ .marginLX = 0, .marginRX = 0, .marginTY = 10 });

    std::vector<HkNodeBasePtr> ctrs2;
    ctrs2.reserve(10'000);
    for (int i = 0;i < 10'000;i++) // with O2 works ok 01.09.2023
    {
        // ctrs.emplace_back("MyContainer" + std::to_string(i + 20));
        const auto& ct = std::make_shared<HkContainer>("MyContauner");
        // ct->setColor(i % 2 == 0 ? glm::vec3{ 0.7f, 0.8f, 0.9f } : glm::vec3{ 0.6f, 0.7f, 0.8f });
        ct->setColor(i % 2 == 0 ? glm::vec3{ 0.75f, 0.75f, 0.75f } : glm::vec3{ 1.0f, 1.0f, 1.0f });
        ct->setSize({ 20, 20 });
        ctrs2.push_back(std::move(ct));
        // ctrs2.emplace_back(std::make_shared<HkContainer>("MyContauner"));
        // ctrs2.push_back(ctrs.at(i));
    }

    HkImageViewPtr imgView = std::make_shared<HkImageView>("MyImgView");
    HkImageViewPtr imgView2 = std::make_shared<HkImageView>("MyImgView2");
    imgView->loadImage("/home/hekapoo/container.jpg");
    imgView2->loadImage("/home/hekapoo/imeg.jpeg");

    bool isFs = false;
    int i = 2;
    ctr3->setOnClickListener([&windowFrame, &isFs, &all_ctrs, &i]()
        {
            isFs ? windowFrame->setWindowMode(HkWindowFrameMode::Grabbable)
                : windowFrame->setWindowMode(HkWindowFrameMode::FullScreenFixed);
            isFs = !isFs;
            // imgView2->loadImage(isFs ? "/home/hekapoo/imeg.jpeg" : "/home/hekapoo/container.jpg");
            // std::cout << "eu sunt\n";
            // for (const auto& c : all_ctrs)
            // {
            //     c->setHAlignment((HkAlignment)i);
            // }
            // i++;
            // if (i > 4) i = 2;

        });

    // std::cout << "-----" << (sizeof(*ctr) * ctrs2.size()) / 1024.0f / 1024.0f << "mb\n";

    ctr->setColor({ 0.3f,0.3f,0.7f });
    ctr2->setColor({ 1.0f,0.4f,0.5f });
    ctr3->setColor({ 0.4f,0.5f,0.6f });
    ctr4->setColor({ 0.7f,0.8f,0.9f });
    ctr5->setColor({ 0.0f,0.1f,0.2f });
    // ctr->setSize({ 100, 200 });
    // ctr2->setSize({ 100, 300 });
    // ctr3->setSize({ 300, 200 });
    // ctr4->setSize({ 100, 100 });
    // ctr5->setSize({ 500, 30 });

    ctr->setSize({ 200, 300 });
    ctr2->setSize({ 300, 350 });
    ctr3->setSize({ 400, 450 });
    ctr4->setSize({ 100, 150 });
    ctr5->setSize({ 330, 450 });

    // ctr5->getStyle().setColor({ 1.0f, 1.0f, 1.0f });

    // ctr->setSize({ 50, 200 });
    // ctr2->setSize({ 250, 300 });
    // ctr3->setSize({ 150, 200 });
    // ctr4->setSize({ 50, 100 });
    // ctr5->setSize({ 450, 330 });

    imgView->setSize({ 300, 300 });
    // imgView2->setSize({ 1280 * 0.25f, 720 * 0.5f });
    // ctr->setOnClickListener()

    windowFrame->setPos({ 1280 * 0.25, 720 * 0.25 });
    windowFrame->setSize({ 1280 * 0.4, 720 * 0.7 });

    // windowFrame->setPos({ 300, 100 });
    // windowFrame->setSize({ 1280 / 2, 720 / 2 });

    /*Fill screen hack for not, later we shall position stuff relative to top left corner, not center*/
    // windowFrame->setPos({ 1280 * 0.5f, 15 });
    // windowFrame->setSize({ 1280 * 0.40, 720 * 0.75 - 30 });

    // windowFrame->pushChildren({ ctr, ctr2 });
    windowFrame->getStyle().setOverflowAllowedXY(true)
        .setDirection(HkDirection::Horizontal)
        .setHAlignment(HkHAlignment::Left)
        .setVAlignment(HkVAlignment::Center);
    windowFrame->pushChildren({ ctr, ctr2, ctr3, ctr4, ctr5 });
    // windowFrame->pushChildren(ctrs2);
    ctr->pushChildren({ ctr2 });
    ctr2->pushChildren({ ctr3 });
    ctr3->pushChildren({ imgView, imgView2 });
    ctr3->getStyle().setOverflowAllowedXY(true);
    ctr2->getStyle().setOverflowAllowedXY(true);
    imgView->getStyle().setAllMargins(20);

    ctr->getStyle().setLeftRightMargins(30);
    // ctr2->setOverflow(true, true);
    // ctr->pushChildren({ ctr3 });
    // ctr2->pushChildren({ ctr4, ctr5 });
    // windowFrame->pushChildren({ ctr, ctr2, ctr3, ctr4, ctr5 });
    // windowFrame->printTree();

    HkSceneManagement::get().setRoot(windowFrame);
    // HkSceneManagement::get().init(1280, 720); // <--- GET RID OF THIS, IT FUCKS UP IF NOT SET EXACTLY AS THE WINDOW CREATION SIZE
    HkSceneManagement::get().init(1920, 1080);
    int time = 60'000;
    double previousTime = glfwGetTime();
    int frameCount = 0;
    while (!glfwWindowShouldClose(window) && time > 0)
    {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //TODO: GLFWAPI void glfwPostEmptyEvent(void); could be used in the future in case of animations
        HkSceneManagement::get().update();
        //TODO: ^^ we might have to split rendering and updating
        // HkDrawDebugger::get().drawBuffer();
        glfwSwapBuffers(window);
        // glfwWaitEvents();
        glfwPollEvents();

        // Measure speed
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - previousTime >= 1.0)
        {
            glfwSetWindowTitle(window, std::to_string(frameCount).c_str());
            frameCount = 0;
            previousTime = currentTime;
        }
        time--;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
