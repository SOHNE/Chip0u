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

#include "FrontEnd.h"

#include "Application.h"


std::string
HEX(uint32_t n, uint8_t d)
{
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
        s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
}


FrontEnd::FrontEnd(Application *app)
    : m_app(app)
{
}

FrontEnd::~FrontEnd()
{
    rlImGuiShutdown();
    delete m_app;
}


void
FrontEnd::Setup()
{
    // Imgui
    IMGUI_CHECKVERSION();
    rlImGuiSetup(true);
    m_io = &ImGui::GetIO();

    // File dialog setup
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".ch8", ImVec4(0.25f, 1.0f, 0.0f, 1.0f), ICON_FA_FILE);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, "roms", ImVec4(0.25f, 1.0f, 0.0f, 1.0f), ICON_FA_GAMEPAD);

    m_dialogConfig.path = "./roms";
    m_dialogConfig.flags = ImGuiFileDialogFlags_Modal;
}

void
FrontEnd::Draw()
{
    rlImGuiBegin();

        DrawToolbar();

        if (m_showDebug)
        {
            DrawDebug();
        }

    rlImGuiEnd();
}

void
FrontEnd::DrawToolbar()
{
    std::string menu_action{};
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open ROM..."))
            {
                menu_action = "Open";
            }

            if (ImGui::MenuItem("Exit")) m_app->m_isRunning = false;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Show Debug (F1)", nullptr, m_showDebug))
            {
                m_showDebug = !m_showDebug;
                SetWindowSize(m_showDebug ? m_app->m_windowWidthUI : m_app->m_displayWidth, m_app->m_displayHeight);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About")) menu_action = "About";
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    {
        if (menu_action == "Open")
        {
            m_app->SetPaused(true);
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose a ROM", ".ch8,.rom", m_dialogConfig);
        }

        ImVec2 maxSize = ImVec2(m_app->m_displayWidth, m_app->m_displayHeight);
        ImVec2 minSize = ImVec2(m_app->m_displayWidth * 0.75f, m_app->m_displayHeight * 0.75f);
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, minSize, maxSize))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            { // action if OK
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                m_app->LoadFile(filePathName.c_str());
            }

            // close
            m_app->m_isPaused = false;
            ImGuiFileDialog::Instance()->Close();
        }
    }

    {
        if (menu_action == "About") ImGui::OpenPopup("About");

        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
        {
            m_app->m_isPaused = true;

            ImGui::SetItemDefaultFocus();

            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Chip0 - CHIP-8 Emulator").x) * 0.5f);
            ImGui::Text("Chip0u - CHIP-8 Emulator");
            ImGui::Separator();
            ImGui::Text("Description: A learning project");
            ImGui::Text("Author: Leandro Peres");
            ImGui::Text("Libraries: raylib, ImGui, ImGuiFileDialog");
            ImGui::Text("License: MIT");

            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
                m_app->m_isPaused = false;
            }
            ImGui::EndPopup();
        }
    }
}

void
FrontEnd::DrawDebug()
{
    {
        DrawControls();
        DrawStack();
        DrawRegisters();
        DrawDataRegisters();
    }

    {
        auto debugWindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

        ImGui::SetNextWindowPos(ImVec2(64 * 10, 190), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(m_app->m_uiDisplacement, 150), ImGuiCond_Always);
        ImGui::Begin("Debugger", nullptr, debugWindowFlags | ImGuiWindowFlags_NoTitleBar);

        if (ImGui::BeginTabBar("DebuggerTabs", ImGuiTabBarFlags_None))
        {
            DrawDisassembly();
            DrawMemory();
            DrawInput();

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}

void
FrontEnd::DrawControls()
{
    auto debugWindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

    Chip8 *chip8 = m_app->m_chip8;
    std::string menu_action{};

    // New window for the buttons
    ImGui::SetNextWindowPos(ImVec2(64 * 10, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_app->m_uiDisplacement * 0.4F, 50), ImGuiCond_Always);
    ImGui::Begin("Controls", nullptr, debugWindowFlags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        if (m_app->m_isPaused)
        {
            if (ImGui::Button(ICON_FA_PLAY))
            {
                m_app->m_isPaused = false;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("Resume execution");
            }
        }
        else
        {
            if (ImGui::Button(ICON_FA_PAUSE))
            {
                m_app->m_isPaused = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("Pause execution");
            }
        }

        // Step button
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_RIGHT_TO_BRACKET))
        {
            m_app->m_isPaused = true;
            chip8->Clock();
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Step forward");
        }

        // Reset button with reloading ROM
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_ROTATE))
        {
            m_app->Reset();
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Reset and reload ROM");
        }

        // Show display lines
        ImGui::SameLine();
        if (m_app->m_showLines)
        {
            if (ImGui::Button(ICON_FA_CHESS_BOARD))
            {
                m_app->SetDisplayLines(false);
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("Hide display lines");
            }
        }
        else
        {
            if (ImGui::Button(ICON_FA_SQUARE))
            {
                m_app->SetDisplayLines(true);
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("Show display lines");
            }
        }

        // Show display lines
        ImGui::SameLine();
        if (m_app->m_isLightTheme)
        {
            if (ImGui::Button(ICON_FA_SUN))
            {
                m_app->SetLightTheme(false);
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("Toggle dark theme");
            }
        }
        else
        {
            if (ImGui::Button(ICON_FA_MOON))
            {
                m_app->SetLightTheme(true);
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("Toggle light theme");
            }
        }

        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_ELLIPSIS))
        {
            menu_action = "ExtraControls";
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Extra controls");
        }

        {
            if (menu_action == "ExtraControls")
            {
                ImGui::OpenPopup("Extra Controls");
            }

            // appear just below the button
            ImGui::SetNextWindowPos(ImVec2(64 * 10, 70), ImGuiCond_Appearing);
            if (ImGui::BeginPopupModal("Extra Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                DrawExtraControls();
                ImGui::EndPopup();
            }
        }

    ImGui::End();
}


// As a modal centered popup window, manipulates Application::emulation_cfg_t
void
FrontEnd::DrawExtraControls()
{
    ImGui::SetItemDefaultFocus();

        ImGui::Text("Emulation Configuration");
        ImGui::Separator();

        ImGui::SliderFloat("Precision", &m_app->m_emulation_cfg.precision, 0.0f, 1.0f, "%.2f");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Precision of the lerp function");
        }

        ImGui::SliderFloat("Lerp Duration", &m_app->m_emulation_cfg.lerp_duration, 0.1f, 1.0f, "%.2f");
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Duration of the lerp effect");
        }

        // Cycles per frame
        if (ImGui::BeginCombo("Speed", std::to_string(m_app->m_speeds[m_app->m_emulation_cfg.speed]).c_str()))
        {
            for (int i = 0; i < m_app->m_speeds.size(); ++i)
            {
                bool isSelected = (m_app->m_emulation_cfg.speed == i);
                std::string speeds = std::to_string(m_app->m_speeds[i]) + " cycles/frame";
                if (ImGui::Selectable(speeds.c_str(), isSelected))
                {
                    m_app->m_emulation_cfg.speed = i;
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Cycles per frame");
        }

        // Close button
        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
        }
}

void
FrontEnd::DrawRegisters()
{
    Chip8 *chip8 = m_app->m_chip8;

    auto debugWindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2(64 * 10, 70), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_app->m_uiDisplacement * 0.4F, 120), ImGuiCond_Always);
    ImGui::Begin("Registers", nullptr, debugWindowFlags);
        ImGui::Text("PC: 0x%s\t[%d]", HEX(chip8->GetPC(), 3).c_str(), chip8->GetPC());
        ImGui::Text("I : 0x%s\t[%d]", HEX(chip8->GetI(), 3).c_str(), chip8->GetI());
        ImGui::Text("SP: 0x%s\t[%d]", HEX(chip8->GetSP(), 1).c_str(), chip8->GetSP());
        ImGui::Text("DT: 0x%s\t[%d]", HEX(chip8->GetDelayTimer(), 1).c_str(), chip8->GetDelayTimer());
        ImGui::Text("ST: 0x%s\t[%d]", HEX(chip8->GetSoundTimer(), 1).c_str(), chip8->GetSoundTimer());
    ImGui::End();
}

void
FrontEnd::DrawStack()
{
    auto debugWindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2((64 * 10) + m_app->m_uiDisplacement * 0.4F, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_app->m_uiDisplacement * 0.25F, 170), ImGuiCond_Always);
    ImGui::Begin("Stack", nullptr, debugWindowFlags);
        auto stack = m_app->m_chip8->GetStack();
        for (int i = 0; i < 16; ++i)
        {
            ImGui::Text("0x%s: 0x%s", HEX(i, 1).c_str(), HEX(stack[i], 3).c_str());
        }
    ImGui::End();
}

void
FrontEnd::DrawDataRegisters()
{
    auto debugWindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2((64 * 10) + m_app->m_uiDisplacement * 0.65F, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_app->m_uiDisplacement * 0.35F, 170), ImGuiCond_Always);
    ImGui::Begin("V0~VF", nullptr, debugWindowFlags);
        for (int i = 0; i < 16; ++i)
        {
            uint8_t v = m_app->m_chip8->GetV()[i];
            ImGui::Text("V%X: 0x%s [%d]", i, HEX(v, 2).c_str(), v);
        }
    ImGui::End();
}

void
FrontEnd::DrawMemory()
{
    if (!ImGui::BeginTabItem("Memory")) return;

    static char start[4] = "000";
    static char end[4] = "FFF";

    if (ImGui::TabItemButton(ICON_FA_CIRCLE_QUESTION, ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
        ImGui::OpenPopup("DebugHelper");

    if (ImGui::BeginPopup("DebugHelper"))
    {
            ImGui::Text("Memory range: 000 - FFF");
            ImGui::Text("000-1FF - Chip 8 interpreter (contains font set in emu)");
            ImGui::Text("050-0A0 - Used for the built in 4x5 pixel font set (0-F)");
            ImGui::Text("200-FFF - Program ROM and work RAM");
            ImGui::Separator();
            ImGui::Text("Range: ");
            ImGui::SetNextItemWidth((m_app->m_uiDisplacement * 0.5F) - 150);
            ImGui::InputText("Start", start, IM_ARRAYSIZE(start), ImGuiInputTextFlags_CharsHexadecimal);
            ImGui::SameLine();
            ImGui::SetNextItemWidth((m_app->m_uiDisplacement * 0.5F) - 150);
            ImGui::InputText("End", end, IM_ARRAYSIZE(end), ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::EndPopup();
    }

    uint32_t start_val = strlen(start) != 0 ? std::stoul(start, nullptr, 16) : 0;
    uint32_t end_val = strlen(end) != 0 ? std::stoul(end, nullptr, 16) : 4096;

    // Clamp the values to valid memory range
    start_val = std::max(0u, std::min(start_val, 4096u));
    end_val = std::max(0u, std::min(end_val, 4096u));

    auto memory = m_app->m_chip8->GetMemory();
    for (uint32_t i = start_val; i < end_val; i += 16)
    {
        std::string s = HEX(i, 3) + ": ";
        ImGui::TextUnformatted(s.c_str());
        ImGui::SameLine();
        for (int j = 0; j < std::min(16u, end_val - i + 1); ++j)
        {
            s = HEX(memory[i + j], 2);
            ImVec4 color = (memory[i + j] != 0) ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
            ImGui::TextColored(color, "%s", s.c_str());
            // Tool tip
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Addr: %s", HEX(i + j, 3).c_str());
                ImGui::Separator();

                uint8_t value = memory[i + j];
                ImGui::Text("Decimal: %hhu", value);
                if (value >= 0x20 && value <= 0x7E)
                {
                    ImGui::Text("Char: %c", value);
                }
                else
                {
                    ImGui::Text("Char: ---");
                }
                ImGui::EndTooltip();
            }

            if (j < 15) ImGui::SameLine();
        }
    }
    ImGui::EndTabItem();
}

void
FrontEnd::DrawInput()
{
    if (!ImGui::BeginTabItem("Input")) return;

    if (ImGui::TabItemButton(ICON_FA_CIRCLE_QUESTION, ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
        ImGui::OpenPopup("DebugHelper");

    if (ImGui::BeginPopup("DebugHelper"))
    {
        ImGui::Text("0x0 - 0xF: Chip 8 keys");
        ImGui::Text("Press a key to activate");
        ImGui::EndPopup();
    }

    Chip8 *chip8 = m_app->m_chip8;
    auto keys = chip8->GetKeyboard();
    auto flags = ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_Repeat;

    uint8_t i = 0;
    for (const auto &keypair : m_uiKeys)
    {
        ImVec4 buttonColor = keys[keypair.first] ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);

        bool isPressed = ImGui::ButtonEx(keypair.second, ImVec2(24, 24), flags);
        chip8->SetKey(keypair.first, isPressed);

        ImGui::PopStyleColor();

        // Arrange buttons in a 4x4 grid
        if (i++ % 4 != 3) ImGui::SameLine();
    }

    ImGui::EndTabItem();
}

void
FrontEnd::DrawDisassembly()
{
    if (!ImGui::BeginTabItem("Disassembled")) return;

    if (ImGui::TabItemButton(ICON_FA_CIRCLE_QUESTION, ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
        ImGui::OpenPopup("DebugHelper");

    if (ImGui::BeginPopup("DebugHelper"))
    {
        ImGui::Text("Disassembled instructions");
        ImGui::Text("More human readable than raw memory");
        ImGui::Text("Shows the current instruction (PC) in red");
        ImGui::Separator();
        ImGui::Checkbox("Limit Disassembly Range", &m_limitDisassemblyRange);
        if(ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Limit the disassembly range to the current PC");
        }
        ImGui::EndPopup();
    }

    {
        std::string header = "addr   op" + std::string(5, ' ') + "instruction";
        ImGui::TextColored(ImVec4(0.25f, 1.0f, 0.0f, 1.0f), "%s", header.c_str());
    }

    {
        int pc = m_app->m_chip8->GetPC();
        int start = std::max(0, pc - 10);
        int end = std::min(4096, pc + 10);

        for (const auto &[addr, inst]: m_app->m_disassembled)
        {
            // Limit the disassembly range to the current PC
            if (m_limitDisassemblyRange && (addr < start || addr > end)) continue;

            // Is the current instruction? Then color it red!
            ImVec4 color = (addr == pc)
                           ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
                           : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

            ImGui::TextColored(color, "%s", inst.c_str());
        }
    }
    ImGui::EndTabItem();
}
