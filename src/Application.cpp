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

#include "Application.h"

#include "FrontEnd.h"


Application::Application()
{
    m_chip8 = new Chip8();
    m_frontend = new FrontEnd(this);

    // Create a window sized by the CHIP8 resolution
    InitWindow(m_frontend->GetShowDebug() ? m_windowWidthUI : m_displayWidth,
               m_displayHeight, "Chip0u [CHIP-8 Emulator]");
    SetTargetFPS(60);

    m_isRunning = true;
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

    // Reset PixelColor
    for (auto & p : PixelColor) p = m_themes[m_isLightTheme].bg;
}

void
Application::Setup()
{
    LoadFile("roms/TEST.ch8");

    m_frontend->Setup();
}

void
Application::Input()
{
    for (const auto& [key, value] : m_keyMapping)
    {
        if (IsKeyDown(key)) m_chip8->SetKey(value, true);
        else if (IsKeyReleased(key)) m_chip8->SetKey(value, false);
    }

    if (IsKeyPressed(KEY_F1))
    {
        // Toggle the debug UI
        m_frontend->SetShowDebug(!m_frontend->GetShowDebug());
        // Resize the window
        SetWindowSize(m_frontend->GetShowDebug() ? m_windowWidthUI : m_displayWidth, m_displayHeight);
    }
}


void
Application::Update()
{
    if (!m_isPaused)
    {
        for (int i = 0; i < m_speeds[m_emulation_cfg.speed]; ++i)
        {
            m_chip8->Clock();
        }
    }
}

void
Application::Reset()
{
    m_chip8->Reset();
    m_chip8->LoadGame(m_latestFile.c_str());
    m_disassembled = m_chip8->GetDisassembled();

    // Reset PixelColor
    for (auto & p : PixelColor) p = m_themes[m_isLightTheme].bg;
}

void Application::Render()
{
    if (!m_chip8->GetDrawFlag()) return;

    BeginDrawing();

        ClearBackground(m_themes[m_isLightTheme].bg);

        // CHIP-8 display
        {
            auto display = m_chip8->GetDisplay();

            for (int y = 0; y < 32; ++y)
            {
                for (int x = 0; x < 64; ++x)
                {
                    // Get the current color of the pixel
                    int32_t i = y * 64 + x;
                    Color currentColor = PixelColor[i];

                    // Determine the target color based on the display data
                    bool isPixelOn = display[i];
                    Color targetColor = isPixelOn
                                        ? m_themes[m_isLightTheme].fg
                                        : m_themes[m_isLightTheme].bg;

                    // Check if the current color is different from the target color
                    bool isColorDifferent = currentColor.r != targetColor.r
                                            || currentColor.g != targetColor.g
                                            || currentColor.b != targetColor.b
                                            || currentColor.a != targetColor.a;

                    // Only calculate the new color if the current color is different from the target color
                    if (isColorDifferent)
                    {
                        // Define the lerp factor (-t/log2(precision))
                        float lerpFactor = -m_emulation_cfg.lerp_duration / log2f(m_emulation_cfg.precision);

                        // Calculate the new color
                        PixelColor[i] = ColorLerp(currentColor, targetColor, lerpFactor);
                    }

                    // Draw the rectangle with the new color
                    DrawRectangle(x * 10, y * 10 + 20, 10, 10, PixelColor[i]);
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

        // Draw the frontend
        m_frontend->Draw();

    EndDrawing();
}

void
Application::Destroy()
{
    m_isRunning = false;
    CloseWindow();
}
