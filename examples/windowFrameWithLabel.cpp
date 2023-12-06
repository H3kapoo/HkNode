#include "../src/management/HkAppManager.hpp"
#include "../src/management/HkSceneManagement.hpp"
#include "../src/elements/HkWindowFrame.hpp"
#include "../src/elements/HkLabel.hpp"

using namespace hkui;

int main()
{
    /* Setup manager. Init OpenGL & GLFW */
    if (!HkAppManager::get().setup()) { return -1; }

    /* Create window. Master flagged window is the one which upon closure, ends the app */
    HkWindowManagerPtr window = std::make_shared<HkWindowManager>("OsWindowManager",
        HkWindowManager::HkWindowConfig{ .width = 1920, .height = 1080, .isMaster = true });

    /* Add window to the list of app managed windows */
    HkAppManager::get().addWindow(window);

    /* Before action any window, make the window the current one. Any subseqent OpenGL calls will act on this
       window alone. Optional if only one Window is created, context is made current by default */
    window->makeContextCurrent();

    /* Create frame into which other UI elements will reside */
    HkWindowFramePtr windowFrame = std::make_shared<HkWindowFrame>("WindowFrame");

    /* Tell the Window to register a new root, a new sub WindowFrame (we can have multiple) */
    window->addSubWindow(windowFrame);

    /* Configure the window to be grabbable */
    windowFrame->setWindowMode(HkWindowFrameMode::Grabbable);

    /* Minimal size setting & style config so WindowFrame is pinchable and scalable using the mouse.
       Set also grabbable window title. Default windowFrame orientation is Horizontal */
    windowFrame->getContainerStyle().setPinchConfig({ .enable = true });
    windowFrame->setSize({ 1280 , 720 });
    windowFrame->setTitle("My Window Title");

    HkLabelPtr label = std::make_shared<HkLabel>("MyLabel");

    /* Set different options on label text */
    label->getTextStyle()
        .setFontPath("assets/fonts/LiberationSerif-Regular.ttf")
        .setRenderMethod(HkTextUserConfig::HkTextRenderMethod::BITMAP)
        .setFontSize(24)
        .setWrapAtWord(true)
        .setFontColor(glm::vec4(1.0f))
        .setTextVHAlign(HkTextUserConfig::HkTextVAlign::Center, HkTextUserConfig::HkTextHAlign::Center);

    /* Set constraint style aka where the element will be positioned and how.
       In this example, MyLabel will be 50% of it's parent's width and height at any time */
    label->getStyle()
        .setVHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.5f },
            { .type = HkSizeType::PercParent, .value = 0.5f });


    /* Assign a text to the label to display */
    std::string myText = "The quick brown Fox jumps over the lazy dog";
    label->setText(myText);

    /* Finally, push label to be the child of windowFrame*/
    windowFrame->pushChildren({ label });

    /*Blocking from here on until window exit*/
    HkAppManager::get().runLoop();
    HkAppManager::get().teardown();
    return 0;
}
