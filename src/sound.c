/*
        SHARP PC-G800 series emulator
        サウンド
*/

#include "g800.h"
#include <stdio.h>
#include <string.h>

#define VOLUME 0x5f /* ボリューム */

static int oldXout = 0; /* 1周期前のXout */

/*
        オーディオバッファサイズを求める
*/
int getSoundBufferSize(int freq_update_io) {
  int sample_size;

  for (sample_size = 0x40000000;
       sample_size != 1 && FREQ_SOUND / sample_size <= freq_update_io;
       sample_size >>= 1)
    ;

  return sample_size;
}

/*
        音をバッファに書き込む
*/
void writeSound(int xout) {
  unsigned int x, len, freq_cpu;

  if ((xout && oldXout) || (!xout && !oldXout))
    return;
  oldXout = xout;

  freq_cpu = freqCPU / (csClk ? 2 : 1);
  x = freq_cpu / freqUpdateIO - z80.i.states;
  if (x & 0x80000000)
    return;
  if (x & 0xf0000000)
    len = FREQ_SOUND * (x >> 16) / (freq_cpu >> 16);
  else if (x & 0xff000000)
    len = FREQ_SOUND * (x >> 12) / (freq_cpu >> 12);
  else if (x & 0xfff00000)
    len = FREQ_SOUND * (x >> 8) / (freq_cpu >> 8);
  else if (x & 0xffff0000)
    len = FREQ_SOUND * (x >> 4) / (freq_cpu >> 4);
  else
    len = FREQ_SOUND * x / freq_cpu;
  if (len < soundBufferSize)
    memset(soundWriteBuffer + len, (xout ? VOLUME : 0), soundBufferSize - len);
}

/*
        読込バッファと書込バッファを交換する
*/
void flipSoundBuffer(void) {
#if 0
	uint8 *p;
#endif

  if (buzzer == BUZZER_SYNC)
    while (!soundPlayed)
      delay(1);

#if 0
	/* SDL2.0ではこの方法ではうまくいかない. */
	p = soundReadBuffer;
	soundReadBuffer = soundWriteBuffer;
	soundWriteBuffer = p;
	memset(soundWriteBuffer, (oldXout ? VOLUME: 0), soundBufferSize);
#else
  memcpy(soundReadBuffer, soundWriteBuffer, soundBufferSize);
  memset(soundWriteBuffer, (oldXout ? VOLUME : 0), soundBufferSize);
#endif

  soundPlayed = FALSE;
}

/*
        Copyright 2005 ~ 2013 maruhiro
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
