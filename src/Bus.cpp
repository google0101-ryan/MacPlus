#include <Bus.hpp>
#include <bits/stdint-uintn.h>
#include <cstddef>
#include <fstream>
#include <cstring>

Bus::Bus(std::string fileName)
{
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);
    std::ofstream out("out.bin");
    std::ofstream out_hi("out_hi.bin");

    out.clear();
    out_hi.clear();

    size_t size = file.tellg();

    if (size < 0)
    {
        printf("[ERROR]: Couldn't open file %s\n", fileName.c_str());
        exit(1);
    }

    rom_low = new uint8_t[0xFFFFFF];
    rom_high = new uint8_t[0xFFFFFF];

    memset(rom_low, 0, sizeof(0xFFFFFF));
    memset(rom_high, 0, sizeof(0xFFFFFF));

    file.seekg(0, std::ios::beg);
    file.read((char*)rom_low, std::min(size, 0xFFFFFFUL));

    file.seekg(0, std::ios::beg);
    file.read((char*)rom_high, std::min(size, 0xFFFFFFUL));

    file.close();

    out.write((char*)rom_low, std::min(size, 0xFFFFFFUL));
    out.close();

    out_hi.write((char*)rom_high, std::min(size, 0xFFFFFFUL));
    out_hi.close();

    printf("0x%08x\n", *(uint32_t*)&rom_low[4]);
}