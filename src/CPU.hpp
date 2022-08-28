#pragma once

#include <bits/stdint-uintn.h>
#include <cstdint>
#include <Bus.hpp>

class CPU
{
private:
    uint32_t d[8];
    uint32_t a[8];
    uint32_t pc, pc0;
    uint16_t cond_code;
    Bus* bus;

    enum FLAGS
    {
        C_FLAG = 1 << 0,
        V_FLAG = 1 << 1,
        Z_FLAG = 1 << 2,
        N_FLAG = 1 << 3,
        X_FLAG = 1 << 4
    };

    void SetFlag(FLAGS flag, bool f)
    {
        if (f)
            cond_code |= flag;
        else 
            cond_code &= ~flag;
    }

    bool GetFlag(FLAGS f)
    {
        return cond_code & f;
    }

    void b(uint16_t instr);
    void code_4(uint16_t instr);
    void cmpi(uint16_t instr);

    void WriteReg(int index, uint32_t data)
    {
        if (index < 8)
            d[index] = data;
        else
            a[index - 8] = data;
    }
public:
    CPU(Bus* bus);

    void Clock();
    void Dump();
};