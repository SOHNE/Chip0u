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

#ifndef CHIP0U_APPLICATION_H
#define CHIP0U_APPLICATION_H

#include <map>
#include <vector>

#include "raylib.h"

#include "chip8/Chip8.h"

// Forward declaration
class FrontEnd;

class Application
{
public:
    // Color palette
    typedef struct ColorPalette
    {
        Color bg;
        Color fg;
    } ColorPalette;

    typedef struct emulation_cfg_t
    {
        float precision;
        float lerp_duration;

        int32_t speed;
    } emulation_cfg_t;

public:
    Application();
    ~Application();

    void LoadFile(const char* filename);

    void Setup();
    void Input();
    void Update();
    void Render();
    void Destroy();

    void Reset();

    bool IsRunning() const;

    void SetPaused(bool bPaused);
    void SetSpeed(int speed);
    void SetKey(uint8_t key, bool bPressed);

    void SetDisplayLines(bool bShow);
    void SetLightTheme(bool bLight);

private:
    static Color ColorLerp(Color a, Color b, float halfLife);

private:
    friend class FrontEnd;

    emulation_cfg_t m_emulation_cfg {0.01f, 0.7f};

    Chip8    *m_chip8 {nullptr};
    FrontEnd *m_frontend {nullptr};

    // TODO: move to a struct
    uint8_t m_isRunning     : 1  {false};
    uint8_t m_isPaused      : 1  {false};
    uint8_t m_showLines     : 1  {false};
    uint8_t m_isLightTheme  : 1  {true};

    std::map<uint16_t, std::string> m_disassembled;

    // Window sizes (normal and ui)
    static constexpr uint32_t m_displayWidth    { 64 * 10 };               // 640
    static constexpr uint32_t m_displayHeight   { ( 32 * 10 ) + 20 };     // 340 (+ 20 for the title bar)

    static constexpr uint32_t m_uiDisplacement  { 420 };
    static constexpr uint32_t m_windowWidthUI   { m_displayWidth + m_uiDisplacement };    // 940 (+ 300 for the ui)

    // Display pixel colors
    Color PixelColor[DISPLAY_SIZE] = {m_themes[m_isLightTheme].bg};

    // Execution speed map (cycles per frame)
    std::vector<uint32_t> m_speeds = {1, 2, 7, 10, 15, 20, 30, 60, 120, 240, 1000};

    // Mapping of keys to CHIP8 keys
    std::map<KeyboardKey, uint8_t> m_keyMapping =
    {
        {KEY_ONE,   0x1},
        {KEY_TWO,   0x2},
        {KEY_THREE, 0x3},
        {KEY_FOUR,  0xC},
        {KEY_Q,     0x4},
        {KEY_W,     0x5},
        {KEY_E,     0x6},
        {KEY_R,     0xD},
        {KEY_A,     0x7},
        {KEY_S,     0x8},
        {KEY_D,     0x9},
        {KEY_F,     0xE},
        {KEY_Z,     0xA},
        {KEY_X,     0x0},
        {KEY_C,     0xB},
        {KEY_V,     0xF}
    };

    // Themes
    ColorPalette m_themes[2] = {
            {DARKGRAY, WHITE},
            {RAYWHITE, BLACK}
    };

    // Latest file loaded
    std::string m_latestFile;
};


inline bool
Application::IsRunning() const
{
    return !WindowShouldClose() && m_isRunning;
}

inline void
Application::SetPaused(bool bPaused)
{
    m_isPaused = bPaused;
}

inline void
Application::SetSpeed(int speed)
{
    m_emulation_cfg.speed = speed;
}

inline void
Application::SetKey(uint8_t key, bool bPressed)
{
    m_chip8->SetKey(key, bPressed);
}

inline void
Application::SetDisplayLines(bool bShow)
{
    m_showLines = bShow;
}

inline void
Application::SetLightTheme(bool bLight)
{
    m_isLightTheme = bLight;
}

inline Color
Application::ColorLerp(Color a, Color b, float halfLife)
{
    // Get the time elapsed since the last frame
    float deltaTime = GetFrameTime();

    // Calculate the lerp factor based on the delta time and half-life
    float t = 1.0F - exp2f(-deltaTime / halfLife);

    return (Color)
            {
                    (uint8_t) (a.r + (b.r - a.r) * t),
                    (uint8_t) (a.g + (b.g - a.g) * t),
                    (uint8_t) (a.b + (b.b - a.b) * t),
                    (uint8_t) (a.a + (b.a - a.a) * t)
            };
}

#endif //CHIP0U_APPLICATION_H
