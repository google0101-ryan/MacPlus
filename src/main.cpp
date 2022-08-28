#include "CPU.hpp"
#include <Bus.hpp>
#include <bits/stdint-uintn.h>

int main() 
{
    Bus* bus = new Bus("../mac.rom");

    printf("Created bus\n");

    printf("0x%08x\n", (bus->read<uint16_t>(6) & ~1) | bus->read<uint16_t>(4) << 16);

    CPU* cpu = new CPU(bus);

    while (1)
        cpu->Clock();
}