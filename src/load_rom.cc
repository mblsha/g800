#include "load_rom.h"
#include "g800.h"

/* アドレス0000~003fの初期値 */
uint8_t base[] = {
    0xc3, 0xf4, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc9, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xc9, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xc9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc9,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc9, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc3, 0x03, 0xbd, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xc9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* アドレス0038~003aの初期値 (PC-G850) */
const uint8_t base_g850[] = {0xc3, 0x37, 0xbc};

/*
        ROMを1ページ読み込む (下請け)
*/
static int loadROM1page(uint8_t *p, const char *dir, int page, int size) {
  int result;
  char path[FILENAME_MAX];

  if (page < 0)
    sprintf(path, "%s/base.bin", dir);
  else
    sprintf(path, "%s/rom%02x.bin", dir, page);
  if ((result = readBin(path, p, size)) > 0)
    return result;

  if (page < 0)
    sprintf(path, "%s/base.txt", dir);
  else
    sprintf(path, "%s/rom%02x.txt", dir, page);
  if ((result = readHexAbs(path, p, NULL, size, FALSE)) > 0)
    return result;
  return 0;
}

int loadROM(const char *dir) {
  int page;

  if (useROM) {
    /* 0000-003fを読み込む */
    if (loadROM1page(base, dir, -1, 0x40) < 0x40)
      return FALSE;

    /* ROMのページ数を調べる */
    for (romBanks = 0; loadROM1page(NULL, dir, romBanks, 0x4000) == 0x4000;
         romBanks++)
      ;
    if (romBanks == 0)
      return FALSE;

    /* ROMを読み込む */
    rom = (uint8_t*)malloc(0x4000 * romBanks);
    for (page = 0; page != romBanks; page++)
      loadROM1page(ROM(page), dir, page, 0x4000);
  } else {
    int font_off;
    uint8_t *p;

    /* 0000-003fを設定する */
    if (machine == MACHINE_G850)
      memcpy(&base[0x0038], base_g850, sizeof(base_g850));

    /* 擬似ROMのページ数を設定する */
    if (machine == MACHINE_E200) {
      romBanks = 5;
      font_off = 0x4000 * 1 + 0x3886;
    } else if (machine == MACHINE_G815) {
      romBanks = 14;
      font_off = 0x4000 * 4 + 0x1720;
    } else if (machine == MACHINE_G850) {
      romBanks = 22;
      font_off = 0x4000 * 4 + 0x2016;
    } else {
      romBanks = 2;
      font_off = 0x4000 * 1;
    }

    /* 擬似ROMを設定する */
    rom = (uint8_t*)malloc(0x4000 * romBanks);
    for (p = rom; p < rom + 0x4000 * romBanks; p++)
      *p = rand() & 0xff;
    memcpy(rom + font_off, font, sizeof(font));
  }

  /* BANKを初期化する */
  romBank = exBank = 0;
  memcpy(&memory[0x8000], ROM(0), 0x4000);
  memcpy(&memory[0xc000], ROM(0), 0x4000);
  return TRUE;
}

/*
        EXROMを1ページ読み込む (下請け)
*/
static int loadExROM1page(uint8_t *p, const char *dir, int page, int size) {
  int result;
  char path[FILENAME_MAX];

  sprintf(path, "%s/exrom%02x.bin", dir, page);
  if ((result = readBin(path, p, size)) > 0)
    return result;

  sprintf(path, "%s/exrom%02x.txt", dir, page);
  if ((result = readHexAbs(path, p, NULL, size, FALSE)) > 0)
    return result;
  return 0;
}

int loadExROM(const char *dir) {
  int page;

  /* EXROMのページ数を調べる */
  for (exBanks = 0; loadExROM1page(NULL, dir, exBanks, 0x4000) == 0x4000;
       exBanks++)
    ;
  if (exBanks == 0)
    return FALSE;

  /* ROMを読み込む */
  exrom = (uint8_t*)malloc(0x4000 * exBanks);
  for (page = 0; page != exBanks; page++)
    loadExROM1page(EXROM(page), dir, page, 0x4000);
}

int loadRAM(const char *path) {
  if (readHex(path, memory, NULL, 0x8000, FALSE) > 0)
    return TRUE;

  if (!useROM) {
    memset(memory, 0, 0x8000);
    memory[0x7901] = 0x02;
    memory[0x7903] = 0x60;
  }
  return FALSE;
}

/*
        RAMを保存する
*/
int storeRAM(const char *path) {
  if (exram != NULL && ramBank > 0)
    memcpy(memory, exram, 0x8000);
  return (writeHex(path, memory, 0x40, 0x8000 - 0x40) > 0);
}
