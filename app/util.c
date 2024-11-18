#include "stm32f4xx_hal.h"
#define IO_READ_32(addr) (*(volatile uint32_t *)addr)
#define IO_WRITE_32(addr, value) (*(volatile uint32_t *)addr = value)

uint32_t do_memory_read(uint32_t addr)
{
    return IO_READ_32(addr);
}

void do_memory_write(uint32_t addr, uint32_t value)
{
    IO_WRITE_32(addr, value);
}