#include <CPU.hpp>
#include <bits/stdint-intn.h>
#include <bits/stdint-uintn.h>
#include <cstddef>
#include <ios>
#include <sstream>
#include <string>

CPU::CPU(Bus* bus)
: bus(bus)
{
    pc = bus->read<uint32_t>(4);
}

void CPU::Clock()
{
    pc0 = pc;
    uint16_t opcode = bus->read<uint16_t>(pc);
    pc += 2;

    uint8_t func = (opcode >> 12) & 0xF;
    switch (func)
    {
    case 0:
    {
        if ((opcode >> 8) == 0b00001100)
        {
            cmpi(opcode);
        }
        else 
        {
            printf("ERROR: Unknown opcode 0 %d\n", opcode);
            Dump();
            exit(1);
        }
    }
    break;
    case 4:
        code_4(opcode);
        break;
    case 6:
        b(opcode);
        break;
    default:
        printf("Unknown opcode 0x%01x (0x%04x)\n", func, opcode);
        Dump();
        exit(1);
    }
}

void CPU::b(uint16_t opcode)
{
    uint8_t cond = (opcode >> 8) & 0xF;
    int16_t off = (int16_t)(uint16_t)(opcode & 0xFF);

    if (!off)
    {
        off = bus->read<int16_t>(pc);
        pc0 = pc;
        pc += 2;
    }

    if (cond == 0)
    {
        printf("bra 0x%08x\n", pc0 + off);
        pc = pc0 + off;
    }
    else if (cond == 6)
    {
        printf("bne 0x%08x (%s)\n", pc + off, GetFlag(Z_FLAG) ? "ignored" : "taken");
        if (!GetFlag(Z_FLAG))
            pc += off;
    }
    else 
    {
        printf("[ERROR]: Unknown cond code %d\n", cond);
        Dump();
        exit(1);
    }
}

std::string hex_string(uint32_t hex)
{
    std::ostringstream ss;
    ss << "0x" << std::hex << hex;
    return ss.str();
}

void CPU::code_4(uint16_t instr)
{
    uint8_t opcode = (instr >> 7) & 0x1FF;

    if (opcode == 0b010011001)
    {
        std::string disasm = "movem.l ";

        int src = instr & 7;
        uint16_t mask = bus->read<uint16_t>(pc);
        pc0 = pc;
        pc += 2;

        uint8_t m = (instr >> 3) & 7;
        uint8_t xn = instr & 7;

        uint32_t operand;

        switch (m)
        {
        case 7:
        {
            switch (xn)
            {
            case 1:
            {
                operand = bus->read<uint32_t>(pc);
                disasm += "(" + hex_string(operand) + ")";
                pc0 = pc;
                pc += 4;
                break;
            }
            default:
                printf("Unknown xn val %d\n", xn);
                Dump();
                exit(1);
            }
        }
        break;
        default:
            printf("Unknown M val %d\n", m);
            Dump();
            exit(1);
        }

        for (int i = 0; i <= 15; i++)
        {
            if (mask & (1 << i))
            {
                WriteReg(i, bus->read<uint32_t>(operand));
                operand += 4;
                disasm += ", ";
                if (i < 8)
                    disasm += "d" + std::to_string(i);
                else
                    disasm += "a" + std::to_string(i-8);
            }
        }

        printf("%s\n", disasm.c_str());
    }
    else if (((instr >> 6) & 0b1111111111) == 0b0100011011)
    {
        uint8_t m = (instr >> 3) & 7;
        uint8_t xn = instr & 7;

        uint32_t operand = 0;

        switch (m)
        {
        case 7:
        {
            switch (xn)
            {
            case 1:
            {
                operand = bus->read<uint32_t>(pc);
                pc0 = pc;
                pc += 4;
                break;
            }
            case 4:
            {
                operand = bus->read<uint16_t>(pc);
                pc0 = pc;
                pc += 2;
                break;
            }
            default:
                printf("Unknown xn val %d\n", xn);
                Dump();
                exit(1);
            }
        }
        break;
        default:
            printf("Unknown M val %d\n", m);
            Dump();
            exit(1);
        }

        cond_code = operand;

        printf("move sr, 0x%08x\n", cond_code);
    }
    else 
    {
        printf("ERROR: Unknown opcode 4 0x%04x\n", instr);
        Dump();
        exit(1);
    }
}

void CPU::cmpi(uint16_t instr)
{
    uint8_t s = (instr >> 6) & 0b11;
    uint8_t m = (instr >> 3) & 0b111; 

    std::string op;

    if (s == 2)
    {
        uint32_t operand = 0;
        switch (m)
        {
        case 0:
        {
            uint8_t rn = instr & 7;
            operand = d[rn];
            op = "d" + std::to_string(rn);
        }
        break;
        default:
            printf("ERROR: cmpi: Unknown m type %d\n", m);
            Dump();
            exit(1);
        }

        uint32_t imm = bus->read<uint32_t>(pc);
        pc0 = pc;
        pc += 4;

        uint32_t result = operand - imm;

        SetFlag(C_FLAG, imm > operand);
        SetFlag(N_FLAG, result & 0x80000000);
        SetFlag(Z_FLAG, result == 0);

        printf("cmpi %s, 0x%08x\n", op.c_str(), imm);
    }
}

void CPU::Dump()
{
    printf("[%s%s%s]\n", GetFlag(C_FLAG) ? "c" : ".", GetFlag(Z_FLAG) ? "z" : ".", GetFlag(N_FLAG) ? "n" : ".");
    printf("pc: 0x%08x\n", pc);
}