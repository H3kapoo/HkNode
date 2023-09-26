#include "APIGate/GlfwGlewGate.hpp"

/*Only for debugging*/
// #include "utils/HkDrawDebugger.hpp"

#include "management/HkAppManager.hpp"
#include "management/HkSceneManagement.hpp"
#include "elements/HkWindowFrame.hpp"
#include "elements/HkContainer.hpp"
#include "elements/HkButton.hpp"
#include "elements/HkImageView.hpp"

using namespace hkui;

// #include <GL/glew.h>
// #include <GLFW/glfw3.h>
// #include <iostream>

// #include <GL/glew.h>
// #include <GLFW/glfw3.h>
// #include <iostream>
// #include <GL/glew.h>
// #include <GLFW/glfw3.h>
// #include <iostream>

// float vertices[] = {
//     -0.125f, -0.125f,
//      0.125f, -0.125f,
//      0.125f,  0.125f,
//     -0.125f,  0.125f
// };

// bool isDragging = false;
// float objectX = 0.0f;
// float objectY = 0.0f;

// static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
//     if (isDragging)
//     {
//         int width, height;
//         glfwGetWindowSize(window, &width, &height);

//         // Convert cursor coordinates to object coordinates
//         objectX = (xpos / width) * 2.0f - 1.0f;
//         objectY = -((ypos / height) * 2.0f - 1.0f);
//     }
// }

// static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
//     if (button == GLFW_MOUSE_BUTTON_LEFT)
//     {
//         if (action == GLFW_PRESS)
//         {
//             double xpos, ypos;
//             glfwGetCursorPos(window, &xpos, &ypos);

//             int width, height;
//             glfwGetWindowSize(window, &width, &height);

//             // Convert cursor coordinates to object coordinates
//             xpos = (xpos / width) * 2.0 - 1.0;
//             ypos = -((ypos / height) * 2.0 - 1.0);

//             // Check if the cursor is within the object's bounds
//             if (xpos >= (objectX - 0.125f) && xpos <= (objectX + 0.125f) &&
//                 ypos >= (objectY - 0.125f) && ypos <= (objectY + 0.125f))
//             {
//                 isDragging = true;
//             }
//         }
//         else if (action == GLFW_RELEASE)
//         {
//             isDragging = false;
//         }
//     }
// }

// int main()
// {
//     if (!glfwInit())
//     {
//         std::cerr << "Failed to initialize GLFW" << std::endl;
//         return -1;
//     }

//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

//     GLFWwindow* window = glfwCreateWindow(800, 600, "Drag Quad with Mouse", nullptr, nullptr);
//     if (!window)
//     {
//         std::cerr << "Failed to create GLFW window" << std::endl;
//         glfwTerminate();
//         return -1;
//     }

//     glfwMakeContextCurrent(window);

//     if (glewInit() != GLEW_OK)
//     {
//         std::cerr << "Failed to initialize GLEW" << std::endl;
//         glfwTerminate();
//         return -1;
//     }

//     glfwSetMouseButtonCallback(window, mouse_button_callback);
//     glfwSetCursorPosCallback(window, cursor_position_callback);

//     GLuint VAO, VBO;
//     glGenVertexArrays(1, &VAO);
//     glGenBuffers(1, &VBO);

//     glBindVertexArray(VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);

//     const char* vertexShaderSource = R"(
//         #version 330 core
//         layout (location = 0) in vec2 position;
//         uniform vec2 objectPosition;
//         void main()
//         {
//             gl_Position = vec4(position + objectPosition, 0.0, 1.0);
//         }
//     )";

//     const char* fragmentShaderSource = R"(
//         #version 330 core
//         out vec4 FragColor;
//         uniform vec3 color;
//         void main()
//         {
//             FragColor = vec4(color, 1.0);
//         }
//     )";

//     GLuint vertexShader, fragmentShader, shaderProgram;
//     vertexShader = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
//     glCompileShader(vertexShader);

//     fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
//     glCompileShader(fragmentShader);

//     shaderProgram = glCreateProgram();
//     glAttachShader(shaderProgram, vertexShader);
//     glAttachShader(shaderProgram, fragmentShader);
//     glLinkProgram(shaderProgram);
//     glUseProgram(shaderProgram);

//     GLint objectPositionLocation = glGetUniformLocation(shaderProgram, "objectPosition");
//     GLint colorLocation = glGetUniformLocation(shaderProgram, "color");
//     glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);

//     while (!glfwWindowShouldClose(window))
//     {
//         glClear(GL_COLOR_BUFFER_BIT);

//         glUniform2f(objectPositionLocation, objectX, objectY);

//         glBindVertexArray(VAO);
//         glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     glfwTerminate();
//     return 0;
// }

int main()
{
    //TODO: We need the dummy context used for loading glfw to also load all possible VAOs and assets
    // but by doing this, we lose the ability to load things at runtime I think..or not?
    if (!HkAppManager::get().setup()) { return -1; }

    //TODO: Make it the responsability of the window to generate VAO and shader?
    // we need to make it order agnostic
    HkWindowManagerPtr sceneWindow1 = std::make_shared<HkWindowManager>("MyWindowManager",
        HkWindowManager::HkWindowConfig{ .width = 1920, .height = 1080, .isMaster = true });
    // HkWindowManagerPtr sceneWindow2 = std::make_shared<HkWindowManager>("MyWindowManager2",
    //     HkWindowManager::HkWindowConfig{ .width = 800, .height = 600 });

    HkAppManager::get().addWindow(sceneWindow1);
    // HkAppManager::get().addWindow(sceneWindow2);
    sceneWindow1->makeContextCurrent();

    HkWindowFramePtr windowFrame = std::make_shared<HkWindowFrame>("MyWindowFrame");
    windowFrame->setWindowMode(HkWindowFrameMode::Grabbable);
    sceneWindow1->addSubWindow(windowFrame); //NOTE: Needs to be added before adding any children

    windowFrame->getStyle().setOverflowAllowedXY(true)
        .setDirection(HkDirection::Horizontal)
        .setHAlignment(HkHAlignment::Left)
        .setVAlignment(HkVAlignment::Center);

    windowFrame->setPos({ 1280 * 0.25, 720 * 0.25 });
    windowFrame->setSize({ 1280 * 0.4, 720 * 0.7 });

    HkContainerPtr ctr = std::make_shared<HkContainer>("MyContainer");
    HkContainerPtr ctr4 = std::make_shared<HkContainer>("MyContainer4");

    ctr->getStyle().setColor({ 1.3f,0.3f,0.7f });
    ctr4->getStyle().setColor({ 0.7f,0.8f,0.9f });

    ctr->setSize({ 200, 300 });
    ctr4->setSize({ 100, 150 });

    // std::vector<HkNodeBasePtr> ctrs2;
    // ctrs2.reserve(5'000);
    // for (int i = 0;i < 5'000;i++) // with O2 works ok 01.09.2023
    // {
    //     const auto& ct = std::make_shared<HkContainer>("MyContauner");
    //     ct->getStyle().setColor(i % 2 == 0 ? glm::vec3{ 0.75f, 0.75f, 0.75f } : glm::vec3{ 1.0f, 1.0f, 1.0f });
    //     ct->setSize({ 20, 20 });
    //     ctrs2.push_back(std::move(ct));
    // }

    windowFrame->pushChildren({ ctr, ctr4 });
    // windowFrame->pushChildren(ctrs2);
    // sceneWindow1->makeContextNotCurrent();

    // sceneWindow2->makeContextCurrent();

    HkWindowFramePtr windowFrame2 = std::make_shared<HkWindowFrame>("MyWindowFrame2");
    windowFrame2->setWindowMode(HkWindowFrameMode::FullScreenFixed);
    // sceneWindow2->addSubWindow(windowFrame2); //NOTE: Needs to be added before adding any children

    windowFrame2->getStyle().setOverflowAllowedXY(true)
        .setDirection(HkDirection::Horizontal)
        .setHAlignment(HkHAlignment::Right)
        .setVAlignment(HkVAlignment::Center);

    HkContainerPtr ctr2 = std::make_shared<HkContainer>("MyContainer2");
    HkContainerPtr ctr3 = std::make_shared<HkContainer>("MyContainer3");


    ctr2->getStyle().setColor({ 1.0f,0.4f,0.5f });
    ctr3->getStyle().setColor({ 0.4f,0.5f,0.6f });

    ctr2->setSize({ 300, 350 });
    ctr3->setSize({ 400, 450 });

    windowFrame2->pushChildren({ ctr2, ctr3 });


    // HkWindowManagerPtr sceneWindow3 = std::make_shared<HkWindowManager>("MyWindowManager3",
    //     HkWindowManager::HkWindowConfig{ .width = 800, .height = 600 });
    // HkAppManager::get().addWindow(sceneWindow3);

    // sceneWindow3->makeContextCurrent();

    HkWindowFramePtr windowFrame3 = std::make_shared<HkWindowFrame>("MyWindowFrame2");
    windowFrame3->setWindowMode(HkWindowFrameMode::FullScreenFixed);
    // sceneWindow3->addSubWindow(windowFrame3); //NOTE: Needs to be added before adding any children

    windowFrame3->getStyle().setOverflowAllowedXY(true)
        .setDirection(HkDirection::Horizontal)
        .setHAlignment(HkHAlignment::Right)
        .setVAlignment(HkVAlignment::Center);

    HkContainerPtr ctr5 = std::make_shared<HkContainer>("MyContainer5");
    HkContainerPtr ctr6 = std::make_shared<HkContainer>("MyContainer6");

    ctr5->getStyle().setColor({ 0.0f,0.1f,0.2f });
    ctr6->getStyle().setColor({ 1.0f,0.4f,0.5f });

    ctr5->setSize({ 330, 450 });
    ctr6->setSize({ 400, 450 });

    windowFrame3->getStyle().setOverflowAllowedXY(true)
        .setDirection(HkDirection::Horizontal)
        .setHAlignment(HkHAlignment::Right)
        .setVAlignment(HkVAlignment::Center);

    windowFrame3->pushChildren({ ctr5, ctr6 });


    // std::vector<HkNodeBasePtr> ctrs2;
    // ctrs2.reserve(5'000);
    // for (int i = 0;i < 5'000;i++) // with O2 works ok 01.09.2023
    // {
    //     const auto& ct = std::make_shared<HkContainer>("MyContauner");
    //     ct->getStyle().setColor(i % 2 == 0 ? glm::vec3{ 0.75f, 0.75f, 0.75f } : glm::vec3{ 1.0f, 1.0f, 1.0f });
    //     ct->setSize({ 20, 20 });
    //     ctrs2.push_back(std::move(ct));
    // }

    // HkContainerPtr ctr = std::make_shared<HkContainer>("MyContainer");
    // HkContainerPtr ctr2 = std::make_shared<HkContainer>("MyContainer2");
    // HkContainerPtr ctr3 = std::make_shared<HkContainer>("MyContainer3");
    // HkContainerPtr ctr4 = std::make_shared<HkContainer>("MyContainer4");
    // HkContainerPtr ctr5 = std::make_shared<HkContainer>("MyContainer5");
    // HkContainerPtr ctr6 = std::make_shared<HkContainer>("MyContainer6");


    // ctr->getStyle().setColor({ 1.3f,0.3f,0.7f });
    // ctr2->getStyle().setColor({ 1.0f,0.4f,0.5f });
    // ctr3->getStyle().setColor({ 0.4f,0.5f,0.6f });
    // ctr4->getStyle().setColor({ 0.7f,0.8f,0.9f });
    // ctr5->getStyle().setColor({ 0.0f,0.1f,0.2f });
    // ctr6->getStyle().setColor({ 1.0f,0.4f,0.5f });

    // ctr->setSize({ 200, 300 });
    // ctr2->setSize({ 300, 350 });
    // ctr3->setSize({ 400, 450 });
    // ctr4->setSize({ 100, 150 });
    // ctr5->setSize({ 330, 450 });
    // ctr6->setSize({ 400, 450 });

    // windowFrame->pushChildren({ ctr, ctr4 });
    // windowFrame2->pushChildren({ ctr2, ctr3 });
    // windowFrame3->pushChildren({ ctr5, ctr6 });

    HkAppManager::get().runLoop(); // Blocking from here on, maybe could be run in separate thread

    // HkAppManager::get().removeWindow(sceneWindow2); //TODO: To be done automatically at teardown
    // HkAppManager::get().removeWindow(sceneWindow1); //TODO: To be done automatically at teardown
    HkAppManager::get().teardown();
    // glfwInit();
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // // GLFWwindow* window = glfwCreateWindow(1280, 720, "HkUI", NULL, NULL);
    // GLFWwindow* window = glfwCreateWindow(1920, 1080, "HkUI", NULL, NULL);
    // if (window == NULL)
    // {
    //     std::cout << "Failed to create GLFW window" << std::endl;
    //     glfwTerminate();
    //     return -1;
    // }
    std::cout << "got here\n";
    return 0;

    // glfwMakeContextCurrent(window);
    // glfwSwapInterval(0); // zero to disable Vsync
    // glfwSetFramebufferSizeCallback(window, resizeCallback);
    // glfwSetCursorPosCallback(window, mouseMoveCallback);
    // glfwSetMouseButtonCallback(window, mouseClickCallback);
    // glfwSetCursorEnterCallback(window, mouseEnterCallback);
    // glfwSetScrollCallback(window, mouseScrollCallback);
    // glfwSetDropCallback(window, dropCallback);

    // if (glewInit() != GLEW_OK)
    // {
    //     std::cout << "GLEW failed to init\n";
    //     return -1;
    // }

    // HkWindowFramePtr windowFrame = std::make_shared<HkWindowFrame>("MyWindowFrame");

    // windowFrame->setWindowMode(HkWindowFrameMode::FullScreenFixed);
    // // windowFrame->setWindowMode(HkWindowFrameMode::Grabbable);

    // HkContainerPtr ctr = std::make_shared<HkContainer>("MyContainer");
    // HkContainerPtr ctr2 = std::make_shared<HkContainer>("MyContainer2");
    // HkContainerPtr ctr3 = std::make_shared<HkContainer>("MyContainer3");
    // HkContainerPtr ctr4 = std::make_shared<HkContainer>("MyContainer4");
    // HkContainerPtr ctr5 = std::make_shared<HkContainer>("MyContainer5");
    // std::vector<HkContainerPtr> all_ctrs{ ctr,ctr2, ctr3, ctr4, ctr5 };
    // for (const auto& c : all_ctrs)
    // {
    //     c->getStyle().setVAlignment(HkVAlignment::Center);
    // }

    // std::vector<HkNodeBasePtr> ctrs2;
    // ctrs2.reserve(5'000);
    // for (int i = 0;i < 5'000;i++) // with O2 works ok 01.09.2023
    // {
    //     const auto& ct = std::make_shared<HkContainer>("MyContauner");
    //     ct->getStyle().setColor(i % 2 == 0 ? glm::vec3{ 0.75f, 0.75f, 0.75f } : glm::vec3{ 1.0f, 1.0f, 1.0f });
    //     ct->setSize({ 20, 20 });
    //     ctrs2.push_back(std::move(ct));
    // }

    // HkImageViewPtr imgView = std::make_shared<HkImageView>("MyImgView");
    // HkImageViewPtr imgView2 = std::make_shared<HkImageView>("MyImgView2");
    // imgView->loadImage("/home/hekapoo/container.jpg");
    // imgView2->loadImage("/home/hekapoo/imeg.jpeg");

    // bool isFs = true;
    // int i = 2;
    // ctr3->getEvents().setOnReleaseListener([&windowFrame, &isFs, &all_ctrs, &i]()
    //     {
    //         isFs ? windowFrame->setWindowMode(HkWindowFrameMode::Grabbable)
    //             : windowFrame->setWindowMode(HkWindowFrameMode::FullScreenFixed);
    //         isFs = !isFs;
    //     });

    // ctr->getStyle().setColor({ 0.3f,0.3f,0.7f });
    // ctr2->getStyle().setColor({ 1.0f,0.4f,0.5f });
    // ctr3->getStyle().setColor({ 0.4f,0.5f,0.6f });
    // ctr4->getStyle().setColor({ 0.7f,0.8f,0.9f });
    // ctr5->getStyle().setColor({ 0.0f,0.1f,0.2f });


    // ctr->setSize({ 200, 300 });
    // ctr2->setSize({ 300, 350 });
    // ctr3->setSize({ 400, 450 });
    // ctr4->setSize({ 100, 150 });
    // ctr5->setSize({ 330, 450 });

    // ctr5->getEvents()
    //     .setOnClickListener([]() {std::cout << "hello\n";})
    //     .setOnScrollListener([&ctr5]() { std::cout << "now scrolling me " << ctr5->getNodeInfo().name << "\n"; });


    // imgView->setSize({ 300, 300 });

    // windowFrame->setPos({ 1280 * 0.25, 720 * 0.25 });
    // windowFrame->setSize({ 1280 * 0.4, 720 * 0.7 });

    // // windowFrame->setPos({ 300, 100 });
    // // windowFrame->setSize({ 1280 / 2, 720 / 2 });

    // windowFrame->getStyle().setOverflowAllowedXY(true)
    //     .setDirection(HkDirection::Horizontal)
    //     .setHAlignment(HkHAlignment::Left)
    //     .setVAlignment(HkVAlignment::Center);
    // windowFrame->pushChildren({ ctr, ctr2, ctr3, ctr4, ctr5 });
    // // windowFrame->pushChildren(ctrs2);
    // ctr->pushChildren({ ctr2 });
    // ctr2->pushChildren({ ctr3 });
    // ctr3->pushChildren({ imgView, imgView2 });
    // ctr3->getStyle().setOverflowAllowedXY(true);
    // ctr2->getStyle().setOverflowAllowedXY(true);
    // imgView->getStyle().setAllMargins(20);

    // ctr->getStyle().setLeftRightMargins(30);

    // HkSceneManagement::get().setRoot(windowFrame);
    // HkSceneManagement::get().init(1920, 1080); // <--- GET RID OF THIS, IT FUCKS UP IF NOT SET EXACTLY AS THE WINDOW CREATION SIZE
    // int time = 60'000;
    // double previousTime = glfwGetTime();
    // int frameCount = 0;
    // // while (!glfwWindowShouldClose(window))
    // // {

    // //     glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // //     //TODO: GLFWAPI void glfwPostEmptyEvent(void); could be used in the future in case of animations
    // //     HkSceneManagement::get().update();
    // //     //TODO: ^^ we might have to split rendering and updating
    // //     // HkDrawDebugger::get().drawBuffer();
    // //     glfwSwapBuffers(window);
    // //     // glfwWaitEvents();
    // //     glfwPollEvents();

    // //     // Measure speed
    // //     double currentTime = glfwGetTime();
    // //     frameCount++;
    // //     if (currentTime - previousTime >= 1.0)
    // //     {
    // //         glfwSetWindowTitle(window, std::to_string(frameCount).c_str());
    // //         frameCount = 0;
    // //         previousTime = currentTime;
    // //     }
    // //     time--;
    // // }

    // // glfwDestroyWindow(window);
    // // glfwTerminate();
    // return 0;
}
