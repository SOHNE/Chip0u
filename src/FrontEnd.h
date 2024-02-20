// MIT License

// Copyright (c) 2024 Leandro Peres, aka "zschzen"

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

    bool m_limitDisassemblyRange {true};

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
