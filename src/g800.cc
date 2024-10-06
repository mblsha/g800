/*
        SHARP PC-G800 series emulator
        メイン
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define DEF_GLOBAL
#include "g800.h"


/*
        初期化をエミュレートする
*/
void boot(void) {
  z80reset(&z80);

  z80outport(NULL, 0x11, 0);
  z80outport(NULL, 0x12, 0);
  z80outport(NULL, 0x14, 0);
  z80outport(NULL, 0x15, 1);
  z80outport(NULL, 0x16, 0xff);
  z80outport(NULL, 0x17, 0xf);
  z80outport(NULL, 0x18, 0);
  z80outport(NULL, 0x19, 0);
  z80outport(NULL, 0x1b, 0);
  z80outport(NULL, 0x1c, 1);
  timerInterval = 388643;
  z80.r.im = 1;

  memcpy(memory, base.data(), base.size());
  memory[0x790d] = 0;

  switch (machine) {
  case MACHINE_E200:
    z80outport(NULL, 0x58, 0xc0);
    break;
  case MACHINE_G815:
    z80outport(NULL, 0x50, 0xc0);
    break;
  case MACHINE_G850:
    memory[0x779c] =
        (memory[0x779c] >= 0x07 && memory[0x779c] <= 0x1f ? memory[0x779c]
                                                          : 0x0f);
    z80outport(NULL, 0x40, 0x24);
    z80outport(NULL, 0x40, memory[0x790d] + 0x40);
    z80outport(NULL, 0x40, memory[0x779c] + 0x80);
    z80outport(NULL, 0x40, 0xa0);
    z80outport(NULL, 0x40, 0xa4);
    z80outport(NULL, 0x40, 0xa6);
    z80outport(NULL, 0x40, 0xa9);
    z80outport(NULL, 0x40, 0xaf);
    z80outport(NULL, 0x40, 0xc0);
    z80outport(NULL, 0x40, 0x25);
    z80outport(NULL, 0x60, 0);
    z80outport(NULL, 0x61, 0xff);
    z80outport(NULL, 0x62, 0);
    z80outport(NULL, 0x64, 0);
    z80outport(NULL, 0x65, 1);
    z80outport(NULL, 0x66, 1);
    z80outport(NULL, 0x67, 0);
    z80outport(NULL, 0x6b, 4);
    z80outport(NULL, 0x6c, 0);
    z80outport(NULL, 0x6d, 0);
    z80outport(NULL, 0x6e, 4);
    break;
  }
}

/*
        電源停止したか?
*/
int isoff(void) { return z80.r.halt && z80.r.iff == 0 && ioReset == 0; }

/*
        プログラムを実行する
*/
int exec(int exit_when_underflow) {
  int wait, states_update_io;
  uint8_t itype;

  wait = 1000 / freqUpdateIO;
  z80.i.stack_under = (exit_when_underflow ? 0x7ff6 : 0xffff);
  z80.i.states = 0;

  for (;;) {
    /* ウェイト */
    states_update_io = freqCPU / (csClk ? 2 : 1) / freqUpdateIO;
    do {
      z80.i.states += states_update_io;
      statesKeyStrobeLast += states_update_io;
      delay(wait);
    } while (z80.i.states < 0);

    /* サウンドバッファを切り替える */
    if (buzzer != BUZZER_NONE)
      flipSoundBuffer();

    /* コード実行 */
    switch (z80exec(&z80)) {
    case Z80_RUN:
      break;
    case Z80_HALT:
      if (!isoff())
        break;
    case Z80_UNDERFLOW:
      updateLCD();
      return 0;
    }

    /* キー更新・キー割り込み */
    itype = updateKey();
    if (itype & interruptMask) {
      interruptType |= itype;
      z80int1(&z80);
    }

    /* タイマ更新・タイマ割り込み */
    if (timerCount-- == 0) {
      timerCount = freqUpdateIO * timerInterval / 1000 / 1000;
      if (interruptMask & INTERRUPT_1S) {
        timer ^= TIMER_1S;
        interruptType |= INTERRUPT_1S;
        z80int1(&z80);
      }
    }

    /* リセットキー */
    if (keyReset) {
      keyReset = FALSE;

      if (exit_when_underflow)
        return 0;
      boot();
    }

    /* LCD更新 */
    updateLCD();
  }
}

/*
        Copyright 2005 ~ 2017 maruhiro
        All rights reserved.

        Redistribution and use in source and binary forms,
        with or without modification, are permitted provided that
        the following conditions are met:

         1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

         2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

        THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
   EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
   OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* eof */
