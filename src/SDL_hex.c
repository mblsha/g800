/*
        IntelHEX形式処理(SDL_hex.c)
*/

#include "SDL.h"
#include <stdio.h>
#include <string.h>

/*
        文字を数値(1byte)に変換する (下請け)
*/
static int h2i(char c) {
  switch (c) {
  case '0':
    return 0;
  case '1':
    return 1;
  case '2':
    return 2;
  case '3':
    return 3;
  case '4':
    return 4;
  case '5':
    return 5;
  case '6':
    return 6;
  case '7':
    return 7;
  case '8':
    return 8;
  case '9':
    return 9;
  case 'A':
  case 'a':
    return 0xa;
  case 'B':
  case 'b':
    return 0xb;
  case 'C':
  case 'c':
    return 0xc;
  case 'D':
  case 'd':
    return 0xd;
  case 'E':
  case 'e':
    return 0xe;
  case 'F':
  case 'f':
    return 0xf;
  default:
    return -16;
  }
}
static int hex2i(const char *hex) {
  int val;

  if ((val = (h2i(*hex) << 4) | h2i(*(hex + 1))) < 0)
    return -1;
  return val;
}

/*
        IntelHEX形式ファイルを読み込む
*/
Sint64 SDLHex_Load_RW(SDL_RWops *rw, void *mem, Sint64 *ret_top,
                      size_t mem_size, int check, int free_rw) {
  int i, len, off_h, off_l, off, rec, val, sum = 0, top = 0xffff, size = 0;
  uint8_t *w;
  char buf[8];

  if (rw == NULL)
    return -1;

  for (;;) {
    /* レコード開始記号 */
    if (SDL_RWread(rw, buf, 1, 1) == 0)
      break;
    if (*buf == '\n' || *buf == '\r')
      continue;
    if (*buf != ':')
      goto fail;

    /* データ長 */
    if (SDL_RWread(rw, buf, 2, 1) == 0)
      goto fail;
    if ((len = hex2i(buf)) < 0)
      goto fail;
    if (len == 0)
      break;
    sum += len;

    /* オフセットアドレス */
    if (SDL_RWread(rw, buf, 4, 1) == 0)
      goto fail;
    if ((off_h = hex2i(buf + 0)) < 0)
      goto fail;
    if ((off_l = hex2i(buf + 2)) < 0)
      goto fail;
    off = (off_h << 8) | off_l;
    sum += off_h + off_l;

    /* レコードタイプを得る */
    if (SDL_RWread(rw, buf, 2, 1) == 0)
      goto fail;
    if ((rec = hex2i(buf)) == 0) /* データレコード */
      ;
    else if (rec == 1) /* エンドレコード */
      break;
    else /* その他のレコード */
      goto fail;
    sum += rec;

    /* データ部 */
    for (i = 0, w = (uint8_t *)mem + off; i < len; i++, w++) {
      if (SDL_RWread(rw, buf, 2, 1) == 0)
        goto fail;
      if ((val = hex2i(buf)) < 0)
        goto fail;
      if (mem != NULL && (uint8_t *)mem <= w && w < (uint8_t *)mem + mem_size)
        *w = val;
      sum += val;
    }
    sum = (‾sum + 1) & 0xff;

    /* チェックサム */
    if (SDL_RWread(rw, buf, 2, 1) == 0)
      goto fail;
    if (check && hex2i(buf) != sum)
      goto fail;

    /* 先頭アドレス */
    if (top > off)
      top = off;

    /* サイズ */
    if (size < off + len - top)
      size = off + len - top;
  }

  if (free_rw)
    SDL_RWclose(rw);
  if (ret_top != NULL)
    *ret_top = top;
  return size;

fail:;
  if (free_rw)
    SDL_RWclose(rw);
  return -1;
}

/*
        IntelHEX形式ファイルを読み込む(ファイル内のアドレスを無視する)
*/
Sint64 SDLHex_LoadAbs_RW(SDL_RWops *rw, void *mem, Sint64 *ret_top,
                         size_t mem_size, int check, int free_rw) {
  size_t size;
  Sint64 top = 0;

  if (rw == NULL)
    return 0;

  if ((size = SDLHex_Load_RW(rw, NULL, &top, 0x10000, check, SDL_FALSE)) < 0) {
    if (free_rw)
      SDL_RWclose(rw);
    return size;
  }

  if (ret_top != NULL)
    *ret_top = top;
  if (mem == NULL)
    return size;

  SDL_RWseek(rw, 0, RW_SEEK_SET);
  return SDLHex_Load_RW(rw, (uint8_t *)mem - top, NULL, mem_size + top, check,
                        SDL_TRUE);
}

/*
        IntelHEX形式ファイルを読み込む
*/
Sint64 SDLHex_Load(const char *path, void *mem, Sint64 *ret_top,
                   size_t mem_size, int check) {
  return SDLHex_Load_RW(SDL_RWFromFile(path, "r"), mem, ret_top, mem_size,
                        check, SDL_TRUE);
}

/*
        IntelHEX形式ファイルを読み込む(ファイル内のアドレスを無視する)
*/
Sint64 SDLHex_LoadAbs(const char *path, void *mem, Sint64 *ret_top,
                      size_t mem_size, int check) {
  return SDLHex_LoadAbs_RW(SDL_RWFromFile(path, "r"), mem, ret_top, mem_size,
                           check, SDL_TRUE);
}

/*
        数値(1byte)を文字に変換する (下請け)
*/
static char *int2c(char *hex, int i) {
  const static char int_to_ascii[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                      '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

  hex[0] = int_to_ascii[i >> 4];
  hex[1] = int_to_ascii[i & 0xf];
  return hex;
}

/*
        IntelHEX形式ファイルを書き込む
*/
Sint64 SDLHex_Save_RW(SDL_RWops *rw, const void *mem, Sint64 off, size_t size,
                      int free_rw) {
  int orig_size = size, len, off_h, off_l, sum;
  const uint8_t *r = (uint8_t *)mem + off;
  char buf[80], *w;

  if (rw == NULL)
    return -1;

  for (;;) {
    w = buf;

    /* レコード開始記号 */
    *w++ = ':';

    if (size <= 0) {
      /* エンドレコード */
#if defined(_WIN32)
      strcpy(w, "00000001FF\r\n");
      SDL_RWwrite(rw, buf, 13, 1);
#else
      strcpy(w, "00000001FF\n");
      SDL_RWwrite(rw, buf, 12, 1);
#endif
      break;
    }

    /* データ長 */
    if (size > 16)
      len = 16;
    else
      len = size;
    int2c(w, len);
    w += 2;
    sum = len;

    /* オフセットアドレス */
    off_h = ((int)(r - (uint8_t *)mem) >> 8) & 0xff;
    int2c(w, off_h);
    w += 2;
    sum += off_h;

    off_l = (int)(r - (uint8_t *)mem) & 0xff;
    int2c(w, off_l);
    w += 2;
    sum += off_l;

    /* レコードタイプ */
    int2c(w, 0x00);
    w += 2;
    sum += 0x00;

    /* データ部 */
    for (; len > 0; len--, size--) {
      int2c(w, *r);
      w += 2;
      sum += *r++;
    }

    /* チェックサム */
    int2c(w, (‾sum + 1) & 0xff);
    w += 2;

    /* 改行 */
#if defined(_WIN32)
    *w++ = '\r';
#endif
    *w++ = '\n';

    /* 書き込み */
    if (SDL_RWwrite(rw, buf, (int)(w - buf), 1) == 0)
      goto fail;
  }

  if (free_rw)
    SDL_RWclose(rw);
  return orig_size;

fail:;
  if (free_rw)
    SDL_RWclose(rw);
  return -1;
}

/*
        IntelHEX形式ファイルを書き込む(ファイル内のアドレスを無視する)
*/
Sint64 SDLHex_SaveAbs_RW(SDL_RWops *rw, const void *mem, Sint64 off,
                         size_t size, int free_rw) {
  return SDLHex_Save_RW(rw, (const uint8_t *)mem - off, off, size, free_rw);
}

/*
        IntelHEX形式ファイルを書き込む
*/
Sint64 SDLHex_Save(const char *path, const void *mem, Sint64 off, size_t size) {
  return SDLHex_Save_RW(SDL_RWFromFile(path, "w"), mem, off, size, SDL_TRUE);
}

/*
        IntelHEX形式ファイルを書き込む(ファイル内のアドレスを無視する)
*/
Sint64 SDLHex_SaveAbs(const char *path, const void *mem, Sint64 off,
                      size_t size) {
  return SDLHex_SaveAbs_RW(SDL_RWFromFile(path, "w"), mem, off, size, SDL_TRUE);
}

/*
        Copyright 2014 maruhiro
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
