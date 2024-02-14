#ifndef CHIPO_CHIP8_H
#define CHIPO_CHIP8_H

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <map>
#include <vector>

class Chip8
{
public:
    // CHIP8 Instruction format
    typedef struct
    {
        uint16_t opcode;
        uint16_t NNN;   // 12 bit address/constant
        uint8_t NN;     // 8 bit constant
        uint8_t N;      // 4 bit constant
        uint8_t X;      // 4 bit register identifier
        uint8_t Y;      // 4 bit register identifier
    } instruction_t;

public:
    Chip8();
    ~Chip8() = default;

    void Initialize();
    void LoadGame(const char* filename);
    void Clock();
    void Reset(bool reloadROM = true);

    inline void SetKey(uint8_t key, bool state) { keyboard[key] = state; }

    // get display array
    inline bool GetDrawFlag() { return drawFlag; }
    inline bool* GetDisplay() { return display; }

    // Get Memory
    inline uint8_t* GetMemory() { return memory; }

    // Get registers
    inline uint8_t* GetV() { return V; }
    inline uint16_t GetI() { return I; }
    inline uint16_t GetPC() { return PC; }
    inline uint8_t GetSP() { return SP; }
    inline uint16_t* GetStack() { return Stack; }
    inline uint8_t GetDelayTimer() { return delay_timer; }
    inline uint8_t GetSoundTimer() { return sound_timer; }
    inline uint8_t* GetKeyboard() { return keyboard; }

    // Get disassembled instructions
    std::map<uint16_t, std::string> GetDisassembled() const { return disassemble(0x200, 0xFFF); }

private:
    // RAM 4KB
    uint8_t memory[4 * 1024];

    // Loaded 'ROM' filename
    const char* romFilename;

    // Display 64x32
    bool display[64 * 32];
    bool drawFlag;

    // Stack
    uint16_t Stack[16];
    uint8_t SP;     // Stack pointer

    // Registers
    uint8_t V[16];  // V0 - VF
    uint16_t I;     // Index register
    uint16_t PC;    // Program counter

    // Timers
    uint8_t delay_timer;
    uint8_t sound_timer;

    // Keyboard
    uint8_t keyboard[16];   // 0x0 - 0xF

    // Instructions
    instruction_t Instruction;

    // Instructions map to function pointers
    struct instruction_map_t
    {
        //name
        std::string name;
        void        (Chip8::*function)(void) = nullptr;
    };
    std::map<uint16_t, instruction_map_t> loopup;

    // Instructions
    void OP_00E0(), OP_00EE(), OP_1NNN(), OP_2NNN(), OP_3XNN(), OP_4XNN(), OP_5XY0(), OP_6XNN(), OP_7XNN();
    void OP_8XY0(), OP_8XY1(), OP_8XY2(), OP_8XY3(), OP_8XY4(), OP_8XY5(), OP_8XY6(), OP_8XY7(), OP_8XYE();
    void OP_9XY0(), OP_ANNN(), OP_BNNN(), OP_CXNN(), OP_DXYN(), OP_EX9E(), OP_EXA1();
    void OP_FX07(), OP_FX0A(), OP_FX15(), OP_FX18(), OP_FX1E(), OP_FX29(), OP_FX33(), OP_FX55(), OP_FX65();

    // Ilegal instruction
    void OP_XXX();

    // Produces a map of strings, with keys equivalent to instruction start locations
    // in memory, for the specified address range
    std::map<uint16_t, std::string> disassemble(uint16_t nStart, uint16_t nStop) const;
};

#endif //CHIPO_CHIP8_H
