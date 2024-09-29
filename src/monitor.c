/*
        SHARP PC-G800 series emulator
        擬似モニタ
*/

#include "g800.h"
#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
        16進数の文字列を数値に変換する
*/
int atoix(const char *buf) {
  int x;

  sscanf(buf, "%x", &x);
  return x;
}

/*
        実行レジスタを設定する
*/
void go(uint16 pc) {
  z80.r16.af = 0x0044;
  z80.r16.bc = 0x0000;
  z80.r16.de = 0x0000;
  z80.r16.hl = 0x0100;
  z80.r16.ix = 0x7c05;
  z80.r16.iy = 0x7c03;
  z80.r16.sp = 0x7ff6;
  z80.r16.pc = pc;
}

/*
        プログラムを読み込む
*/
int loadProg(uint16 *begin_ret, const char *path) {
  int size, begin;

  if (path == NULL || strcmp(path, "") == 0)
    return 0;
  if ((size = readHex(path, memory, &begin, 0x8000, FALSE)) == 0)
    return -1;

  if (begin_ret != NULL)
    *begin_ret = begin;
  return size;
}

/*
        表示できる文字に変換する (dump0の下請け)
*/
static uint8_t to_disp(uint8_t chr) {
  if (chr < 0x20 || chr > 0xf8)
    return '.';
  else
    return chr;
}

/*
        dumpを1画面表示する (dumpの下請け)
*/
static void dump0(uint16 address) {
  uint8_t sum = 0, *p;

  address &= 0xfff0;
  for (p = &memory[address]; p < &memory[address + 16]; p++)
    sum += *p;

  glocate(0, 0);
  gprintf("%04X : %02X %02X %02X %02X  %c%c%c%c"
          "(%02X)   %02X %02X %02X %02X  %c%c%c%c"
          "       %02X %02X %02X %02X  %c%c%c%c"
          "       %02X %02X %02X %02X  %c%c%c%c",
          address, memory[address + 0], memory[address + 1],
          memory[address + 2], memory[address + 3],
          to_disp(memory[address + 0]), to_disp(memory[address + 1]),
          to_disp(memory[address + 2]), to_disp(memory[address + 3]), sum,
          memory[address + 4], memory[address + 5], memory[address + 6],
          memory[address + 7], to_disp(memory[address + 4]),
          to_disp(memory[address + 5]), to_disp(memory[address + 6]),
          to_disp(memory[address + 7]), memory[address + 8],
          memory[address + 9], memory[address + 10], memory[address + 11],
          to_disp(memory[address + 8]), to_disp(memory[address + 9]),
          to_disp(memory[address + 10]), to_disp(memory[address + 11]),
          memory[address + 12], memory[address + 13], memory[address + 14],
          memory[address + 15], to_disp(memory[address + 12]),
          to_disp(memory[address + 13]), to_disp(memory[address + 14]),
          to_disp(memory[address + 15]));

  if (lcdRows < 6)
    return;
  if (address >= 0xfff0) {
    gprintf("------------------------"
            "                        ");
    return;
  }

  sum = 0;
  for (p = &memory[address + 16]; p < &memory[address + 32]; p++)
    sum += *p;

  gprintf("%04X : %02X %02X %02X %02X  %c%c%c%c"
          "(%02X)   %02X %02X %02X %02X  %c%c%c%c",
          address + 16, memory[address + 16], memory[address + 17],
          memory[address + 18], memory[address + 19],
          to_disp(memory[address + 16]), to_disp(memory[address + 17]),
          to_disp(memory[address + 18]), to_disp(memory[address + 19]), sum,
          memory[address + 20], memory[address + 21], memory[address + 22],
          memory[address + 23], to_disp(memory[address + 20]),
          to_disp(memory[address + 21]), to_disp(memory[address + 22]),
          to_disp(memory[address + 23]));
}

/*
        Dumpを表示する (下請け)
*/
static uint8_t dump(uint16 address) {
  uint8_t k;

  for (;;) {
    dump0(address);
    switch (k = ggetchr()) {
    case 0x1e: /* ↑ */
      address -= 16;
      break;
    case 0x1f: /* ↓ */
      address += 16;
      break;
    case 0x01: /* BASIC */
    case 0x02: /* TEXT */
    case 0x03: /* C */
    case 0x04: /* STAT */
    case 0x05: /* BREAK */
    case 0x06: /* OFF */
    case 0x0c: /* CLS */
      gcls();
      return k;
    }
  }
}

/*
        メモリを編集する (下請け)
*/
static int edit0(uint16 address, int cur, uint8_t k) {
  uint8_t x, *p;

  if ('0' <= k && k <= '9')
    x = k - '0';
  else if ('A' <= k && k <= 'F')
    x = k - 'A' + 0xa;
  else if ('a' <= k && k <= 'f')
    x = k - 'a' + 0xa;
  else
    return FALSE;

  p = &memory[address + cur / 2];
  if (cur % 2 == 0)
    *p = (*p & 0x0f) | (x << 4);
  else
    *p = (*p & 0xf0) | x;
  return TRUE;
}
static uint8_t edit(uint16 address) {
  const uint8_t col[] = {7,  8,  10, 11, 13, 14, 16, 17, 7,  8,  10,
                       11, 13, 14, 16, 17, 7,  8,  10, 11, 13, 14,
                       16, 17, 7,  8,  10, 11, 13, 14, 16, 17};
  const uint8_t row[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
                       2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};
  int cur = (address & 0x0f) * 2;
  uint8_t k;

  for (;;) {
    /* スクロールする */
    if (cur < 0) {
      cur += 32;
      address -= 16;
    } else if (cur >= 32) {
      cur -= 32;
      address += 16;
    }
    address &= 0x7ff0;

    /* キーを得る, カーソルを表示する */
    dump0(address);
    glocate(col[cur], row[cur]);
    k = getChrcode(CURSOR_OVER);

    /* 実行する */
    switch (k) {
    case 0x1e: /* ↑ */
      cur -= 8;
      break;
    case 0x1f: /* ↓ */
      cur += 8;
      break;
    case 0x1d: /* ← */
      cur--;
      break;
    case 0x1c: /* → */
      cur++;
      break;
    case 0x01: /* BASIC */
    case 0x02: /* TEXT */
    case 0x03: /* C */
    case 0x04: /* STAT */
    case 0x05: /* BREAK */
    case 0x06: /* OFF */
    case 0x0c: /* CLS */
      gcls();
      return k;
    default:
      if (edit0(address, cur, k))
        cur++;
      break;
    }
  }
}

/*
        逆アセンブルする (下請け)
*/
static uint8_t disasm(uint16 address) {
  char buf[256];
  uint8_t k, row, *p;

  for (;;) {
    gcls();
    p = &memory[address];
    for (row = 0; row < lcdRows; row++) {
      p = z80disasm(buf, p, z80bank(&z80, (uint16)(p - memory)),
                    (uint16)(p - memory), z80.i.symbol);
      glocate(0, row);
      gprintf("%04X:%s", (int)(p - memory), buf);
    }

    switch (k = ggetchr()) {
    case 0x1e: /* ↑ */
      break;
    case 0x1f: /* ↓ */
      address = (int)((uint8_t *)z80disasm(buf, &memory[address],
                                         z80bank(&z80, (uint16)(p - memory)),
                                         (uint16)(p - memory), z80.i.symbol) -
                      memory);
      break;
    case 0x01: /* BASIC */
    case 0x02: /* TEXT */
    case 0x03: /* C */
    case 0x04: /* STAT */
    case 0x05: /* BREAK */
    case 0x06: /* OFF */
    case 0x0c: /* CLS */
      gcls();
      return k;
    }
  }
}

/*
        パラメータを得る (getParam1, getParam2の下請け)
*/
static const uint8_t *_getParam(const uint8_t *p, int *param) {
  uint8_t tmp[256];
  const uint8_t *q;
  uint8_t *r;

  for (q = p, r = tmp; isxdigit(*q); q++, r++)
    *r = *q;
  *r = 0;

  if (p == q) {
    *param = -1;
    return (*q == 0 || *q == ',' ? q : NULL);
  }

  *param = atoix((char *)tmp);
  return q;
}

/*
        1つのパラメータを得る (monitorの下請け)
*/
static int getParam1(const uint8_t *buf, int off, int *param) {
  return (_getParam(buf + off, param) != NULL);
}

/*
        2つのパラメータを得る (monitorの下請け)
*/
static int getParam2(const uint8_t *buf, int off, int *param1, int *param2) {
  const uint8_t *p;

  if ((p = _getParam(buf + off, param1)) == NULL)
    return FALSE;
  if (*p != ',')
    return FALSE;
  if (_getParam(p + 1, param2) == NULL)
    return FALSE;
  return TRUE;
}

/*
        文字列を大文字にする (monitorの下請け)
*/
static uint8_t *toUpperStr(uint8_t *buf) {
  uint8_t *p;

  for (p = buf; *p != 0; p++)
    *p = toupper((char)*p);

  return buf;
}

/*
        擬似モニタを実行する
*/
int monitor(void) {
  int size, param1, param2;
  uint16 address;
  uint8_t buf[256];

  /* タイトルを表示する */
  memory[0x7902] = 0;
  *VRAM_CR(lcdCols, 0) = 0x00;
  *VRAM_CR(lcdCols, 1) = 0x00;
  *VRAM_CR(lcdCols, 2) = 0x00;
  gcls();
  gprintf("MACHINE LANGUAGE MONITOR\r");

  while (memory[0x7902] == 0x00) {
    /* プロンプトを表示する */
    gprintf("*");

    /* 入力されたコマンドを得る */
    switch (ggetline(buf, (const uint8_t *)"", GETLINE_MAN)) {
    case 0x01: /* BASIC */
      memory[0x7902] = 0x40;
      return 0;
    case 0x02: /* TEXT */
      return 0;
    case 0x03: /* C */
      return 0;
    case 0x04: /* STAT */
      return 0;
    case 0x06: /* OFF */
      poweroff();
      return 0;
    case 0x0c: /* CLS */
      gcls();
      break;
    case 0x0d: /* RETURN */
      /* コマンドを実行する */
      toUpperStr(buf);

      gprintf("\r");

      if (buf[0] == 0) {
        /* コマンドなし */
      } else if (memcmp(buf, "D", 1) == 0 && getParam1(buf, 1, &param1)) {
        /* D:ダンプ */
        dump(param1 > 0 ? param1 : 0x100);
      } else if (memcmp(buf, "E", 1) == 0 && getParam1(buf, 1, &param1)) {
        /* E:編集 */
        edit(param1 > 0 ? param1 : 0x100);
      } else if (memcmp(buf, "U", 1) == 0 && getParam1(buf, 1, &param1)) {
        /* U:逆アセンブル */
        disasm(param1 > 0 ? param1 : z80.r16.pc);
      } else if (memcmp(buf, "G", 1) == 0 && getParam1(buf, 1, &param1) &&
                 param1 >= 0) {
        /* G:実行 */
        go(param1);
        exec(TRUE);
      } else if (memcmp(buf, "R", 1) == 0) {
        /* R:読み込み */
        if ((size = loadProg(&address, pathSioIn)) <= 0)
          gprintf("READ ERROR\r");
        else
          gprintf("INFO:%04X-%04X\r", address, address + size - 1);
      } else if (memcmp(buf, "W", 1) == 0 &&
                 getParam2(buf, 1, &param1, &param2) && param1 >= 0 &&
                 param2 >= 0) {
        /* W:書き込み */
        if (writeHex(pathSioOut, memory, param1, param2 - param1) <= 0)
          gprintf("WRITE ERROR\r");
      } else if (memcmp(buf, "USER", 4) == 0 && getParam1(buf, 4, &param1)) {
        /* USER:ユーザエリア */
        if (param1 >= 0x100 && param1 <= 0x7fff) {
          memory[0x7fff] = (param1 + 1) >> 8;
          memory[0x7ffe] = (param1 + 1) & 0xff;
        }
        size = (memory[0x7fff] * 0x100 + memory[0x7ffe] - 1) & 0xffff;
        if (size == 0xffff)
          gprintf("FREE:NOT RESERVED\r");
        else
          gprintf("FREE:%04X-%04X\r", 0x100, size);
      } else {
        /* エラー */
        gprintf(" SYNTAX ERROR\r");
      }
    }
  }

  return 0;
}

/*
        Copyright 2005 ~ 2016 maruhiro
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
