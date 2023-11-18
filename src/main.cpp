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
    sceneWindow1->makeContextCurrent();

    HkWindowFramePtr windowFrame = std::make_shared<HkWindowFrame>("MyWindowFrame");
    windowFrame->setWindowMode(HkWindowFrameMode::FullScreenFixed);
    sceneWindow1->addSubWindow(windowFrame); //NOTE: Needs to be added before adding any children

    windowFrame->getStyle().setOverflowAllowedXY(false)
        .setLayout(HkLayout::Horizontal)
        .setPinchConfig({ .enable = true })
        .setRowWrapping(false)
        .setHAlignment(HkHAlignment::Center)
        .setVAlignment(HkVAlignment::Center);

    windowFrame->setSize({ 1280 , 720 });

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
    ctr->getStyle().setColor({ 1.0f,0.0f,1.0f })
        .setLayout(HkLayout::VPinch)
        .setHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.5f, .min = 300 })
        .setPinchConfig({ .enable = true, .alwaysOn = true, .allowRight = true, .allowTop = true, .allowBottom = true });

    ctr4->getStyle().setColor({ 1.0f,0.0f,0.0f })
        .setLayout(HkLayout::VPinch)
        // .setLayout(HkLayout::Vertical)
        .setHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.25f, .min = 300 })
        .setPinchConfig({ .enable = true,.alwaysOn = true, .allowRight = true, .allowTop = true, .allowBottom = true });

    ctr5->getStyle().setColor({ 0.3f,0.5f,0.0f })
        .setLayout(HkLayout::VPinch)
        .setHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.25f, .min = 300 })
        .setPinchConfig({ .enable = true, .alwaysOn = true,.allowTop = true, .allowBottom = true });

    //ctr4 children
    ctr6->getStyle().setColor({ 0.0f,0.0f,1.0f })
        .setLayout(HkLayout::Horizontal)
        .setVSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.75f, .min = 300 })
        .setPinchConfig({ .enable = true,.alwaysOn = true, .allowLeft = true, .allowRight = true, .allowBottom = true });

    ctr7->getStyle().setColor({ 0.3f,0.7f,0.0f })
        .setLayout(HkLayout::Horizontal)
        .setOverflowAllowedXY(true)
        .setRowWrapping(true)
        .setVSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.25f, .min = 300 })
        .setPinchConfig({ .enable = true,.alwaysOn = true, .allowLeft = true, .allowRight = true });
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



    ctr10->getStyle().setColor({ 0.0f,0.7f,1.0f })
        .setVHAlignment(HkVAlignment::Top, HkHAlignment::Left)
        .setVSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.5f, .min = 50 })
        .setPinchConfig({ .enable = true,.alwaysOn = true, .allowLeft = true, .allowBottom = true });

    ctr11->getStyle().setColor({ 0.4f,0.7f,0.0f })
        .setVHAlignment(HkVAlignment::Top, HkHAlignment::Left)
        .setVSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.25f, .min = 50 })
        .setPinchConfig({ .enable = true, .alwaysOn = true,.allowLeft = true, .allowBottom = true });

    ctr12->getStyle().setColor({ 0.0f,0.7f,1.0f })
        .setVHAlignment(HkVAlignment::Top, HkHAlignment::Left)
        .setVSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.25f, .min = 50 })
        .setPinchConfig({ .enable = true, .alwaysOn = true,.allowLeft = true, .allowBottom = true });
    {

        // ctr13->getStyle().setColor({ 0.1f,0.7f,0.0f })
        //     .setLayout(HkLayout::Vertical)
        //     .setPinchConfig({ .allowRight = true, .allowTop = true });
        // ctr14->getStyle().setColor({ 0.5f,0.7f,1.0f })
        //     .setLayout(HkLayout::Vertical)
        //     .setPinchConfig({ .allowLeft = true, .allowTop = true });

        // ctr->pushChildren({ ctr4, ctr5, ctr6 });
        // bool x = false;
        // ctr->getEvents().setOnClickListener([&windowFrame, &ctr6, &ctr5]() {
        //     ctr5->getStyle().setPinchConfig({ .allowLeft = true, .allowRight = true });
        //     windowFrame->pushChildren({ ctr6 });
        //     });
    }

    HkLabelPtr label = std::make_shared<HkLabel>("Label1");

    label->getTextStyle()
        .setFontPath("assets/fonts/LiberationSerif-Regular.ttf")
        .setRenderMethod(HkTextUserConfig::HkTextRenderMethod::BITMAP)
        .setFontSize(32)
        // .setWrapAtWord(true)
        // .setTextAngle(45)
        // .setTextAngle(0)
        .setFontColor(glm::vec3(1.0f));

    // label->getStyle()
    //     .setVHSizeConfig(
    //         { .type = HkSizeType::PercParent, .value = 1.0f },
    //         { .type = HkSizeType::PercParent, .value = 1.0f });

    label->getStyle()
        .setVHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.5f },
            { .type = HkSizeType::PercParent, .value = 0.5f });

    // uint32_t a = 60;
    std::string smallText = "AFG auick brown Fox jumps over";

    label->getEvents().setOnClickListener([&label, &smallText]()
        {
            // printf("str: %s\n", smallText.c_str());
            label->setText(smallText + smallText);
        });

    // label->setText("Pala gateste biban NO CAPS NO CAPS CAPS CAPS CAPS"
    //     "a ton more text look mom there's a lot of?./ text in here #$%%^");
    std::string text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Duis faucibus, erat ut suscipit euismod, lectus est egestas purus, a porta orci ipsum vitae neque. Aenean justo velit, vestibulum vel volutpat nec, dictum eget nisl. Pellentesque a dapibus ex. Sed sed odio blandit, cursus libero et, vehicula lacus. Donec aliquam mi mollis odio luctus, at dignissim purus auctor. Morbi id enim eget tellus rhoncus lobortis. Morbi id quam turpis.\n\nUt quis purus id metus dictum facilisis. Praesent purus enim, ornare at augue non, pretium ultricies nisl. Morbi ultrices rutrum augue, eget egestas odio finibus quis. Pellentesque venenatis, dolor nec eleifend porta, diam diam faucibus arcu, at molestie libero quam id diam. Integer aliquam sem vel erat lacinia vulputate. Cras eget mauris sit amet nunc tempus fermentum. Quisque fermentum imperdiet justo, eget blandit lacus ultricies in. Donec nec suscipit lorem.\n\nPellentesque varius ligula ut eros venenatis, non tristique purus efficitur. Aliquam erat volutpat. In ex augue, mollis sit amet vulputate at, porta eu orci. Aenean et sodales ante. Quisque tristique tincidunt risus at vulputate. Nullam lacinia purus et venenatis scelerisque. Donec sed porta massa. Cras lacinia eros nec sodales aliquam. Donec vel neque ac mi sodales luctus. Phasellus semper nisl metus, id vulputate lectus feugiat non. Mauris nec nisi gravida, ornare mi nec, pulvinar erat. Nullam id quam id ex placerat posuere.\n\nSed tempus tempus odio, non porttitor diam interdum id. Suspendisse vestibulum ipsum sit amet mauris rutrum, ornare fringilla lectus vestibulum. Sed sed aliquam nisi, id rhoncus felis. Mauris fermentum massa velit. Proin vitae tincidunt ante, quis facilisis sem. Vestibulum efficitur, elit eget feugiat fringilla, sapien mi dapibus ex, quis lobortis nisi nulla ac nibh. Praesent luctus mauris vehicula urna auctor lacinia. Maecenas ac felis ex. Suspendisse sed rhoncus nulla, ac egestas felis. Donec euismod at arcu sed laoreet. Vestibulum venenatis luctus ullamcorper. Quisque interdum gravida arcu in elementum.\n\nQuisque eu lobortis metus. Cras imperdiet diam ut sem euismod, sed gravida elit accumsan. Integer a erat dui. Aliquam pharetra pharetra faucibus. Integer ligula enim, convallis a imperdiet eget, sollicitudin at sapien. Aliquam sed justo vel neque eleifend ullamcorper. Nulla consectetur porttitor odio, sed ornare erat blandit quis. Cras ullamcorper magna sit amet diam tristique aliquet. Nulla facilisi. Sed purus libero, laoreet nec nibh ac, sodales commodo mi. In id venenatis ex. Fusce tellus augue, vestibulum ut pretium eu, facilisis eget nunc. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Interdum et malesuada fames ac ante ipsum primis in faucibus.\n\nCurabitur posuere metus ac dolor porttitor, quis commodo nunc varius. Vestibulum consequat, metus non venenatis ullamcorper, odio nulla commodo erat, vitae iaculis mauris lacus id nunc. Curabitur urna magna, viverra vel tellus ac, accumsan condimentum sem. Nunc eu dignissim nibh. Sed dignissim, arcu et tristique sagittis, lectus urna cursus ligula, id tincidunt augue risus sit amet est. Nullam lobortis fermentum metus, nec placerat mauris vestibulum sollicitudin. Etiam ut accumsan arcu, sit amet tempus quam. Maecenas finibus nec turpis a vulputate. Nulla sed urna enim. Aliquam vestibulum gravida erat.";
    // std::string smallText = "The quick brown fox jumps over the lazy dog";
    // std::string smallText = "o";
    // std::string smallText = "o";
    // std::string smallText = "AFG_auick_brown_Fox_jumps_over";
    // std::string smallText = "AFG auick brown Fox jumps over";
    // std::string smallText = "ooo_ooo_ooo_oo_oo_oooo";
    // std::string smallText = "A";

    // label->setText(text + text + text + text + text + text + text + text + text + text + text);
    // label->setText(text + text + text + text + text + text);
    // label->setText("AA");
    label->setText(std::string(text.begin(), text.begin() + 400));
    // label->setText(text + text + text);
    // label->setText(smallText);
    // label->setText(smallText + smallText + smallText + smallText + smallText + smallText);
    // label->setText(smallText + smallText + smallText + smallText + smallText + smallText + smallText + smallText + smallText + smallText + smallText + smallText);
    // label->setText("o");
    // label->setText("8");
    // label->setText("jfsfgegwegweSGgergrewgGGEF");
    // windowFrame->pushChildren({ ctr, ctr4, ctr5, ctr6 });
    // windowFrame->pushChildren({ ctr, ctr4, ctr5 });
    windowFrame->pushChildren({ label });

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
                .setColor((i + j) % 2 == 0 ? glm::vec3{ r,g,b } : glm::vec3{ r,g,b })
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
    windowFrame2->setWindowMode(HkWindowFrameMode::Grabbable);
    // sceneWindow1->addSubWindow(windowFrame2); //NOTE: Needs to be added before adding any children
    // sceneWindow2->addSubWindow(windowFrame2); //NOTE: Needs to be added before adding any children

    windowFrame2->getStyle().setOverflowAllowedXY(true)
        .setLayout(HkLayout::Horizontal)
        .setPinchConfig({ .enable = true })
        .setHAlignment(HkHAlignment::Left)
        .setVAlignment(HkVAlignment::Center);

    HkContainerPtr ctr2 = std::make_shared<HkContainer>("MyContainer2");
    HkContainerPtr ctr3 = std::make_shared<HkContainer>("MyContainer3");


    ctr2->getStyle().setColor({ 1.0f,0.4f,0.5f });
    ctr3->getStyle().setColor({ 0.4f,0.5f,0.6f });

    // ctr2->setSize({ 300, 350 });
    // ctr3->setSize({ 400, 450 });

    std::vector<HkNodeBasePtr> ctrs3;
    ctrs3.reserve(500);
    for (int i = 0;i < 500;i++) // with O2 works ok 01.09.2023
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
