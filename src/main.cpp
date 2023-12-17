#include <cstdlib>

#include "APIGate/GlfwGlewGate.hpp"
#include "management/HkAppManager.hpp"
#include "management/HkSceneManagement.hpp"
#include "elements/HkWindowFrame.hpp"
#include "elements/HkContainer.hpp"
#include "elements/HkButton.hpp"
#include "elements/HkLabel.hpp"

using namespace hkui;

int main()
{
    //TODO: We need the dummy context used for loading glfw to also load all possible VAOs and assets
    // but by doing this, we lose the ability to load things at runtime I think..or not?
    if (!HkAppManager::get().setup()) { return -1; }

    //TODO: Make it the responsability of the window to generate VAO and shader?
    // we need to make it order agnostic
    HkWindowManagerPtr sceneWindow1 = std::make_shared<HkWindowManager>("MyWindowManager",
        HkWindowManager::HkWindowConfig{ .width = 1600, .height = 900, .isMaster = true });
    // sceneWindow1->setBackgroundImage("/home/hekapoo/container.jpg");
    // sceneWindow1->setBackgroundImage("/home/hekapoo/Downloads/fbi_wp.jpg");
    // sceneWindow1->setBackgroundImage("/home/hekapoo/container.jpg");
    //TODO: There is a bug with 2 windows while animating the windowFrame movement
    // HkWindowManagerPtr sceneWindow2 = std::make_shared<HkWindowManager>("MyWindowManager2",
    //     HkWindowManager::HkWindowConfig{ .width = 800, .height = 600 });

    HkAppManager::get().addWindow(sceneWindow1);
    // HkAppManager::get().addWindow(sceneWindow2);
    // sceneWindow1->makeContextCurrent();

    HkWindowFramePtr windowFrame = std::make_shared<HkWindowFrame>("MyWindowFrame");
    windowFrame->setWindowMode(HkWindowFrameMode::FullScreenFixed);
    // windowFrame->setWindowMode(HkWindowFrameMode::Grabbable);
    sceneWindow1->addSubWindow(windowFrame); //NOTE: Needs to be added before adding any children

    windowFrame->setSize({ 1280, 720 });
    windowFrame->setPos({ 1920 / 2 - 500 , 1080 / 2 - 300 });

    HkContainerPtr ctr = std::make_shared<HkContainer>("MyContainer");
    HkContainerPtr ctr4 = std::make_shared<HkContainer>("MyContainer4");
    HkContainerPtr ctr5 = std::make_shared<HkContainer>("MyContainer5");
    HkContainerPtr ctr6 = std::make_shared<HkContainer>("MyContainer6");
    HkContainerPtr ctr7 = std::make_shared<HkContainer>("MyContainer7");
    HkContainerPtr ctr8 = std::make_shared<HkContainer>("MyContainer8");

    HkContainerPtr ctr9 = std::make_shared<HkContainer>("MyContainer9");
    HkContainerPtr ctr10 = std::make_shared<HkContainer>("MyContainer10");
    HkContainerPtr ctr11 = std::make_shared<HkContainer>("MyContainer11");
    HkContainerPtr ctr13 = std::make_shared<HkContainer>("MyContainer13");
    HkContainerPtr ctr14 = std::make_shared<HkContainer>("MyContainer14");

    HkButtonPtr button = std::make_shared<HkButton>("MyButton");
    HkButtonPtr button2 = std::make_shared<HkButton>("MyButton2");

    const glm::vec4 borderButtonColor = { 0.7647f, 0.8784f, 0.898f, 1.0f };
    const glm::vec4 hoveredButtonColor = { 0.3451f, 0.5216f, 0.6863f, 1.0f };
    const glm::vec4 baseButtonColor = { 0.2549f, 0.4471f, 0.6235f, 1.0f };

    button->getStyle()
        .setColor(baseButtonColor)
        .setVHAlignment(HkVAlignment::Center, HkHAlignment::Center)
        .setBorders(10, 10, 10, 10)
        .setMargins(10, 0, 10, 10)
        .setBorderColor(borderButtonColor)
        .setVHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.5f, .min = 50 },
            { .type = HkSizeType::PercParent, .value = 0.5f, .min = 50 });

    button2->getStyle()
        .setColor(baseButtonColor)
        .setVHAlignment(HkVAlignment::Center, HkHAlignment::Center)
        .setBorders(10, 10, 10, 10)
        .setMargins(0, 10, 10, 10)
        .setBorderColor(borderButtonColor)
        .setVHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.5f, .min = 50 },
            { .type = HkSizeType::PercParent, .value = 0.5f, .min = 50 });

    button->getEvents().setOnEnterListener([&button, &hoveredButtonColor]()
        {
            button->getStyle().setColor(hoveredButtonColor);
        });
    button2->getEvents().setOnEnterListener([&button2, &hoveredButtonColor]()
        {
            button2->getStyle().setColor(hoveredButtonColor);
        });


    button->getEvents().setOnExitListener([&button, &baseButtonColor]()
        {
            button->getStyle().setColor(baseButtonColor);
        });
    button2->getEvents().setOnExitListener([&button2, &baseButtonColor]()
        {
            button2->getStyle().setColor(baseButtonColor);
        });


    button->getEvents().setOnClickListener([&button]()
        {
            printf("Clicked on: %s\n", button->getNodeInfo().name.c_str());
        });
    button2->getEvents().setOnClickListener([&button2]()
        {
            printf("Clicked on: %s\n", button2->getNodeInfo().name.c_str());
        });


    //TODO: Resolve this "2 style" behaviour as it;s very confusing
    windowFrame->getStyle()
        .setBorderColor({ 0.6f,0.6f,0.0f,1.0f })
        .setBorders(3, 3, 3, 3);
    windowFrame->getContainerStyle()
        // .setColor({ .04f, .54f, .34f, 1.0f })
        // .setPadding(10, 10, 10, 10)
        .setOverflowAllowedXY(false)
        .setLayout(HkLayout::Horizontal)
        .setPinchConfig({ .enable = true })
        // .setRowWrapping(true)
        // .setColWrapping(true)
        .setHAlignment(HkHAlignment::Center)
        .setVAlignment(HkVAlignment::Center);

    // windowFrame->pushChildren({ button, button2 });
    // ctr10->pushChildren({ ctr6, ctr7 });
    // ctr7->pushChildren({ ctr4, ctr5 });
    // ctr5->pushChildren({ ctr12 });

    // Providing a seed value
    // srand((unsigned)time(NULL));

    int scale = 2;
    std::vector<HkNodeBasePtr> ctrs2;
    ctrs2.reserve(scale * scale);
    for (int i = 1; i <= scale; i++) // with O2 works ok 01.09.2023
    {
        for (int j = 1; j <= scale; j++)
        {
            // double r = (rand() % 255) / 255.0f;
            // double g = (rand() % 255) / 255.0f;
            // double b = (rand() % 255) / 255.0f;

            const auto btn = std::make_shared<HkButton>("MyContauner");
            btn->getStyle()
                .setColor(baseButtonColor)
                .setBorderColor(borderButtonColor)
                .setGridRowCol(i, j)
                .setBorders(10, 10, 10, 10)
                // .setMargins(10, 10, 10, 10)
                // .setVHSizeConfig({ .type = HkSizeType::FitCell }, { .type = HkSizeType::FitCell })
                .setVHSizeConfig(
                    { .type = HkSizeType::PercCell, .value = 0.5f },
                    { .type = HkSizeType::PercCell, .value = 0.5f })
                // .setVHAlignment(HkVAlignment::Top, HkHAlignment::Left);
                .setVHAlignment(HkVAlignment::Center, HkHAlignment::Center);

            btn->getEvents().setOnEnterListener([btn, &hoveredButtonColor]()
                {
                    btn->getStyle().setColor(hoveredButtonColor);
                });
            btn->getEvents().setOnExitListener([btn, &baseButtonColor]()
                {
                    btn->getStyle().setColor(baseButtonColor);
                });
            ctrs2.push_back(std::move(btn));
            // ctrs2.push_back(btn);
        }
    }

    // ctrs2[0]->getStyle()
    //     .setVHAlignment(HkVAlignment::Top, HkHAlignment::Left);
    // ctrs2[1]->getStyle()
    //     .setVHAlignment(HkVAlignment::Top, HkHAlignment::Right);
    // ctrs2[2]->getStyle()
    //     .setVHAlignment(HkVAlignment::Bottom, HkHAlignment::Left);
    // ctrs2[3]->getStyle()
    //     .setVHAlignment(HkVAlignment::Bottom, HkHAlignment::Right);

    std::vector<float> rows, cols;
    rows.assign(scale, 1.0f);
    cols.assign(scale, 1.0f);
    windowFrame->getContainerStyle()
        .setLayout(HkLayout::Grid)
        // .setOverflowAllowedXY(true)
        // .setPadding(30, 30, 30, 30)
        .setGridConfig(
            HkGridConfig{
                .cols{cols},
                .rows{rows}
            });
    windowFrame->pushChildren(ctrs2);
    // windowFrame->printTree();
    // HkImageViewPtr imgView = std::make_shared<HkImageView>("MyImgView");
    // HkImageViewPtr imgView2 = std::make_shared<HkImageView>("MyImgView2");
    // HkImageViewPtr imgView3 = std::make_shared<HkImageView>("MyImgView3");
    // HkImageViewPtr imgView4 = std::make_shared<HkImageView>("MyImgView4");
    // imgView->loadImage("/home/hekapoo/container.jpg");
    // imgView2->loadImage("/home/hekapoo/container.jpg");
    // imgView3->loadImage("/home/hekapoo/container.jpg");
    // imgView4->loadImage("/home/hekapoo/Downloads/fbi_wp.jpg");
    // imgView->setSize({ 1920,1080 });
    // windowFrame->getStyle().setOverflowAllowedXY(false);

    // windowFrame->pushChildren({ imgView, imgView2, imgView3, imgView4 });
    // windowFrame->pushChildren({ ctr, ctr4, ctr5, ctr6 });

    // sceneWindow1->makeContextNotCurrent();

    // sceneWindow1->setBackgroundImage("/home/hekapoo/Downloads/fbi_wp.jpg");

    // sceneWindow2->makeContextCurrent();

    HkWindowFramePtr windowFrame2 = std::make_shared<HkWindowFrame>("MyWindowFrame2");
    windowFrame2->setWindowMode(HkWindowFrameMode::Grabbable);
    // sceneWindow1->addSubWindow(windowFrame2); //NOTE: Needs to be added before adding any children
    // sceneWindow2->addSubWindow(windowFrame2); //NOTE: Needs to be added before adding any children

    windowFrame2->getContainerStyle().setOverflowAllowedXY(true)
        .setLayout(HkLayout::Horizontal)
        .setPinchConfig({ .enable = true })
        .setHAlignment(HkHAlignment::Left)
        .setVAlignment(HkVAlignment::Center);
    windowFrame2->setTitle("Second window");

    HkContainerPtr ctr2 = std::make_shared<HkContainer>("MyContainer2");
    HkContainerPtr ctr3 = std::make_shared<HkContainer>("MyContainer3");


    ctr2->getStyle().setColor({ 1.0f,0.4f,0.5f, 1.0f });
    ctr3->getStyle().setColor({ 0.4f,0.5f,0.6f, 1.0f });

    // ctr2->setSize({ 300, 350 });
    // ctr3->setSize({ 400, 450 });

    std::vector<HkNodeBasePtr> ctrs3;
    ctrs3.reserve(500);
    for (int i = 0;i < 500;i++) // with O2 works ok 01.09.2023
    {
        const auto& ct = std::make_shared<HkContainer>("MyContauner");
        ct->getStyle().setColor(i % 2 == 0 ? glm::vec4{ 0.75f, 0.75f, 0.75f,1.0f } : glm::vec4{ 1.0f, 1.0f, 1.0f,1.0f });
        // ct->setSize({ 20, 20 });
        ctrs3.push_back(std::move(ct));
    }

    // windowFrame2->pushChildren({ ctr2, ctr3 });
    windowFrame2->pushChildren(ctrs3);

    // HkWindowManagerPtr sceneWindow3 = std::make_shared<HkWindowManager>("MyWindowManager3",
    //     HkWindowManager::HkWindowConfig{ .width = 800, .height = 600 });
    // HkAppManager::get().addWindow(sceneWindow3);

    // sceneWindow3->makeContextCurrent();

    HkWindowFramePtr windowFrame3 = std::make_shared<HkWindowFrame>("MyWindowFrame2");
    // windowFrame3->setWindowMode(HkWindowFrameMode::Grabbable);
    // sceneWindow3->addSubWindow(windowFrame3); //NOTE: Needs to be added before adding any children
    // sceneWindow1->addSubWindow(windowFrame3); //NOTE: Needs to be added before adding any children

    windowFrame3->getStyle().setOverflowAllowedXY(true)
        .setLayout(HkLayout::Horizontal)
        .setHAlignment(HkHAlignment::Right)
        .setVAlignment(HkVAlignment::Center);

    HkContainerPtr ctr15 = std::make_shared<HkContainer>("MyContainer15");
    HkContainerPtr ctr16 = std::make_shared<HkContainer>("MyContainer16");

    // ctr5->getStyle().setColor({ 0.0f,0.1f,0.2f });
    // ctr6->getStyle().setColor({ 1.0f,0.4f,0.5f });

    // ctr5->setSize({ 330, 450 });
    // ctr6->setSize({ 400, 450 });

    // std::vector<HkNodeBasePtr> ctrs4;
    // ctrs4.reserve(5'000);
    // for (int i = 0;i < 5'000;i++) // with O2 works ok 01.09.2023
    // {
    //     const auto& ct = std::make_shared<HkContainer>("MyContauner");
    //     ct->getStyle().setColor(i % 2 == 0 ? glm::vec3{ 0.75f, 0.75f, 0.75f } : glm::vec3{ 1.0f, 1.0f, 1.0f });
    //     ct->setSize({ 20, 20 });
    //     ctrs4.push_back(std::move(ct));
    // }

    // windowFrame3->getStyle().setOverflowAllowedXY(true)
    //     .setLayout(HkLayout::Horizontal)
    //     .setHAlignment(HkHAlignment::Left)
    //     .setVAlignment(HkVAlignment::Center);

    windowFrame3->pushChildren({ ctr15, ctr16 });
    // windowFrame3->pushChildren(ctrs4);


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
}
