#ifndef LOAD_ROM_H
#define LOAD_ROM_H

#include <stdint.h>

extern uint8_t base[];
extern const uint8_t base_g850[];

// ROMを読み込む (下請け)
int loadROM(const char *dir);

// EXROMを読み込む (下請け)
int loadExROM(const char *dir);

// RAMを読み込む (下請け)
int loadRAM(const char *path);

#endif  // LOAD_ROM_H
