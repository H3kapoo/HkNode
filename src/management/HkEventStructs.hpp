#pragma once

namespace hkui
{
enum class HkMouseAction
{
    None,
    Click,
    Release,
    Move,
    Drag,
    Entered,
    Exited
};

enum class HkMouseButton
{
    None,
    Left,
    Right,
    Middle,
    Unknown
};

enum class HkEvent
{
    None,
    FocusScan,
    HoverScan,
    GeneralUpdate,
    WindowResize,
    MouseMove,
    MouseClick,
    MouseEnterExit,
    MouseScroll,
    DropPath,
    // Keyboard // TO DO:
};
}