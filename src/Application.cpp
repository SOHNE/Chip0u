#include "Application.h"

#include <iostream>

std::string hex(uint32_t n, uint8_t d)
{
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
        s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
};

Application::Application()
{
    // Create a window sized by the CHIP8 resolution
    InitWindow(m_showUI ? m_windowWidthUI : m_displayWidth,
               m_displayHeight, "Chip0u [CHIP-8 Emulator]");
    SetTargetFPS(60);
    m_isRunning = true;

    m_chip8 = new Chip8();
}

Application::~Application()
{
    delete m_chip8;
}



void
Application::LoadFile(const char *filename)
{
    m_latestFile = std::string(filename);
    m_chip8->LoadGame(filename);
    m_disassembled = m_chip8->GetDisassembled();
}

void
Application::Setup()
{
    LoadFile("roms/TEST.ch8");

    // Imgui
    IMGUI_CHECKVERSION();
    rlImGuiSetup(true);
    m_io = &ImGui::GetIO();

    // File dialog setup
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".ch8", ImVec4(0.25f, 1.0f, 0.0f, 1.0f), ICON_FA_FILE);
    ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, "roms", ImVec4(0.25f, 1.0f, 0.0f, 1.0f), ICON_FA_GAMEPAD);
}

void
Application::Input()
{
    for (const auto& [key, value] : m_keyMapping)
    {
        uint8_t keyVal = value.first;
        if (IsKeyDown(key)) m_chip8->SetKey(keyVal, true);
        else if (IsKeyReleased(key)) m_chip8->SetKey(keyVal, false);
    }

    if (IsKeyPressed(KEY_F1))
    {
        m_showUI = !m_showUI;
        SetWindowSize(m_showUI ? m_windowWidthUI : m_displayWidth, m_displayHeight);
    }
}


void
Application::Update()
{
    if (!m_isPaused)
    {
        for (int i = 0; i < m_speeds[m_speedIndex]; ++i)
        {
            m_chip8->Clock();
        }
    }
}

void Application::Render()
{
    BeginDrawing();

    ClearBackground(m_themes[m_isLightTheme].bg);

    // CHIP-8 display
    if (m_chip8->GetDrawFlag())
    {
        auto display = m_chip8->GetDisplay();
        for (int y = 0; y < 32; ++y)
        {
            for (int x = 0; x < 64; ++x)
            {
                if (display[y * 64 + x] != 1) continue;

                // Add an offset to the y-coordinate to account for the toolbar height
                DrawRectangle(x * 10, y * 10 + 20, 10, 10, m_themes[m_isLightTheme].fg);
            }
        }
    }

    // Lines
    if (m_showLines)
    {
        Color backgroundColor = Fade(m_themes[m_isLightTheme].bg, 0.25f);
        Color foregroundColor = Fade(m_themes[m_isLightTheme].fg, 0.25f);

        for (uint16_t i = 0; i < 64; ++i)
        {
            if (i < 32)
            {
                DrawLine(0, i * 10 + 20, 64 * 10, i * 10 + 20, backgroundColor);
                DrawLine(0, i * 10 + 20, 64 * 10, i * 10 + 20, foregroundColor);
            }

            DrawLine(i * 10, 0 + 20, i * 10, 32 * 10 + 20, backgroundColor);
            DrawLine(i * 10, 0 + 20, i * 10, 32 * 10 + 20, foregroundColor);
        }
    }

    {
        rlImGuiBegin();

            DrawToolbarUI();

            // Draw UI
            if (m_showUI)
            {
                DrawDebugUI();
            }

        rlImGuiEnd();
    }

    EndDrawing();
}

void
Application::DrawToolbarUI()
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

            if (ImGui::MenuItem("Exit")) m_isRunning = false;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Show Debug UI (F1)", nullptr, m_showUI))
            {
                m_showUI = !m_showUI;
                SetWindowSize(m_showUI ? m_windowWidthUI : m_displayWidth, m_displayHeight);
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
            m_isPaused = true;

            IGFD::FileDialogConfig config;
            config.path = "./roms";
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose a ROM", ".ch8,.rom", config);
        }

        ImVec2 maxSize = ImVec2(m_displayWidth, m_displayHeight);
        ImVec2 minSize = ImVec2(m_displayWidth * 0.75f, m_displayHeight * 0.75f);
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, minSize, maxSize))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            { // action if OK
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                LoadFile(filePathName.c_str());
            }

            // close
            m_isPaused = false;
            ImGuiFileDialog::Instance()->Close();
        }
    }

    {
        if (menu_action == "About") ImGui::OpenPopup("About");

        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
        {
            m_isPaused = true;

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
                m_isPaused = false;
            }
            ImGui::EndPopup();
        }
    }
}


void
Application::DrawDebugUI()
{
    auto debugWindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

    // New window for the buttons
    ImGui::SetNextWindowPos(ImVec2(64 * 10, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_uiDisplacement * 0.4F, 50), ImGuiCond_Always);
    ImGui::Begin("Controls", nullptr, debugWindowFlags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        if (m_isPaused)
        {
            if (ImGui::Button(ICON_FA_PLAY))
            {
                m_isPaused = false;
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
                m_isPaused = true;
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
            m_isPaused = true;
            m_chip8->Clock();
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Step forward");
        }

        // Reset button with reloading ROM
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_ROTATE))
        {
            m_chip8->LoadGame(m_latestFile.c_str());
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Reset and reload ROM");
        }

        // Show display lines
        ImGui::SameLine();
        if (m_showLines)
        {
            if (ImGui::Button(ICON_FA_CHESS_BOARD))
            {
                m_showLines = false;
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
                m_showLines = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("Show display lines");
            }
        }

        // Show display lines
        ImGui::SameLine();
        if (m_isLightTheme)
        {
            if (ImGui::Button(ICON_FA_SUN))
            {
                m_isLightTheme = false;
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
                m_isLightTheme = true;
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("Toggle light theme");
            }
        }

        // Cycles per frame
        ImGui::SameLine();
        if (ImGui::BeginCombo("Speed", std::to_string(m_speeds[m_speedIndex]).c_str()))
        {
            for (int i = 0; i < m_speeds.size(); ++i)
            {
                bool isSelected = (m_speedIndex == i);
                std::string speeds = std::to_string(m_speeds[i]) + " cycles/frame";
                if (ImGui::Selectable(speeds.c_str(), isSelected))
                {
                    m_speedIndex = i;
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

    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(64 * 10, 70), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_uiDisplacement * 0.4F, 120), ImGuiCond_Always);
    ImGui::Begin("Registers", nullptr, debugWindowFlags);
        ImGui::Text("PC: 0x%s\t[%d]", hex(m_chip8->GetPC(), 3).c_str(), m_chip8->GetPC());
        ImGui::Text("I : 0x%s\t[%d]", hex(m_chip8->GetI(), 3).c_str(), m_chip8->GetI());
        ImGui::Text("SP: 0x%s\t[%d]", hex(m_chip8->GetSP(), 1).c_str(), m_chip8->GetSP());
        ImGui::Text("DT: 0x%s\t[%d]", hex(m_chip8->GetDelayTimer(), 1).c_str(), m_chip8->GetDelayTimer());
        ImGui::Text("ST: 0x%s\t[%d]", hex(m_chip8->GetSoundTimer(), 1).c_str(), m_chip8->GetSoundTimer());
    ImGui::End();

    // Stack on left side of the registers
    ImGui::SetNextWindowPos(ImVec2((64 * 10) + m_uiDisplacement * 0.4F, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_uiDisplacement * 0.25F, 170), ImGuiCond_Always);
    ImGui::Begin("Stack", nullptr, debugWindowFlags);
    auto stack = m_chip8->GetStack();
    for (int i = 0; i < 16; ++i)
    {
        ImGui::Text("0x%s: 0x%s", hex(i, 1).c_str(), hex(stack[i], 3).c_str());
    }
    ImGui::End();

    // Data registers
    ImGui::SetNextWindowPos(ImVec2((64 * 10) + m_uiDisplacement * 0.65F, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_uiDisplacement * 0.35F, 170), ImGuiCond_Always);
    ImGui::Begin("V0~VF", nullptr, debugWindowFlags);
        for (int i = 0; i < 16; ++i)
        {
            uint8_t v = m_chip8->GetV()[i];
            ImGui::Text("V%X: 0x%s [%d]", i, hex(v, 2).c_str(), v);
        }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(64 * 10, 190), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_uiDisplacement, 150), ImGuiCond_Always);
    ImGui::Begin("Debugger", nullptr, debugWindowFlags | ImGuiWindowFlags_NoTitleBar);

        if (ImGui::BeginTabBar("DebuggerTabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Memory"))
            {
                if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                    ImGui::OpenPopup("DebugHelper");

                if (ImGui::BeginPopup("DebugHelper"))
                {
                    ImGui::Text("Memory range: 000 - FFF");
                    ImGui::Text("000-1FF - Chip 8 interpreter (contains font set in emu)");
                    ImGui::Text("050-0A0 - Used for the built in 4x5 pixel font set (0-F)");
                    ImGui::Text("200-FFF - Program ROM and work RAM");
                    ImGui::EndPopup();
                }

                static char start[4] = "000";
                static char end[4] = "FFF";

                ImGui::Text("Range: ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth((m_uiDisplacement * 0.5F) - 150);
                ImGui::InputText("Start", start, IM_ARRAYSIZE(start), ImGuiInputTextFlags_CharsHexadecimal);
                ImGui::SameLine();
                ImGui::SetNextItemWidth((m_uiDisplacement * 0.5F) - 150);
                ImGui::InputText("End", end, IM_ARRAYSIZE(end), ImGuiInputTextFlags_CharsHexadecimal);

                uint32_t start_val = strlen(start) != 0 ? std::stoul(start, nullptr, 16) : 0;
                uint32_t end_val = strlen(end) != 0 ? std::stoul(end, nullptr, 16) : 4096;

                // Clamp the values to valid memory range
                start_val = std::max(0u, std::min(start_val, 4096u));
                end_val = std::max(0u, std::min(end_val, 4096u));

                ImGui::Separator();

                auto memory = m_chip8->GetMemory();
                for (uint32_t i = start_val; i < end_val; i += 16)
                {
                    std::string s = hex(i, 3) + ": ";
                    ImGui::TextUnformatted(s.c_str());
                    ImGui::SameLine();
                    for (int j = 0; j < std::min(16u, end_val - i + 1); ++j)
                    {
                        s = hex(memory[i + j], 2);
                        ImVec4 color = (memory[i + j] != 0) ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
                        ImGui::TextColored(color, "%s", s.c_str());
                        // Tool tip
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::Text("Addr: %s", hex(i + j, 3).c_str());
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

            if (ImGui::BeginTabItem("Input"))
            {
                if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                    ImGui::OpenPopup("DebugHelper");

                if (ImGui::BeginPopup("DebugHelper"))
                {
                    ImGui::Text("0x0 - 0xF: Chip 8 keys");
                    ImGui::Text("Press a key to activate");
                    ImGui::EndPopup();
                }

                DrawKeysUI();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Disassembled"))
            {
                if (ImGui::TabItemButton("?", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
                    ImGui::OpenPopup("DebugHelper");

                if (ImGui::BeginPopup("DebugHelper"))
                {
                    ImGui::Text("Disassembled instructions");
                    ImGui::Text("More human readable than raw memory");
                    ImGui::Text("Shows the current instruction (PC) in red");
                    ImGui::EndPopup();
                }

                std::string header = "addr  op" + std::string(5, ' ') + "instruction";
                ImGui::TextColored(ImVec4(0.25f, 1.0f, 0.0f, 1.0f), "%s", header.c_str());
                header = "----  ----   -----------";
                ImGui::Text("%s", header.c_str());

                for (const auto& [addr, inst] : m_disassembled)
                {
                    // Is the current instruction? Then color it red!
                    header = hex(addr, 4) + "  " + inst;
                    if (addr == m_chip8->GetPC())
                    {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", header.c_str());
                    }
                    else
                    {
                        ImGui::Text("%s", header.c_str());
                    }
                }
                ImGui::EndTabItem();
            }

    ImGui::EndTabBar();
}

    ImGui::End();
}

void
Application::DrawKeysUI()
{
    auto keys = m_chip8->GetKeyboard();
    auto flags = ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_Repeat;

    uint8_t i = 0;
    for (const auto &keypair : m_uiKeys)
    {
        ImVec4 buttonColor = keys[keypair.first] ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);

        bool isPressed = ImGui::ButtonEx(keypair.second, ImVec2(24, 24), flags);
        m_chip8->SetKey(keypair.first, isPressed);

        ImGui::PopStyleColor();

        // Arrange buttons in a 4x4 grid
        if (i++ % 4 != 3) ImGui::SameLine();
    }
}

void
Application::Destroy()
{
    m_isRunning = false;
    rlImGuiShutdown();
    CloseWindow();
}
