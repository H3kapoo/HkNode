#include "../src/management/HkAppManager.hpp"
#include "../src/management/HkSceneManagement.hpp"
#include "../src/elements/HkWindowFrame.hpp"

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

    /* Create some dummy containers */
    HkContainerPtr container1 = std::make_shared<HkContainer>("MyContainer1");
    HkContainerPtr container2 = std::make_shared<HkContainer>("MyContainer2");
    HkContainerPtr container3 = std::make_shared<HkContainer>("MyContainer3");
    HkContainerPtr container4 = std::make_shared<HkContainer>("MyContainer4");
    HkContainerPtr container5 = std::make_shared<HkContainer>("MyContainer5");
    HkContainerPtr container6 = std::make_shared<HkContainer>("MyContainer6");
    HkContainerPtr container7 = std::make_shared<HkContainer>("MyContainer7");
    HkContainerPtr container8 = std::make_shared<HkContainer>("MyContainer8");

    /* Set size. Setting layout to HPinch will make all child HkContainers pinchable horizontally.
       Do not add any other type of children directly to WindowFrame. Setting pinch config to
       'enabled' on a windowFrame activates window resize handles. */
    windowFrame->getContainerStyle()
        .setLayout(HkLayout::HPinch)
        .setPinchConfig({ .enable = true });
    windowFrame->setSize({ 1280 , 720 });

    /*
        Final hierarchy:
    */
    /* We enable horizontal pinching for this container, meaning children containers will be
       pinchable also. As a standalone container, this one will be pinchable from the left and
       from the right only. Additionally, it will ocupy 25% of total parent's width (minimum 30px)
       and the rest of the parent's children containers, the rest of 75%. This must exactly add to 100%. */
    container4->getStyle()
        .setLayout(HkLayout::HPinch)
        .setColor({ 0.3f, 0.7f, 0.0f, 1.0f })
        .setPinchConfig({ .enable = true, .allowLeft = true, .allowRight = true })
        .setVSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.25f, .min = 30 });

    /* Configure children of 'container1' . Note the '.value' of both add up to 1.0f (100%). Layouts here
       are normal, not pinchable.*/
    container1->getStyle()
        .setLayout(HkLayout::Vertical)
        .setColor({ 1.0f, 0.0f, 0.0f ,1.0f })
        .setPinchConfig({ .enable = true, .allowRight = true, .allowTop = true, .allowBottom = true })
        .setHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.5f, .min = 30 });

    container2->getStyle()
        .setLayout(HkLayout::Vertical)
        .setColor({ 0.3f,0.5f,0.0f ,1.0f })
        .setPinchConfig({ .enable = true, .allowTop = true, .allowBottom = true })
        .setHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.5f, .min = 30 });

    /* Push container1 and 2 to be children of container4. Now space of container4 has been partitioned
       into 2 equal parts with a vertical bar in the middle that's draggable. */
    container4->pushChildren({ container1, container2 });

    /* Set container6 to be vertically pinchable. Children containers will be scalable vertically with
       while pinching the horizontal bar that will appear. */
    container6->getStyle()
        .setColor({ 0.0f,0.7f,1.0f, 1.0f })
        .setLayout(HkLayout::VPinch)
        .setPinchConfig({ .enable = true, .allowLeft = true, .allowRight = true })
        .setHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.5f, .min = 30 });

    /* Create container3, allowing pinch from the left, right and bottom */
    container3->getStyle()
        .setLayout(HkLayout::Horizontal)
        .setColor({ 0.0f,0.0f,1.0f,1.0f })
        .setPinchConfig({ .enable = true, .allowLeft = true, .allowRight = true, .allowBottom = true })
        .setVSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.75f, .min = 30 });

    /* Push container3 and container4 as pinchable children of container6 */
    container6->pushChildren({ container3, container4 });

    /* Create container5 in similar manner as the others */
    container5->getStyle()
        .setLayout(HkLayout::Horizontal)
        .setColor({ 0.4f,0.7f,0.0f, 1.0f })
        .setPinchConfig({ .enable = true, .allowLeft = true })
        .setHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.25f, .min = 30 });

    /* Create container7 in similar manner as the others */
    container7->getStyle()
        .setLayout(HkLayout::Horizontal)
        .setColor({ 0.4f,0.7f,0.0f, 1.0f })
        .setPinchConfig({ .enable = true, .allowLeft = true })
        .setHSizeConfig(
            { .type = HkSizeType::PercParent, .value = 0.25f, .min = 30 });

    /* Push children. CHildren 5 thru 7 are now in a 25% 50% 25% formation and we can drag
       the pinch bars between them to adjust the percentages. */
    windowFrame->pushChildren({ container5, container6, container7 });

    /* Add a child to container2 just for fun */
    container8->getStyle()
        .setLayout(HkLayout::Horizontal)
        .setColor({ 1.0f, 0.3f, .5f, 1.0f })
        .setVHSizeConfig(
            { .value = 100, .min = 50 }, { .value = 100, .min = 50 });
    container2->pushChildren({ container8 });

    /*Blocking from here on until window exit*/
    HkAppManager::get().runLoop();
    HkAppManager::get().teardown();
    return 0;
}
