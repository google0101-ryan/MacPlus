#pragma once

#include <bits/stdint-uintn.h>
#include <cstdint>
#include <string>
#include <bit>

inline uint16_t byteswap(uint16_t val)
{
    unsigned char* cp = (unsigned char*)&val;
    unsigned char tmp = cp[0];
    cp[0] = cp[1];
    cp[1] = tmp;
    return *(uint16_t*)cp;
}

inline uint32_t byteswap32(uint32_t val)
{
    uint8_t hi = (val & 0xFF000000) >> 24;
    uint8_t midhi = (val & 0xFF0000) >> 16;
    uint8_t midlo = (val & 0xFF00) >> 8;
    uint8_t lo = (val & 0xFF);

    return (lo << 24) | (midlo << 16) | (midhi << 8) | hi;
}

class Bus
{
private:
    uint8_t* rom_low;
    uint8_t* rom_high;
public:
    Bus(std::string fileName);

    uint8_t read8(uint32_t addr)
    {
        uint8_t value = 0;
        if (addr < 0x1FFFF)
            value = rom_low[addr];
        else if (addr >= 0x400000 && addr <= 0x41FFFF)
            value = rom_high[addr - 0x400000];
        else if (addr >= 0xf80000)
        {
            return 0;
        }
        else 
        {
            printf("[ERROR]: Couldn't read from 0x%08x\n", addr);
            exit(1);
        }
        return value;
    }

    uint16_t read16(uint32_t addr)
    {
        uint16_t value = 0;

        if (addr < 0x1FFFF)
        {
            value = rom_low[addr] << 8;
            value |= rom_low[addr+1];
        }
        else if (addr >= 0x400000 && addr <= 0x41FFFF)
        {
            value = rom_low[addr-0x400000] << 8;
            value |= rom_low[addr+1-0x400000];
        }
        else if (addr >= 0xf80000)
        {
            return 0;
        }
        else 
        {
            printf("[ERROR]: Couldn't read from 0x%08x\n", addr);
            exit(1);
        }
        return value;
    }

    uint32_t read32(uint32_t addr)
    {
        uint32_t value = 0;

        if (addr < 0x1FFFF)
        {
            value = rom_low[addr] << 24;
            value |= rom_low[addr+1] << 16;
            value |= rom_low[addr+2] << 8;
            value |= rom_low[addr+3];
        }
        else if (addr >= 0x400000 && addr <= 0x41FFFF)
        {
            value = rom_low[addr-0x400000] << 24;
            value |= rom_low[addr+1-0x400000] << 16;
            value |= rom_low[addr+2-0x400000] << 8;
            value |= rom_low[addr+3-0x400000];
        }
        else if (addr >= 0xf80000)
        {
            return 0;
        }
        else 
        {
            printf("[ERROR]: Couldn't read from 0x%08x\n", addr);
            exit(1);
        }
        return value;
    }

    template<typename T>
    T read(uint32_t addr)
    {
        switch (sizeof(T)*8)
        {
        case 8:
            return read8(addr);
        case 16:
            return read16(addr);
        case 32:
            return read32(addr);
        default:
            return 0;
        }
    }
};