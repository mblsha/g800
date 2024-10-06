#include "g800.h"

/*
        シミュレータを実行する
*/
int execSim(void) {
  /* システムを初期化 */
  initBasic(&bas);

  /* システムを起動 */
  memory[0x7902] = 0x40;

  while (!isoff()) {
    if (memory[0x7902] == 0x20)
      basPro(&bas);
    else if (memory[0x7902] == 0x40)
      basRun(&bas);
    else
      monitor();
  }
  return 0;
}

int main(int argc, char *argv[]) {
#if defined(_WIN32) && SDL_MAJOR_VERSION == 1
  /* win32のSDL1.2ならば引数をUTF-8に変換する */
  argv = argvToUTF8(argc, argv);
#endif

  /* 初期化 */
  if (!init(argc, argv))
    popup("!", "CANNOT OPEN CONFIG FILE");
  if (!loadROM(dirROM))
    popup("!", "CANNOT OPEN ROM FILES [%s]", dirROM);
  loadExROM(dirROM);
  loadRAM(pathRAM);
  if (pathProg != NULL && strcmp(pathProg, "") != 0)
    if (loadProg(NULL, pathProg) < 0) {
      popup("!", "CANNOT OPEN %s", pathProg);
      beginProg = 0;
    }
  z80srand(time(NULL));

  memset(keyMatrix, 0, sizeof(keyMatrix));
  keyBreak = keyShift = keyReset = FALSE;

  /* 実行 */
  boot();

  if (beginProg != 0) {
    /* アドレスを指定して実行 */
    go(beginProg);
    exec(TRUE);
  } else if (useROM) {
    /* 先頭から実行 */
    exec(FALSE);
  } else if (useBasic) {
    /* BASICシミュレータを実行 */
    execSim();
  } else {
    /* モニタシミュレータを実行 */
    monitor();
  }

  /*
  printf("Total states = %d\n", z80.i.total_states);
  */

  /* 後処理 */
  storeRAM(pathRAM);
  return 0;
}

