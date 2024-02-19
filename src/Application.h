#ifndef CHIP0U_APPLICATION_H
#define CHIP0U_APPLICATION_H

#include <map>
#include <vector>

#include "raylib.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "rlImGui.h"
#include "ImGuiFileDialog.h"

#include "chip8/Chip8.h"

typedef std::pair<uint8_t, const char*> keypair_t;
typedef std::map<KeyboardKey, keypair_t> keymapping_t;


class Application
{
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

private:
    void DrawToolbarUI();
    void DrawDebugUI();
    void DrawKeysUI();

private:
    Chip8   *m_chip8 {nullptr};

    uint8_t m_isRunning     : 1  {false};
    uint8_t m_isPaused      : 1  {false};
    uint8_t m_showLines     : 1  {false};
    uint8_t m_isLightTheme  : 1  {true};

    ImGuiIO *m_io           {nullptr};
    uint8_t m_showUI    : 1 {true};

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
    keymapping_t m_keyMapping =
    {
        {KEY_ONE,   {0x1, "1"}},
        {KEY_TWO,   {0x2, "2"}},
        {KEY_THREE, {0x3, "3"}},
        {KEY_FOUR,  {0xC, "C"}},
        {KEY_Q,     {0x4, "4"}},
        {KEY_W,     {0x5, "5"}},
        {KEY_E,     {0x6, "6"}},
        {KEY_R,     {0xD, "D"}},
        {KEY_A,     {0x7, "7"}},
        {KEY_S,     {0x8, "8"}},
        {KEY_D,     {0x9, "9"}},
        {KEY_F,     {0xE, "E"}},
        {KEY_Z,     {0xA, "A"}},
        {KEY_X,     {0x0, "0"}},
        {KEY_C,     {0xB, "B"}},
        {KEY_V,     {0xF, "F"}}
    };

    // UI keymapping
    std::vector<keypair_t> m_uiKeys =
    {
        {0x1, "1"}, {0x2, "2"}, {0x3, "3"}, {0xC, "C"},
        {0x4, "4"}, {0x5, "5"}, {0x6, "6"}, {0xD, "D"},
        {0x7, "7"}, {0x8, "8"}, {0x9, "9"}, {0xE, "E"},
        {0xA, "A"}, {0x0, "0"}, {0xB, "B"}, {0xF, "F"}
    };


    // Color palette
    typedef struct ColorPalette
    {
        Color bg;
        Color fg;
    } ColorPalette;

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
    return m_isRunning && !WindowShouldClose();
}

#endif //CHIP0U_APPLICATION_H
