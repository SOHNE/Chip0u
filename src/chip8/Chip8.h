#ifndef CHIP0U_CHIP8_H
#define CHIP0U_CHIP8_H

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <map>
#include <vector>

#define TOTAL_RAM       4096
#define TOTAL_REGISTERS 16
#define STACK_SIZE      16

#define DISPLAY_WIDTH   64
#define DISPLAY_HEIGHT  32
#define DISPLAY_SIZE    (DISPLAY_WIDTH * DISPLAY_HEIGHT)

#define FONTSET_SIZE    80

#define PROG_START      0x200
#define PROG_END        0xFFF

class Chip8
{
public:
    // CHIP8 Instruction format
    typedef struct instruction_t
    {
        uint16_t    OP; // 16 bit opcode
        uint16_t    NNN;    // 12 bit constant
        uint8_t     NN;     // 8 bit constant
        uint8_t     N;      // 4 bit constant
        uint8_t     X;      // 4 bit register identifier
        uint8_t     Y;      // 4 bit register identifier
    } instruction_t;

public:
    Chip8();
    ~Chip8() = default;

    void Initialize();
    void LoadGame(const char* filename);
    void Clock();
    void Reset();

    inline void SetKey(uint8_t key, bool state) { m_c8.KP[key] = state; }

    // Getters
    inline bool         GetDrawFlag()   { return m_c8.DF; }
    inline bool*        GetDisplay()    { return m_c8.DP; }
    inline uint8_t*     GetMemory()     { return m_c8.RAM; }
    inline uint8_t*     GetV()          { return m_c8.V; }
    inline uint16_t     GetI()          { return m_c8.I; }
    inline uint16_t     GetPC()         { return m_c8.PC; }
    inline uint8_t      GetSP()         { return m_c8.SP; }
    inline uint16_t*    GetStack()      { return m_c8.STACK; }
    inline uint8_t      GetDelayTimer() { return m_c8.DT; }
    inline uint8_t      GetSoundTimer() { return m_c8.ST; }
    inline uint8_t*     GetKeyboard()   { return m_c8.KP; }

    // Get disassembled instructions
    std::map<uint16_t, std::string> GetDisassembled() const { return disassemble(0x200, 0xFFF); }

private:
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

    // Current state of the CHIP8
    chip8_t m_c8;

    // Instructions
    instruction_t m_instr;

    // Instructions map to function pointers
    typedef struct instruction_map_t
    {
        std::string name;   // Name of the instruction
        void        (Chip8::*function)(void) = nullptr; // Function pointer
    };

    std::map<uint16_t, instruction_map_t> m_lookup;

    // Instructions
    void OP_00E0(), OP_00EE(), OP_1NNN(), OP_2NNN(), OP_3XNN(), OP_4XNN(), OP_5XY0(), OP_6XNN(), OP_7XNN();
    void OP_8XY0(), OP_8XY1(), OP_8XY2(), OP_8XY3(), OP_8XY4(), OP_8XY5(), OP_8XY6(), OP_8XY7(), OP_8XYE();
    void OP_9XY0(), OP_ANNN(), OP_BNNN(), OP_CXNN(), OP_DXYN(), OP_EX9E(), OP_EXA1();
    void OP_FX07(), OP_FX0A(), OP_FX15(), OP_FX18(), OP_FX1E(), OP_FX29(), OP_FX33(), OP_FX55(), OP_FX65();

    // Get masked opcode
    uint16_t GetMaskedOpcode(uint16_t opcode) const;

    // Produces a map of strings, with keys equivalent to instruction start locations
    // in memory, for the specified address range
    typedef std::map<uint16_t, std::string> disassembly_t;
    disassembly_t disassemble(uint16_t nStart, uint16_t nStop) const;
};

#endif //CHIP0U_CHIP8_H
