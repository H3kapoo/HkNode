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
// #define GLFW_INCLUDE_NONE
// #include <GLFW/glfw3.h>

// #include <stdio.h>
// #include <stdlib.h>

// #include "getopt.h"

// static const char* vertex_shader_text =
// "#version 110\n"
// "uniform mat4 MVP;\n"
// "attribute vec2 vPos;\n"
// "varying vec2 texcoord;\n"
// "void main()\n"
// "{\n"
// "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
// "    texcoord = vPos;\n"
// "}\n";

// static const char* fragment_shader_text =
// "#version 110\n"
// "uniform sampler2D texture;\n"
// "uniform vec3 color;\n"
// "varying vec2 texcoord;\n"
// "void main()\n"
// "{\n"
// "    gl_FragColor = vec4(color * texture2D(texture, texcoord).rgb, 1.0);\n"
// "}\n";

// static const glm::vec2 vertices[4] =
// {
//     { 0.f, 0.f },
//     { 1.f, 0.f },
//     { 1.f, 1.f },
//     { 0.f, 1.f }
// };

// static void error_callback(int error, const char* description)
// {
//     fprintf(stderr, "Error: %s\n", description);
// }

// static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
// {
//     if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
//         glfwSetWindowShouldClose(window, GLFW_TRUE);
// }

// int main(int argc, char** argv)
// {
//     GLFWwindow* windows[2];
//     GLuint texture, program, vertex_buffer;
//     GLint mvp_location, vpos_location, color_location, texture_location;

//     glfwSetErrorCallback(error_callback);

//     if (!glfwInit())
//         exit(EXIT_FAILURE);

//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

//     windows[0] = glfwCreateWindow(400, 400, "First", NULL, NULL);
//     if (!windows[0])
//     {
//         glfwTerminate();
//         exit(EXIT_FAILURE);
//     }

//     glfwSetKeyCallback(windows[0], key_callback);

//     glfwMakeContextCurrent(windows[0]);

//     // Only enable vsync for the first of the windows to be swapped to
//     // avoid waiting out the interval for each window
//     glfwSwapInterval(1);

//     // The contexts are created with the same APIs so the function
//     // pointers should be re-usable between them
//     if (glewInit() != GLEW_OK)
//     {
//         std::cerr << "GLEW failed to initialize\n";
//         return false;
//     }

//     // Create the OpenGL objects inside the first context, created above
//     // All objects will be shared with the second context, created below
//     {
//         int x, y;
//         char pixels[16 * 16];
//         GLuint vertex_shader, fragment_shader;

//         glGenTextures(1, &texture);
//         glBindTexture(GL_TEXTURE_2D, texture);

//         srand((unsigned int)glfwGetTimerValue());

//         for (y = 0; y < 16; y++)
//         {
//             for (x = 0; x < 16; x++)
//                 pixels[y * 16 + x] = rand() % 256;
//         }

//         glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 16, 16, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

//         vertex_shader = glCreateShader(GL_VERTEX_SHADER);
//         glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
//         glCompileShader(vertex_shader);

//         fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
//         glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
//         glCompileShader(fragment_shader);

//         program = glCreateProgram();
//         glAttachShader(program, vertex_shader);
//         glAttachShader(program, fragment_shader);
//         glLinkProgram(program);

//         mvp_location = glGetUniformLocation(program, "MVP");
//         color_location = glGetUniformLocation(program, "color");
//         texture_location = glGetUniformLocation(program, "texture");
//         vpos_location = glGetAttribLocation(program, "vPos");

//         glGenBuffers(1, &vertex_buffer);
//         glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
//         glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//     }

//     glUseProgram(program);
//     glUniform1i(texture_location, 0);

//     glEnable(GL_TEXTURE_2D);
//     glBindTexture(GL_TEXTURE_2D, texture);

//     glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
//     glEnableVertexAttribArray(vpos_location);
//     glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
//         sizeof(vertices[0]), (void*)0);

//     windows[1] = glfwCreateWindow(400, 400, "Second", NULL, windows[0]);
//     if (!windows[1])
//     {
//         glfwTerminate();
//         exit(EXIT_FAILURE);
//     }

//     // Place the second window to the right of the first
//     {
//         int xpos, ypos, left, right, width;

//         glfwGetWindowSize(windows[0], &width, NULL);
//         glfwGetWindowFrameSize(windows[0], &left, NULL, &right, NULL);
//         glfwGetWindowPos(windows[0], &xpos, &ypos);

//         glfwSetWindowPos(windows[1], xpos + width + left + right, ypos);
//     }

//     glfwSetKeyCallback(windows[1], key_callback);

//     glfwMakeContextCurrent(windows[1]);

//     // While objects are shared, the global context state is not and will
//     // need to be set up for each context

//     glUseProgram(program);

//     glEnable(GL_TEXTURE_2D);
//     glBindTexture(GL_TEXTURE_2D, texture);

//     glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
//     glEnableVertexAttribArray(vpos_location);
//     glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
//         sizeof(vertices[0]), (void*)0);

//     while (!glfwWindowShouldClose(windows[0]) &&
//         !glfwWindowShouldClose(windows[1]))
//     {
//         int i;
//         const glm::vec3 colors[2] =
//         {
//             { 0.8f, 0.4f, 1.f },
//             { 0.3f, 0.4f, 1.f }
//         };

//         for (i = 0; i < 2; i++)
//         {
//             int width, height;

//             glfwGetFramebufferSize(windows[i], &width, &height);
//             glfwMakeContextCurrent(windows[i]);

//             glViewport(0, 0, width, height);

//             glm::mat4 sceneProjMatrix = glm::ortho(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);

//             glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(sceneProjMatrix));
//             glUniform3f(color_location, colors[i][0], colors[i][1], colors[i][2]);
//             glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

//             glfwSwapBuffers(windows[i]);
//         }

//         glfwWaitEvents();
//     }

//     glfwTerminate();
//     exit(EXIT_SUCCESS);
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
    windowFrame->setWindowMode(HkWindowFrameMode::FullScreenFixed);
    sceneWindow1->addSubWindow(windowFrame); //NOTE: Needs to be added before adding any children

    windowFrame->getStyle().setOverflowAllowedXY(true)
        .setDirection(HkDirection::Horizontal)
        .setHAlignment(HkHAlignment::Left)
        .setVAlignment(HkVAlignment::Center);

    windowFrame->setPos({ 1280 * 0.25, 720 * 0.25 });
    windowFrame->setSize({ 1280 * 0.4, 720 * 0.7 });

    // HkContainerPtr ctr = std::make_shared<HkContainer>("MyContainer");
    // HkContainerPtr ctr4 = std::make_shared<HkContainer>("MyContainer4");

    // ctr->getStyle().setColor({ 1.3f,0.3f,0.7f });
    // ctr4->getStyle().setColor({ 0.7f,0.8f,0.9f });

    // ctr->setSize({ 200, 300 });
    // ctr4->setSize({ 100, 150 });

    std::vector<HkNodeBasePtr> ctrs2;
    ctrs2.reserve(1'000);
    for (int i = 0;i < 1'000;i++) // with O2 works ok 01.09.2023
    {
        const auto& ct = std::make_shared<HkContainer>("MyContauner");
        ct->getStyle().setColor(i % 2 == 0 ? glm::vec3{ 0.75f, 0.75f, 0.75f } : glm::vec3{ 1.0f, 1.0f, 1.0f });
        ct->setSize({ 20, 20 });
        ctrs2.push_back(std::move(ct));
    }

    // windowFrame->pushChildren({ ctr, ctr4 });
    windowFrame->pushChildren(ctrs2);
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

    // HkWindowFramePtr windowFrame3 = std::make_shared<HkWindowFrame>("MyWindowFrame2");
    // windowFrame3->setWindowMode(HkWindowFrameMode::FullScreenFixed);
    // sceneWindow3->addSubWindow(windowFrame3); //NOTE: Needs to be added before adding any children

    // windowFrame3->getStyle().setOverflowAllowedXY(true)
    //     .setDirection(HkDirection::Horizontal)
    //     .setHAlignment(HkHAlignment::Right)
    //     .setVAlignment(HkVAlignment::Center);

    // HkContainerPtr ctr5 = std::make_shared<HkContainer>("MyContainer5");
    // HkContainerPtr ctr6 = std::make_shared<HkContainer>("MyContainer6");


    // ctr5->getStyle().setColor({ 0.0f,0.1f,0.2f });
    // ctr6->getStyle().setColor({ 1.0f,0.4f,0.5f });

    // ctr5->setSize({ 330, 450 });
    // ctr6->setSize({ 400, 450 });

    // windowFrame3->pushChildren({ ctr5, ctr6 });


    // windowFrame3->getStyle().setOverflowAllowedXY(true)
    //     .setDirection(HkDirection::Horizontal)
    //     .setHAlignment(HkHAlignment::Right)
    //     .setVAlignment(HkVAlignment::Center);

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
