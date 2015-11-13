#include "core/memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Global MEMORY state
mem_state_t mem;

void mem_init() {
    mem.flash=(uint8_t*)malloc(0x3FFFFF);     // allocate flash memory
    memset(mem.flash, 0xFF, 0x3FFFFF);

    mem.ram=(uint8_t*)malloc(0x657FF);      // allocate ram memory
    mem.vram=mem.ram+0x40000;              // allocate vram memory
    memset(mem.ram, 0x00, 0x657FF);

    mem.flash_mapped = 0;
    mem.flash_unlocked = 0;
}

void mem_free() {
    free(mem.ram);
    free(mem.flash);
}

uint8_t* phys_mem_ptr(uint32_t addr, uint32_t size) {
    if(addr<0xD00000) {
        if(addr+size < 0x400000) { return NULL; }
           return mem.flash+addr;
    }
    if(addr+size < 0x65800) { return NULL; }
    return mem.ram+addr;
}

// returns wait cycles
uint8_t memory_read_byte(uint32_t address)
{
    address &= 0xFFFFFF;
    int addr = (int)address;

    switch( upperNibble24(addr) ) {

    // FLASH
    case 0x0: case 0x1: case 0x2: case 0x3:
        mem.wait_state += 5;
        return mem.flash[addr];

    // MAYBE FLASH
    case 0x4: case 0x5: case 0x6: case 0x7:
        addr -= 0x400000;
        if(mem.flash_mapped) {
            mem.wait_state += 5;
            return mem.flash[addr];
        }
        mem.wait_state += 257;
        return 0;

    // UNMAPPED
    case 0x8: case 0x9: case 0xA: case 0xB: case 0xC:
        mem.wait_state += 257;
        return 0;

    // RAM
    case 0xD:
        addr -= 0xD00000;
        if(addr <= 0x657FF) {
            mem.wait_state += 3;
            return mem.ram[addr];
        }
    // UNMAPPED
        addr -=  0x65800;
        if(addr <= 0x1A7FF) {
            mem.wait_state += 3;
            return 0;
        }
    // MIRRORED
        return memory_read_byte(address-0x80000);
    case 0xE: case 0xF:
        mem.wait_state += 2;
        return mmio_read_byte(addr);          // read byte from mmio
    default:
        mem.wait_state += 1;
        break;
    }
    return 0;
}

int mem_wait_states() {
     return mem.wait_state;
}

void mem_reset_wait_states() {
     mem.wait_state = 0;
}

void memory_write_byte(uint32_t address, const uint8_t byte)
{
    address &= 0xFFFFFF;
    int addr = (int)address;

    switch( upperNibble24(addr) ) {

    // FLASH
    case 0x0: case 0x1: case 0x2: case 0x3:
        if(mem.flash_unlocked) {
            mem.flash[addr] = byte;
        }
        mem.wait_state += 5;
        return;

    // MAYBE FLASH
    case 0x4: case 0x5: case 0x6: case 0x7:
        addr -= 0x400000;
        if(mem.flash_unlocked) {
          if(mem.flash_mapped) {
              mem.wait_state += 5;
              mem.flash[addr] = byte;
              return;
          }
        }
        mem.wait_state += 257;
        return;

    // UNMAPPED
    case 0x8: case 0x9: case 0xA: case 0xB: case 0xC:
        mem.wait_state += 5;
        return;

    // RAM
    case 0xD:
        addr -= 0xD00000;
        if(addr <= 0x657FF) {
            mem.wait_state += 1;
            mem.ram[addr] = byte;
            mem.wait_state += 1;
            return;
        }
        // UNMAPPED
        addr -=  0x65800;
        if(addr <= 0x1A7FF) {
            mem.wait_state = 1;
            return;
        }
        // MIRRORED
            memory_write_byte(address-0x80000, byte);
            return;
        // MMIO <-> Advanced Perphrial Bus
        case 0xE: case 0xF:
            mem.wait_state = 2;
            mmio_write_byte(addr, byte);         // write byte to the mmio port
            return;
        default:
            mem.wait_state = 1;
          break;
    }
    return;
}