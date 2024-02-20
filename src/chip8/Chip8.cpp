#include "Chip8.h"

#include <random>

#include <chrono>
#include <iostream>

static constexpr uint8_t FONT_SET[FONT_SET_SIZE]
{
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


Chip8::Chip8()
{
    // Populate instructions map
    using c8 = Chip8;
    m_lookup =
    {
        /** Clearing and Returning Instructions */
        {0x00E0, {"CLS", &c8::OP_00E0}}, {0x00EE, {"RET", &c8::OP_00EE}},
        /** Jump Instructions */
        {0x1000, {"JP", &c8::OP_1NNN}},
        /** Call Instructions */
        {0x2000, {"CALL", &c8::OP_2NNN}},
        /** Skip Instructions */
        {0x3000, {"SE", &c8::OP_3XNN}}, {0x4000, {"SNE", &c8::OP_4XNN}}, {0x5000, {"SE", &c8::OP_5XY0}},
        /** Load and Add Instructions */
        {0x6000, {"LD", &c8::OP_6XNN}}, {0x7000, {"ADD", &c8::OP_7XNN}},
        /** Register Instructions */
        {0x8000, {"LD", &c8::OP_8XY0}}, {0x8001, {"OR", &c8::OP_8XY1}}, {0x8002, {"AND", &c8::OP_8XY2}}, {0x8003, {"XOR", &c8::OP_8XY3}}, {0x8004, {"ADD", &c8::OP_8XY4}}, {0x8005, {"SUB", &c8::OP_8XY5}}, {0x8006, {"SHR", &c8::OP_8XY6}}, {0x8007, {"SUBN", &c8::OP_8XY7}}, {0x800E, {"SHL", &c8::OP_8XYE}},
        /** Skip Instructions */
        {0x9000, {"SNE", &c8::OP_9XY0}},
        /** Load Instructions */
        {0xA000, {"LD", &c8::OP_ANNN}},
        /** Jump Instructions */
        {0xB000, {"JP", &c8::OP_BNNN}},
        /** Random Number Instructions */
        {0xC000, {"RND", &c8::OP_CXNN}},
        /** Draw Instructions */
        {0xD000, {"DRW", &c8::OP_DXYN}},
        /** Skip Instructions */
        {0xE09E, {"SKP", &c8::OP_EX9E}}, {0xE0A1, {"SKNP", &c8::OP_EXA1}},
        /** Timer and Load Instructions */
        {0xF007, {"LD", &c8::OP_FX07}}, {0xF00A, {"LD", &c8::OP_FX0A}}, {0xF015, {"LD", &c8::OP_FX15}}, {0xF018, {"LD", &c8::OP_FX18}}, {0xF01E, {"ADD", &c8::OP_FX1E}}, {0xF029, {"LD", &c8::OP_FX29}}, {0xF033, {"LD", &c8::OP_FX33}}, {0xF055, {"LD", &c8::OP_FX55}}, {0xF065, {"LD", &c8::OP_FX65}}
    };

    // Initialize the Chip8
    Reset();
}



void
Chip8::LoadGame(const char* filename)
{
    Reset();

    // Open the file
    FILE* file = fopen(filename, "rb");
    if (file == nullptr)
    {
        printf("File not found\n");
        return;
    }

    // Check file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    // Allocate memory to store the file
    char* buffer = (char*)malloc(sizeof(char) * size);
    if (buffer == nullptr)
    {
        printf("Memory error\n");
        return;
    }

    // Copy the file into the buffer
    size_t result = fread(buffer, 1, size, file);
    if (result != size)
    {
        printf("Reading error\n");
        return;
    }

    // Copy the buffer into the Chip8 memory
    if ((4096 - 512) > size)
    {
        for (int i = 0; i < size; ++i)
        {
            m_c8.RAM[i + 512] = buffer[i];
        }
    }
    else
    {
        printf("Error: ROM too big for memory\n");
        printf("Oversize: %ld\n", size - (4096 - 512));
        printf("ROM size: %ld, Memory size: %d\n", size, 4096 - 512);
    }

    // Close the file and free the buffer
    fclose(file);
    free(buffer);
}

void
Chip8::Clock()
{
    // Fetch current instruction
    {
        m_instr = instruction_t(m_c8.RAM[m_c8.PC] << 8 | m_c8.RAM[m_c8.PC + 1]);

        m_c8.PC += 2; // Move to next instruction
    }

    // Decode opcode
    {
        // Remap opcodes to instructions map
        // Necessary because some opcodes are masked in the instructions map
        // Maybe there's another way to do this, but... nhe... vamo deixá assim memo
        uint16_t masked_opcode = GetMaskedOpcode(m_instr.OP);

        // Execute instruction
        auto it = m_lookup.find(masked_opcode);
        if (it != m_lookup.end())
        {
            (this->*it->second.function)();
        }
    }

    // Update timers
    if (m_c8.DT > 0) --m_c8.DT;
    if (m_c8.ST > 0)
    {
        if (m_c8.ST == 1) printf("BEEP!\n");
        --m_c8.ST;
    }
}

void
Chip8::Reset()
{
    // Initialize registers and memory once
    m_c8.PC     = PROG_START; // Program counter starts at 0x200
    m_c8.I      = 0;     // Reset index register
    m_c8.SP     = 0;     // Reset stack pointer
    m_instr     = {0};   // Reset current opcode

    // Clear display memory
    memset(m_c8.DP, 0, sizeof(m_c8.DP));

    // Clear stack memory
    memset(m_c8.STACK, 0, sizeof(m_c8.STACK));

    // Clear register memory
    memset(m_c8.V, 0, sizeof(m_c8.V));

    // Clear memory
    memset(m_c8.RAM, 0, sizeof(m_c8.RAM));

    // Clear keypad memory
    memset(m_c8.KP, 0, sizeof(m_c8.KP));

    // Load fontset into memory (0x000 - 0x1FF)
    {
        for (int i = 0; i < 16 * 5; ++i) m_c8.RAM[i] = FONT_SET[i];
    }

    // Reset timers
    m_c8.DT = 0; m_c8.ST = 0;

    // Clear display
    m_c8.DF = true;
}

// Instructions
// --------------------------------------------------------------------------------

void
Chip8::OP_00E0()
{
    // Clear the display
    memset(m_c8.DP, 0, sizeof(m_c8.DP));
    m_c8.DF = true;
}

void
Chip8::OP_00EE()
{
    // Return from a subroutine
    --m_c8.SP;
    m_c8.PC = m_c8.STACK[m_c8.SP];
}

void
Chip8::OP_1NNN()
{
    // Jump to address NNN
    m_c8.PC = m_instr.NNN;
}

void
Chip8::OP_2NNN()
{
    // Call subroutine at NNN
    m_c8.STACK[m_c8.SP] = m_c8.PC;
    ++m_c8.SP;
    m_c8.PC = m_instr.NNN;
}

void
Chip8::OP_3XNN()
{
    // Skip next instruction if Vx == NN
    if (m_c8.V[m_instr.X] == m_instr.NN)
    {
        m_c8.PC += 2;
    }
}

void
Chip8::OP_4XNN()
{
    // Skip next instruction if Vx != NN
    if (m_c8.V[m_instr.X] != m_instr.NN)
    {
        m_c8.PC += 2;
    }
}

void
Chip8::OP_5XY0()
{
    // Skip next instruction if Vx == Vy
    if (m_c8.V[m_instr.X] == m_instr.NN)
    {
        m_c8.PC += 2;
    }
}

void
Chip8::OP_6XNN()
{
    // Set Vx = NN
    m_c8.V[m_instr.X] = m_instr.NN;
}

void
Chip8::OP_7XNN()
{
    // Set Vx = Vx + NN
    m_c8.V[m_instr.X] += m_instr.NN;
}

void
Chip8::OP_8XY0()
{
    // Set Vx = Vy
    m_c8.V[m_instr.X] = m_c8.V[m_instr.Y];
}

void
Chip8::OP_8XY1()
{
    // Set Vx = Vx OR Vy
    m_c8.V[m_instr.X] |= m_c8.V[m_instr.Y];
}

void
Chip8::OP_8XY2()
{
    // Set Vx = Vx AND Vy
    m_c8.V[m_instr.X] &= m_c8.V[m_instr.Y];
}

void
Chip8::OP_8XY3()
{
    // Set Vx = Vx XOR Vy
    m_c8.V[m_instr.X] ^= m_c8.V[m_instr.Y];
}

void
Chip8::OP_8XY4()
{
    // Set Vx = Vx + Vy, set VF = carry
    if (m_c8.V[m_instr.Y] > (0xFF - m_c8.V[m_instr.X]))
    {
        m_c8.V[0xF] = 1; // carry
    }
    else
    {
        m_c8.V[0xF] = 0;
    }

    m_c8.V[m_instr.X] += m_c8.V[m_instr.Y];
}

void
Chip8::OP_8XY5()
{
    // Set Vx = Vx - Vy, set VF = NOT borrow
    if (m_c8.V[m_instr.Y] > m_c8.V[m_instr.X])
    {
        m_c8.V[0xF] = 0; // there is a borrow
    }
    else
    {
        m_c8.V[0xF] = 1;
    }

    m_c8.V[m_instr.X] -= m_c8.V[m_instr.Y];
}

void
Chip8::OP_8XY6()
{
    // Set Vx = Vx SHR 1
    m_c8.V[0xF] = m_c8.V[m_instr.X] & 0x1;
    m_c8.V[m_instr.X] >>= 1;
}

void
Chip8::OP_8XY7()
{
    // Set Vx = Vy - Vx, set VF = NOT borrow
    if (m_c8.V[m_instr.X] > m_c8.V[m_instr.Y])
    {
        m_c8.V[0xF] = 0; // there is a borrow
    }
    else
    {
        m_c8.V[0xF] = 1;
    }

    m_c8.V[m_instr.X] = m_c8.V[m_instr.Y] - m_c8.V[m_instr.X];
}

void
Chip8::OP_8XYE()
{
    // Set Vx = Vx SHL 1
    m_c8.V[0xF] = m_c8.V[m_instr.X] >> 7;
    m_c8.V[m_instr.X] <<= 1;
}

void
Chip8::OP_9XY0()
{
    // Skip next instruction if Vx != Vy
    if (m_c8.V[m_instr.X] != m_c8.V[m_instr.Y])
    {
        m_c8.PC += 2;
    }
}

void
Chip8::OP_ANNN()
{
    // Set I = NNN
    m_c8.I = m_instr.NNN;
}

void
Chip8::OP_BNNN()
{
    // Jump to location NNN + V0
    m_c8.PC = m_instr.NNN + m_c8.V[0];
}

void
Chip8::OP_CXNN()
{
    // Set Vx = random byte AND NN
    std::random_device rd;
    std::mt19937 gen(rd());
    gen.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> distrib(0, 0xFF);

    m_c8.V[m_instr.X] = distrib(gen) & m_instr.NN;
}

void
Chip8::OP_DXYN()
{
    // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
    uint8_t spriteX = m_c8.V[m_instr.X];
    uint8_t spriteY = m_c8.V[m_instr.Y];
    uint8_t spriteHeight = m_instr.N;
    uint8_t pixel;

    m_c8.V[0xF] = 0; // Reset collision flag
    for (int currentLine = 0; currentLine < spriteHeight; currentLine++)
    {
        pixel = m_c8.RAM[m_c8.I + currentLine];
        for (int currentPixel = 0; currentPixel < 8; currentPixel++)
        {
            // Determine if the current pixel will flip
            if ((pixel & (0x80 >> currentPixel)) != 0)
            {
                int displayIndex = (spriteX + currentPixel) + ((spriteY + currentLine) * 64);
                if (m_c8.DP[displayIndex] == 1)
                {
                    m_c8.V[0xF] = 1; // Set collision flag
                }
                m_c8.DP[displayIndex] ^= 1; // Flip the pixel at the displayIndex
            }
        }
    }

    m_c8.DF = true;
}

void
Chip8::OP_EX9E()
{
    // Skip next instruction if key with the value of Vx is pressed
    uint8_t key = m_c8.V[m_instr.X];
    if (m_c8.KP[key] != 0)
    {
        m_c8.PC += 2;
    }
}

void
Chip8::OP_EXA1()
{
    // Skip next instruction if key with the value of Vx is not pressed
    uint8_t key = m_c8.V[m_instr.X];
    if (m_c8.KP[key] == 0)
    {
        m_c8.PC += 2;
    }
}

void
Chip8::OP_FX07()
{
    // Set Vx = delay timer value
    m_c8.V[m_instr.X] = m_c8.DT;
}

void
Chip8::OP_FX0A()
{
    // Wait for a key press, store the value of the key in Vx
    bool keyPress = false;
    for (int i = 0; i < 16; ++i)
    {
        if (m_c8.KP[i] != 0)
        {
            m_c8.V[m_instr.X] = i;
            keyPress = true;
        }
    }

    // If no key is pressed, return and try again
    if (!keyPress)
    {
        m_c8.PC -= 2;
    }
}

void
Chip8::OP_FX15()
{
    // Set delay timer = Vx
    m_c8.DT = m_c8.V[m_instr.X];
}

void
Chip8::OP_FX18()
{
    // Set sound timer = Vx
    m_c8.ST = m_c8.V[m_instr.X];
}

void
Chip8::OP_FX1E()
{
    // Set I = I + Vx
    if (m_c8.I + m_c8.V[m_instr.X] > 0xFFF)
    {
        m_c8.V[0xF] = 1;
    }
    else
    {
        m_c8.V[0xF] = 0;
    }

    m_c8.I += m_c8.V[m_instr.X];
}

void
Chip8::OP_FX29()
{
    // Set I = location of sprite for digit Vx
    m_c8.I = m_c8.V[m_instr.X] * 0x5;
}

void
Chip8::OP_FX33()
{
    // Store BCD representation of Vx in memory locations I, I+1, and I+2
    uint8_t Vx = m_c8.V[m_instr.X];
    m_c8.RAM[m_c8.I]     = Vx / 100;
    m_c8.RAM[m_c8.I + 1] = (Vx / 10) % 10;
    m_c8.RAM[m_c8.I + 2] = (Vx % 100) % 10;
}

void
Chip8::OP_FX55()
{
    // Store registers V0 through Vx in memory starting at location I
    for (int i = 0; i <= m_instr.X; ++i)
    {
        m_c8.RAM[m_c8.I + i] = m_c8.V[i];
    }

    // On the original interpreter, when the operation is done, I = I + X + 1
    m_c8.I += m_instr.X + 1;
}

void
Chip8::OP_FX65()
{
    // Read registers V0 through Vx from memory starting at location I
    for (int i = 0; i <= m_instr.X; ++i)
    {
        m_c8.V[i] = m_c8.RAM[m_c8.I + i];
    }

    // On the original interpreter, when the operation is done, I = I + X + 1
    m_c8.I += m_instr.X + 1;
}

// --------------------------------------------------------------------------------

uint16_t
Chip8::GetMaskedOpcode(uint16_t opcode)
{
    uint16_t masked_opcode = opcode & 0xF000;

    switch (masked_opcode >> 12)
    {
        case 0x0: masked_opcode = (opcode & 0x00F0) | (opcode & 0x000F); break;
        case 0x8: masked_opcode = opcode & 0xF00F; break;

        case 0xE:
        case 0xF: masked_opcode = opcode & 0xF0FF; break;
        default: break;
    }

    return masked_opcode;
}

Chip8::disassembly_t
Chip8::disassemble(uint16_t nStart, uint16_t nStop) const
{
    uint32_t addr = nStart;
    std::map<uint16_t, std::string> mapLines{};
    instruction_t instr{0};
    uint16_t line_addr = 0;
    std::string sInst{};

    // By David Barr, aka javidx9
    auto hex = [](uint32_t n, uint8_t d)
    {
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];
        return s;
    };

    // Mounts a human-readable version of the program
    while (addr <= nStop)
    {
        line_addr = addr;

        instr = instruction_t(m_c8.RAM[addr] << 8 | m_c8.RAM[addr + 1]); addr += 2;
        uint16_t masked_opcode = GetMaskedOpcode(instr.OP);

        sInst = "$" + hex(line_addr, 4) + ": ";
        sInst += hex(instr.OP, 4) + std::string(3, ' ');

        const auto it = m_lookup.find(masked_opcode);
        if (it != m_lookup.end())
        {
            sInst += it->second.name;

            switch ((instr.OP & 0xF000) >> 12)
            {
                case 0x0: break;
                case 0x1:
                case 0x2:
                case 0xA:
                case 0xB: sInst += " $" + hex(instr.NNN, 3); break;
                case 0x3:
                case 0x4:
                case 0x6:
                case 0x7:
                case 0xC: sInst += " V" + hex(instr.X, 1) + ", #" + hex(instr.NN, 2); break;
                case 0x5:
                case 0x9:
                case 0x8: sInst += " V" + hex(instr.X, 1) + ", V" + hex(instr.Y, 1); break;
                case 0xD: sInst += " V" + hex(instr.X, 1) + ", V" + hex(instr.Y, 1) + ", #" + hex(instr.N, 1); break;
                case 0xE:
                case 0xF: sInst += " V" + hex(instr.X, 1); break;
                default: break;
            }
        }
        else // Unknown opcode
        {
            sInst += "XXX";
        }

        mapLines[line_addr] = sInst;
    }
    return mapLines;
}
