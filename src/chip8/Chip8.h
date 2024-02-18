#ifndef CHIP0U_CHIP8_H
#define CHIP0U_CHIP8_H

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <string>
#include <map>
#include <vector>

#define cuAssert(x) assert(x)

#define TOTAL_RAM       4096
#define TOTAL_REGISTERS 16
#define STACK_SIZE      16

#define DISPLAY_WIDTH   64
#define DISPLAY_HEIGHT  32
#define DISPLAY_SIZE    (DISPLAY_WIDTH * DISPLAY_HEIGHT)

#define FONT_SET_SIZE    (5 * 16)

#define PROG_START      0x200
#define PROG_END        0xFFF



class Chip8
{
public:
    // CHIP8 Instruction format
    typedef struct instruction_t
    {
        uint16_t    OP;     // 16 bit opcode
        uint16_t    NNN;    // 12 bit constant
        uint8_t     NN;     // 8 bit constant
        uint8_t     N;      // 4 bit constant
        uint8_t     X;      // 4 bit register identifier
        uint8_t     Y;      // 4 bit register identifier

        instruction_t(uint16_t opcode)
        {
            OP = opcode;
            // Extract the different parts of the opcode
            NNN = opcode & 0x0FFF;
            NN = opcode & 0x00FF;
            N = opcode & 0x000F;
            X = (opcode & 0x0F00) >> 8;
            Y = (opcode & 0x00F0) >> 4;
        }
    } instruction_t;

    // Instructions map to function pointers
    typedef struct instruction_map_t
    {
        // Holds the name of the instruction.
        std::string name;

        // A pointer to the function in the Chip8 class that implements the instruction.
        void (Chip8::*function)(void) = nullptr;
    } instruction_map_t;

    typedef struct chip8_t
    {
        uint8_t     V[16];             // V0 - VF
        uint8_t     RAM[TOTAL_RAM];    // 4KB RAM (0x000 - 0xFFF)
        uint16_t    PC;                // Program Counter
        uint16_t    I;                 // Index Register
        uint8_t     DT, ST;            // Delay Timer, Sound Timer
        uint16_t    STACK[16];         // Stack
        uint8_t     SP;                // Stack Pointer
        uint8_t     KP[16];            // Keypad
        bool        DP[DISPLAY_SIZE];  // Display
        bool        DF;                // Draw Flag
    } chip8_t;

    /*
    typedef struct debug_t
    {
        // PC breakpoint
        uint16_t    PC;
    } debug_t;
    */

public:
    Chip8();
    ~Chip8() = default;

    void Clock();
    void Reset();
    void LoadGame(const char* filename);

    void SetKey(uint8_t key, bool state);

    //void AddBreakpoint(uint16_t PC);
    //void RemoveBreakpoint(uint16_t PC);

    // Getters
    bool       GetDrawFlag();
    bool*      GetDisplay();
    uint8_t*   GetMemory();
    uint8_t*   GetV();
    uint16_t   GetI();
    uint16_t   GetPC();
    uint8_t    GetSP();
    uint16_t*  GetStack();
    uint8_t    GetDelayTimer();
    uint8_t    GetSoundTimer();
    uint8_t*   GetKeyboard();

    // Get disassembled instructions
    std::map<uint16_t, std::string> GetDisassembled() const;

private:
    // Current state of the CHIP8
    chip8_t m_c8{0};

    // Instructions
    instruction_t m_instr{0};

    // Lookup table for instructions
    std::map<uint16_t, instruction_map_t> m_lookup;

    // a map of comments for each instruction
    std::map<uint16_t, std::string> m_comments =
    {
        {0x00E0, "Clear the display"},
        {0x00EE, "Return from a subroutine"},
        {0x1, "Jump to address NNN"},
        {0x2, "Call subroutine at NNN"},
        {0x3, "Skip next instruction if Vx = NN"},
        {0x4, "Skip next instruction if Vx != NN"},
        {0x5, "Skip next instruction if Vx = Vy"},
        {0x6, "Set Vx = NN"},
        {0x7, "Set Vx = Vx + NN"},
        {0x8, "Set Vx = Vy"},
        {0x9, "Skip next instruction if Vx != Vy"},
        {0xA, "Set I = NNN"},
        {0xB, "Jump to location NNN + V0"},
        {0xC, "Set Vx = random byte AND NN"},
        {0xD, "Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision"},
        {0xE, "Skip next instruction if key with the value of Vx is pressed"},
        {0xF, "Miscellaneous instructions"}
    };

    // vector of all breakpoints
    //std::vector<debug_t> m_breakpoints;

    // Instructions
    void OP_00E0(), OP_00EE(), OP_1NNN(), OP_2NNN(), OP_3XNN(), OP_4XNN(), OP_5XY0(), OP_6XNN(), OP_7XNN();
    void OP_8XY0(), OP_8XY1(), OP_8XY2(), OP_8XY3(), OP_8XY4(), OP_8XY5(), OP_8XY6(), OP_8XY7(), OP_8XYE();
    void OP_9XY0(), OP_ANNN(), OP_BNNN(), OP_CXNN(), OP_DXYN(), OP_EX9E(), OP_EXA1();
    void OP_FX07(), OP_FX0A(), OP_FX15(), OP_FX18(), OP_FX1E(), OP_FX29(), OP_FX33(), OP_FX55(), OP_FX65();

    // Get masked opcode. Make it possible to look up instructions in the lookup table
    [[nodiscard]] static uint16_t GetMaskedOpcode(uint16_t opcode) ;

    // Produces a map of strings, with keys equivalent to instruction start locations
    // in memory, for the specified address range
    typedef std::map<uint16_t, std::string> disassembly_t;
    disassembly_t disassemble(uint16_t nStart, uint16_t nStop) const;
};

inline void
Chip8::SetKey(uint8_t key, bool state)
{
    cuAssert(key < 16 && "Invalid key");
    m_c8.KP[key] = state;
}

inline bool
Chip8::GetDrawFlag()
{
    return m_c8.DF;
}

inline bool*
Chip8::GetDisplay()
{
    return m_c8.DP;
}

inline uint8_t*
Chip8::GetMemory()
{
    return m_c8.RAM;
}

inline uint8_t*
Chip8::GetV()
{
    return m_c8.V;
}

inline uint16_t
Chip8::GetI()
{
    return m_c8.I;
}

inline uint16_t
Chip8::GetPC()
{
    return m_c8.PC;
}

inline uint8_t
Chip8::GetSP()
{
    return m_c8.SP;
}

inline uint16_t*
Chip8::GetStack()
{
    return m_c8.STACK;
}

inline uint8_t
Chip8::GetDelayTimer()
{
    return m_c8.DT;
}

inline uint8_t
Chip8::GetSoundTimer()
{
    return m_c8.ST;
}

inline uint8_t*
Chip8::GetKeyboard()
{
    return m_c8.KP;
}

inline Chip8::disassembly_t
Chip8::GetDisassembled() const
{
    return disassemble(PROG_START, PROG_END);
}

#endif //CHIP0U_CHIP8_H
