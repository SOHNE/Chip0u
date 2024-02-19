#ifndef CHIP0U_FRONTEND_H
#define CHIP0U_FRONTEND_H

#include <cstdint>
#include <utility>
#include <vector>

#include "imgui.h"
#include "imgui_internal.h"
#include "rlImGui.h"
#include "ImGuiFileDialog.h"

// Forward declaration
class Application;
struct ImGuiIO;


typedef std::pair<uint8_t, const char*> keypair_t;


class FrontEnd
{
public:
    FrontEnd(Application *app);
    ~FrontEnd();

    void Setup();

    void Draw();

    bool GetShowDebug() const;
    void SetShowDebug(bool show);

protected:
    void DrawToolbar();

    void DrawDebug();
    void DrawControls();
    void DrawStack();
    void DrawRegisters();
    void DrawDataRegisters();

    void DrawMemory();
    void DrawInput();
    void DrawDisassembly();

private:
    Application *m_app {nullptr};

    uint8_t m_showDebug : 1 {true};
    ImGuiIO *m_io {nullptr};

    // File dialog
    IGFD::FileDialogConfig m_dialogConfig;

    // UI keymapping
    std::vector<keypair_t> m_uiKeys =
    {
        {0x1, "1"}, {0x2, "2"}, {0x3, "3"}, {0xC, "C"},
        {0x4, "4"}, {0x5, "5"}, {0x6, "6"}, {0xD, "D"},
        {0x7, "7"}, {0x8, "8"}, {0x9, "9"}, {0xE, "E"},
        {0xA, "A"}, {0x0, "0"}, {0xB, "B"}, {0xF, "F"}
    };
};

inline bool
FrontEnd::GetShowDebug() const
{
    return m_showDebug;
}

inline void
FrontEnd::SetShowDebug(bool show)
{
    m_showDebug = show;
}

#endif //CHIP0U_FRONTEND_H
