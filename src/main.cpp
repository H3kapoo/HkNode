#include <cstdlib>

#include "APIGate/GlfwGlewGate.hpp"
#include "management/HkAppManager.hpp"
#include "management/HkSceneManagement.hpp"
#include "elements/HkWindowFrame.hpp"
#include "elements/HkContainer.hpp"
#include "elements/HkButton.hpp"

using namespace hkui;

int main()
{
    //TODO: We need the dummy context used for loading glfw to also load all possible VAOs and assets
    // but by doing this, we lose the ability to load things at runtime I think..or not?
    if (!HkAppManager::get().setup()) { return -1; }

    //TODO: Make it the responsability of the window to generate VAO and shader?
    // we need to make it order agnostic
    HkWindowManagerPtr sceneWindow1 = std::make_shared<HkWindowManager>("MyWindowManager",
        HkWindowManager::HkWindowConfig{ .width = 1280, .height = 720, .isMaster = true });
    //TODO: There is a bug with 2 windows while animating the windowFrame movement
    // HkWindowManagerPtr sceneWindow2 = std::make_shared<HkWindowManager>("MyWindowManager2",
    //     HkWindowManager::HkWindowConfig{ .width = 800, .height = 600 });

    HkAppManager::get().addWindow(sceneWindow1);
    // HkAppManager::get().addWindow(sceneWindow2);
    sceneWindow1->makeContextCurrent();

    HkWindowFramePtr windowFrame = std::make_shared<HkWindowFrame>("MyWindowFrame");
    windowFrame->setWindowMode(HkWindowFrameMode::FullScreenFixed);
    sceneWindow1->addSubWindow(windowFrame); //NOTE: Needs to be added before adding any children

    windowFrame->getStyle().setOverflowAllowedXY(false)
        .setLayout(HkLayout::Grid)
        .setRowWrapping(false)
        .setGridConfig(HkGridConfig{ .cols{1.0f, 1.0f}, .rows{1.0f, 1.0f} })
        .setHAlignment(HkHAlignment::Left)
        .setVAlignment(HkVAlignment::Center);

    windowFrame->setPos({ 1280 * 0.25, 720 * 0.25 });
    windowFrame->setSize({ 1280 , 720 });

    HkContainerPtr ctr = std::make_shared<HkContainer>("MyContainer");
    HkContainerPtr ctr4 = std::make_shared<HkContainer>("MyContainer4");
    HkContainerPtr ctr5 = std::make_shared<HkContainer>("MyContainer5");
    HkContainerPtr ctr6 = std::make_shared<HkContainer>("MyContainer6");
    HkContainerPtr ctr7 = std::make_shared<HkContainer>("MyContainer7");
    HkContainerPtr ctr8 = std::make_shared<HkContainer>("MyContainer8");

    ctr->getStyle().setColor({ 1.0f,1.0f,1.0f })
        .setGridRowCol(1, 1)
        .setVHAlignment(HkVAlignment::Top, HkHAlignment::Left)
        .setVHSizeConfig(
            { .type = HkSizeType::FitCell , .max = 300 },
            { .type = HkSizeType::Percentage, .value = 0.2f, .max = 200 })
        .setMargins(0, 0, 0, 0);

    ctr4->getStyle().setColor({ 1.0f,0.0f,0.0f })
        .setGridRowCol(1, 2)
        .setVHAlignment(HkVAlignment::Top, HkHAlignment::Left)
        .setVHSizeConfig(
            { .type = HkSizeType::FitCell },
            { .type = HkSizeType::FitCell })
        .setMargins(0, 0, 0, 0);

    ctr5->getStyle().setColor({ 0.3f,0.5f,0.0f })
        .setGridRowCol(2, 1)
        .setVHAlignment(HkVAlignment::Top, HkHAlignment::Left)
        .setVHSizeConfig(
            { .type = HkSizeType::FitCell },
            { .type = HkSizeType::FitCell })
        .setMargins(0, 0, 0, 0);

    ctr6->getStyle().setColor({ 0.0f,0.0f,1.0f })
        .setGridRowCol(2, 2)
        .setVHAlignment(HkVAlignment::Top, HkHAlignment::Left)
        .setVHSizeConfig(
            { .type = HkSizeType::FitCell },
            { .type = HkSizeType::FitCell })
        .setMargins(0, 0, 0, 0);

    // ctr4->pushChildren({ ctr5, ctr8 });


    windowFrame->pushChildren({ ctr, ctr4, ctr5, ctr6 });

    // // Providing a seed value
    // srand((unsigned)time(NULL));

    // int scale = 10;
    // std::vector<HkNodeBasePtr> ctrs2;
    // ctrs2.reserve(scale * scale);
    // for (int i = 0;i < scale; i++) // with O2 works ok 01.09.2023
    // {
    //     for (int j = 0;j < scale; j++)
    //     {
    //         double r = (rand() % 255) / 255.0f;
    //         double g = (rand() % 255) / 255.0f;
    //         double b = (rand() % 255) / 255.0f;

    //         const auto& ct = std::make_shared<HkContainer>("MyContauner");
    //         ct->getStyle()
    //             .setColor((i + j) % 2 == 0 ? glm::vec3{ r,g,b } : glm::vec3{ r,g,b })
    //             .setGridRowCol(i + 1, j + 1)
    //             .setVHSizeConfig({ .value = 10 }, { .value = 10 })
    //             .setVHAlignment(HkVAlignment::Center, HkHAlignment::Center);

    //         ctrs2.push_back(std::move(ct));
    //     }
    // }

    // std::vector<float> rows, cols;
    // rows.assign(scale, 1.0f);
    // cols.assign(scale, 1.0f);
    // ctr5->getStyle()
    //     .setLayout(HkLayout::Grid)
    //     .setGridConfig(
    //         HkGridConfig{
    //             .cols{cols},
    //             .rows{rows}
    //         });
    // ctr5->pushChildren(ctrs2);
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

    windowFrame2->getStyle().setOverflowAllowedXY(true)
        .setLayout(HkLayout::Horizontal)
        .setHAlignment(HkHAlignment::Left)
        .setVAlignment(HkVAlignment::Center);

    HkContainerPtr ctr2 = std::make_shared<HkContainer>("MyContainer2");
    HkContainerPtr ctr3 = std::make_shared<HkContainer>("MyContainer3");


    ctr2->getStyle().setColor({ 1.0f,0.4f,0.5f });
    ctr3->getStyle().setColor({ 0.4f,0.5f,0.6f });

    // ctr2->setSize({ 300, 350 });
    // ctr3->setSize({ 400, 450 });

    std::vector<HkNodeBasePtr> ctrs3;
    ctrs3.reserve(5'000);
    for (int i = 0;i < 5'000;i++) // with O2 works ok 01.09.2023
    {
        const auto& ct = std::make_shared<HkContainer>("MyContauner");
        ct->getStyle().setColor(i % 2 == 0 ? glm::vec3{ 0.75f, 0.75f, 0.75f } : glm::vec3{ 1.0f, 1.0f, 1.0f });
        // ct->setSize({ 20, 20 });
        ctrs3.push_back(std::move(ct));
    }

    // windowFrame2->pushChildren({ ctr2, ctr3 });
    windowFrame2->pushChildren(ctrs3);

    // HkWindowManagerPtr sceneWindow3 = std::make_shared<HkWindowManager>("MyWindowManager3",
    //     HkWindowManager::HkWindowConfig{ .width = 800, .height = 600 });
    // HkAppManager::get().addWindow(sceneWindow3);

    // sceneWindow3->makeContextCurrent();

    // HkWindowFramePtr windowFrame3 = std::make_shared<HkWindowFrame>("MyWindowFrame2");
    // windowFrame3->setWindowMode(HkWindowFrameMode::Grabbable);
    // // sceneWindow3->addSubWindow(windowFrame3); //NOTE: Needs to be added before adding any children
    // // sceneWindow1->addSubWindow(windowFrame3); //NOTE: Needs to be added before adding any children

    // windowFrame3->getStyle().setOverflowAllowedXY(true)
    //     .setLayout(HkLayout::Horizontal)
    //     .setHAlignment(HkHAlignment::Right)
    //     .setVAlignment(HkVAlignment::Center);

    // ctr5 = std::make_shared<HkContainer>("MyContainer5");
    // ctr6 = std::make_shared<HkContainer>("MyContainer6");

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

    // // windowFrame3->pushChildren({ ctr5, ctr6 });
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
