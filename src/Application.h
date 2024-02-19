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

public:
    Application();
    ~Application();

    void LoadFile(const char* filename);

    void Setup();
    void Input();
    void Update();
    void Render();
    void Destroy();

    bool IsRunning() const;

    void SetPaused(bool bPaused);
    void SetSpeed(int speed);
    void SetKey(uint8_t key, bool bPressed);

    void SetDisplayLines(bool bShow);
    void SetLightTheme(bool bLight);

private:
    friend class FrontEnd;

    Chip8    *m_chip8 {nullptr};
    FrontEnd *m_frontend {nullptr};

    uint8_t m_isRunning     : 1  {false};
    uint8_t m_isPaused      : 1  {false};
    uint8_t m_showLines     : 1  {true};
    uint8_t m_isLightTheme  : 1  {true};

    std::map<uint16_t, std::string> m_disassembled;

    // Window sizes (normal and ui)
    static constexpr uint32_t m_displayWidth    { 64 * 10 };               // 640
    static constexpr uint32_t m_displayHeight   { ( 32 * 10 ) + 20 };     // 340 (+ 20 for the title bar)

    static constexpr uint32_t m_uiDisplacement  { 420 };
    static constexpr uint32_t m_windowWidthUI   { m_displayWidth + m_uiDisplacement };    // 940 (+ 300 for the ui)

    // Execution speed map (cycles per frame)
    std::vector<uint32_t> m_speeds = {1, 2, 7, 10, 15, 20, 30, 60, 120, 240, 1000};
    uint8_t m_speedIndex = 0;

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
    m_speedIndex = speed;
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

#endif //CHIP0U_APPLICATION_H
