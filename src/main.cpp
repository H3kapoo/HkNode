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
        HkWindowManager::HkWindowConfig{ .width = 1920, .height = 1080, .isMaster = true });
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
    windowFrame->setWindowMode(HkWindowFrameMode::Grabbable);
    sceneWindow1->addSubWindow(windowFrame); //NOTE: Needs to be added before adding any children

    windowFrame->getStyle().setOverflowAllowedXY(false)
        .setLayout(HkLayout::HPinch)
        .setPinchConfig({ .enable = true })
        .setRowWrapping(false)
        .setHAlignment(HkHAlignment::Center)
        .setVAlignment(HkVAlignment::Center);

    windowFrame->setSize({ 1280, 720 });
    windowFrame->setPos({ 1920 / 2 - 200 , 1080 / 2 - 100 });

    HkContainerPtr ctr = std::make_shared<HkContainer>("MyContainer");
    HkContainerPtr ctr4 = std::make_shared<HkContainer>("MyContainer4");
    HkContainerPtr ctr5 = std::make_shared<HkContainer>("MyContainer5");
    HkContainerPtr ctr6 = std::make_shared<HkContainer>("MyContainer6");
    HkContainerPtr ctr7 = std::make_shared<HkContainer>("MyContainer7");
    HkContainerPtr ctr8 = std::make_shared<HkContainer>("MyContainer8");

    HkContainerPtr ctr9 = std::make_shared<HkContainer>("MyContainer9");
    HkContainerPtr ctr10 = std::make_shared<HkContainer>("MyContainer10");
    HkContainerPtr ctr11 = std::make_shared<HkContainer>("MyContainer11");
    HkContainerPtr ctr12 = std::make_shared<HkContainer>("MyContainer12");
    HkContainerPtr ctr13 = std::make_shared<HkContainer>("MyContainer13");
    HkContainerPtr ctr14 = std::make_shared<HkContainer>("MyContainer14");

    // label->setText("Pala gateste biban NO CAPS and this is a very bussy string NO CAPS mr.s white carbonara mmmmm....cuvinte GgGPBpbp");

    // WF children
    ctr->getStyle().setColor({ 1.0f,0.0f,1.0f ,1.0f })
        .setLayout(HkLayout::VPinch)
        .setHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.5f, .min = 300 })
        .setPinchConfig({ .enable = true, .alwaysOn = false, .allowRight = true, .allowTop = true, .allowBottom = true });

    ctr4->getStyle().setColor({ 1.0f,0.0f,0.0f ,1.0f })
        .setLayout(HkLayout::Vertical)
        // .setLayout(HkLayout::Vertical)
        .setVHAlignment(HkVAlignment::Center, HkHAlignment::Center)
        .setHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.25f, .min = 300 })
        .setPinchConfig({ .enable = true,.alwaysOn = false, .allowRight = true, .allowTop = true, .allowBottom = true });

    ctr5->getStyle().setColor({ 0.3f,0.5f,0.0f ,1.0f })
        .setLayout(HkLayout::VPinch)
        .setHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.25f, .min = 300 })
        .setPinchConfig({ .enable = true, .alwaysOn = false,.allowTop = true, .allowBottom = true });

    // //ctr4 children
    // ctr6->getStyle().setColor({ 0.0f,0.0f,1.0f,1.0f })
    //     .setLayout(HkLayout::Horizontal)
    //     .setVSizeConfig(
    //         { .type = HkSizeType::PercParent, .value = 0.75f, .min = 300 })
    //     .setPinchConfig({ .enable = true,.alwaysOn = false, .allowLeft = true, .allowRight = true, .allowBottom = true });

    // ctr7->getStyle().setColor({ 0.3f,0.7f,0.0f,1.0f })
    //     .setLayout(HkLayout::Horizontal)
    //     .setOverflowAllowedXY(true)
    //     .setRowWrapping(true)
    //     .setVSizeConfig(
    //         { .type = HkSizeType::PercParent, .value = 0.25f, .min = 300 })
    //     .setPinchConfig({ .enable = true,.alwaysOn = false, .allowLeft = true, .allowRight = true });
    //

    // ctr8->getStyle().setColor({ 0.5f,0.4f,1.0f })
    //     .setVHSizeConfig(
    //         { .type = HkSizeType::PercParent, .value = 0.5f },
    //         { .type = HkSizeType::FitParent })
    //     .setPinchConfig({ .enable = true, .allowLeft = true, .allowBottom = true })
    //     .setBottomMargin(15);

    // ctr9->getStyle().setColor({ 0.0f,0.4f,1.0f })
    //     .setVHSizeConfig(
    //         { .type = HkSizeType::PercParent, .value = 0.5f },
    //         { .type = HkSizeType::FitParent })
    //     .setPinchConfig({ .enable = true, .allowLeft = true });



    ctr10->getStyle().setColor({ 0.0f,0.7f,1.0f, 1.0f })
        .setVHAlignment(HkVAlignment::Top, HkHAlignment::Left)
        .setVSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.5f, .min = 50 })
        .setPinchConfig({ .enable = true, .alwaysOn = false, .allowLeft = true, .allowBottom = true });

    ctr11->getStyle().setColor({ 0.4f,0.7f,0.0f, 1.0f })
        .setVHAlignment(HkVAlignment::Top, HkHAlignment::Left)
        .setVSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.25f, .min = 50 })
        .setPinchConfig({ .enable = true, .alwaysOn = false, .allowLeft = true, .allowBottom = true });

    ctr12->getStyle().setColor({ 0.0f,0.7f,1.0f, 1.0f })
        .setVHAlignment(HkVAlignment::Top, HkHAlignment::Left)
        .setVSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.25f, .min = 50 })
        .setPinchConfig({ .enable = true, .alwaysOn = false, .allowLeft = true, .allowBottom = true });


    HkLabelPtr label = std::make_shared<HkLabel>("Label1");

    label->getTextStyle()
        .setFontPath("assets/fonts/LiberationSerif-Regular.ttf")
        .setRenderMethod(HkTextUserConfig::HkTextRenderMethod::BITMAP)
        .setFontSize(24)
        .setWrapAtWord(true)
        .setFontColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
        .setLineSpread(10)
        .setTextVHAlign(HkTextUserConfig::HkTextVAlign::Center, HkTextUserConfig::HkTextHAlign::Center);

    // label->getStyle()
    //     .setVHSizeConfig(
    //         { .type = HkSizeType::PercParent, .value = 1.0f },
    //         { .type = HkSizeType::PercParent, .value = 1.0f });

    label->getStyle()
        .setVHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.5f },
            { .type = HkSizeType::PercParent, .value = 0.5f });

    std::string smallText = "AFG auick brown Fox jumps over";

    label->setText(smallText + smallText + smallText + smallText + smallText);

    windowFrame->pushChildren({ ctr, ctr4, ctr5 });
    // windowFrame->pushChildren({ label });

    // ctr->pushChildren({ ctr10, ctr11, ctr12 });
    ctr4->pushChildren({ label });
    // ctr4->pushChildren({ ctr6, ctr7 });
    // ctr5->pushChildren({ label });

    // // ctr7->pushChildren({ ctr11,ctr12 });
    // ctr8->pushChildren({ ctr13,ctr14 });

    // ctr13->pushChildren({ ctr9,ctr10 });
    // windowFrame->pushChildren({ ctr });

    // Providing a seed value
    srand((unsigned)time(NULL));

    int scale = 4;
    std::vector<HkNodeBasePtr> ctrs2;
    ctrs2.reserve(scale * scale);
    for (int i = 0;i < scale; i++) // with O2 works ok 01.09.2023
    {
        for (int j = 0;j < scale; j++)
        {
            double r = (rand() % 255) / 255.0f;
            double g = (rand() % 255) / 255.0f;
            double b = (rand() % 255) / 255.0f;

            const auto& ct = std::make_shared<HkContainer>("MyContauner");
            ct->getStyle()
                .setColor((i + j) % 2 == 0 ? glm::vec4{ r,g,b, 1.0f } : glm::vec4{ r,g,b, 1.0f })
                // .setGridRowCol(i + 1, j + 1)
                .setVHSizeConfig({ .value = 50 }, { .value = 100 })
                // .setVHSizeConfig({ .type = HkSizeType::FitCell }, { .type = HkSizeType::FitCell })
                .setVHAlignment(HkVAlignment::Center, HkHAlignment::Center);

            ctrs2.push_back(std::move(ct));
        }
    }

    // std::vector<float> rows, cols;
    // rows.assign(scale, 1.0f);
    // cols.assign(scale, 1.0f);
    // ctr7->getStyle()
    //     .setLayout(HkLayout::Grid)
    //     .setGridConfig(
    //         HkGridConfig{
    //             .cols{cols},
    //             .rows{rows}
    //         });
    ctr7->pushChildren(ctrs2);
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
    // windowFrame2->setWindowMode(HkWindowFrameMode::Grabbable);
    // sceneWindow1->addSubWindow(windowFrame2); //NOTE: Needs to be added before adding any children
    // sceneWindow2->addSubWindow(windowFrame2); //NOTE: Needs to be added before adding any children

    windowFrame2->getStyle().setOverflowAllowedXY(true)
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
    windowFrame3->setWindowMode(HkWindowFrameMode::Grabbable);
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
