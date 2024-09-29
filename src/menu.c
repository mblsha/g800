/*
        SHARP PC-G800 series emulator
        メニュー
*/

#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include "g800.h"

/* カーソル位置 */
static uint8 curCol;
static uint8 curRow;

/* VRAM保存領域 */
static uint8 savedVram[166 * 9];
static uint8 savedLcdContrast;
static uint8 savedLcdTop;
static uint8 savedLcdBegin;
static uint8 savedMemory790d;
static int savedLcdScales;

/*
        表示位置を決める
*/
static void ulocate(uint8 col, uint8 row) {
  curCol = col;
  curRow = row;
}

/*
        UTF-8文字を現在の位置に表示する
*/
static int uputchr(const char *p) {
  int size;
  char ch;

  if (*p == 0)
    return 0;
  else if (*p == '\n') {
    if (nextRow(&curCol, &curRow))
      scrup();
    return 1;
  } else if (*p == '\r') {
    curCol = 0;
    return 1;
  }

  if ((*p & 0x80) == 0) {
    ch = *p;
    size = 1;
  } else if ((*p & 0xe0) == 0xc0 && (*(p + 1) & 0xc0) == 0x80) {
    ch = '?';
    size = 2;
  } else if ((*p & 0xf0) == 0xe0 && (*(p + 1) & 0xc0) == 0x80 &&
             (*(p + 2) & 0xc0) == 0x80) {
    if ((unsigned char)*p == 0xef && (unsigned char)*(p + 1) == 0xbd)
      ch = *(p + 2);
    else if ((unsigned char)*p == 0xef && (unsigned char)*(p + 1) == 0xbe)
      ch = *(p + 2) | 0x40;
    else
      ch = '?';
    size = 3;
  } else {
    ch = '?';
    size = 1;
  }

  putchr(curCol, curRow, ch);
  if (nextCol(&curCol, &curRow) && *(p + size) != 0)
    scrup();
  return size;
}

/*
        UTF-8文字列を表示する
*/
static void uprintf(const char *str, ...) {
  va_list v;
  char buf[256], *p;

  va_start(v, str);
  vsprintf(buf, str, v);
  va_end(v);

  for (p = buf; *p != 0; p += uputchr(p))
    ;
}

/*
        画面全体を消去する
*/
static void ucls(void) {
  ulocate(0, lcdRows - 1);
  uprintf("\n\n\n\n\n\n\n\n");
  lcdTop = lcdBegin = memory[0x790d] = 0;
  ulocate(0, 0);
}

/*
        1つ前の文字のbyte数を得る (下請け)
*/
static int prevsize(const char *p) {
  if ((*(p - 1) & 0x80) == 0)
    return 1;
  else if ((*(p - 2) & 0xe0) == 0xc0 && (*(p - 1) & 0xc0) == 0x80)
    return 2;
  else if ((*(p - 3) & 0xf0) == 0xe0 && (*(p - 2) & 0xc0) == 0x80 &&
           (*(p - 1) & 0xc0) == 0x80)
    return 3;
  else
    return 1;
}

/*
        文字列を得る(バッファを消去しない) (ugetsの下請け)
*/
static uint8 uadds(char *buf) {
  uint8 x, y, ch;
  char *p;

  /* バッファを表示する */
  for (p = buf; *p != 0; p += uputchr(p))
    ;
  *p = 0;

  for (;;) {
    x = curCol;
    y = curRow;

    /* キー入力を得る */
    ulocate(x, y);
    uprintf("_");
    updateLCD();
    ch = ggetchr();
    ulocate(x, y);
    uprintf(" ");
    ulocate(x, y);

    switch (ch) {
    case 0x0c: /* CLS */
      /* バッファが空ならば終了する */
      if (p == buf)
        return ch;

      /* バッファを全て消去する */
      for (; p > buf; p -= prevsize(p)) {
        prevCol(&curCol, &curRow);
        putchr(curCol, curRow, ' ');
      }
      strcpy(buf, "");
      break;
    case 0x01: /* BASIC */
    case 0x02: /* TEXT */
    case 0x03: /* C */
    case 0x04: /* STAT */
    case 0x05: /* BREAK */
    case 0x06: /* OFF */
    case 0x0a: /* TAB */
      /* 特殊キーを押して入力を終了した */
      return ch;
    case 0x0d: /* RETURN */
      /* RETURNを押して入力を終了した */
      uprintf("\r\n");
      return ch;
    case 0x08: /* BS */
    case 0x1d: /* ← */
      /* 文字を1つ戻す */
      if (p == buf)
        break;
      prevCol(&curCol, &curRow);
      putchr(curCol, curRow, ' ');
      p -= prevsize(p);
      *p = 0;
      break;
    default:
      /* 文字を入力した */
      if (ch < 0x20 || ch >= 0x7f)
        break;
      if (p == buf + lcdCols * lcdRows)
        break;
      *p = (char)ch;
      p += uputchr(p);
      *p = 0;
      break;
    }
  }
}

/*
        文字列を得る
*/
static uint8 ugets(char *buf) {
  strcpy(buf, "");
  return uadds(buf);
}

/*
        VRAMを退避する (下請け)
*/
void pushVram(void) {
  memcpy(savedVram, vram, sizeof(vram));
  savedLcdContrast = lcdContrast;
  savedLcdTop = lcdTop;
  savedLcdBegin = lcdBegin;
  savedMemory790d = memory[0x790d];
  savedLcdScales = lcdScales;

  lcdContrast = 0x0f;
  lcdScales = 2;
  updateLCDContrast();

  ucls();
}

/*
        VRAMを復帰する (下請け)
*/
void popVram(void) {
  memcpy(vram, savedVram, sizeof(vram));
  lcdContrast = savedLcdContrast;
  lcdTop = savedLcdTop;
  lcdBegin = savedLcdBegin;
  memory[0x790d] = savedMemory790d;
  lcdScales = savedLcdScales;
  updateLCDContrast();
}

/*
        メッセージを表示する
*/
void popup(const char *icon, const char *str, ...) {
  va_list v;
  int count = 0;
  char buf[256];

  va_start(v, str);
  vsprintf(buf, str, v);
  va_end(v);

  pushVram();
  waitRelease();

  ulocate(1, 0);
  uprintf(buf);
  do {
    ulocate(0, 0);
    count++;
    if (count < freqUpdateIO / 2)
      uprintf("%s", icon);
    else if (count < freqUpdateIO)
      uprintf(" ");
    else
      count = 0;
    updateLCD();
    updateKey();
    delay(1000 / freqUpdateIO);
  } while (peekKeycode() == GKEY_NONE);

  waitRelease();
  popVram();
}

/*
        文字列strのn文字のbyte数を得る (下請け)
*/
static int strsize(const char *str, int n) {
  int i;
  const char *p = str;

  for (i = 0; i < n; i++) {
    if (*p == 0)
      break;
    else if ((*p & 0x80) == 0)
      p++;
    else if ((*p & 0xe0) == 0xc0 && (*(p + 1) & 0xc0) == 0x80)
      p += 2;
    else if ((*p & 0xf0) == 0xe0 && (*(p + 1) & 0xc0) == 0x80 &&
             (*(p + 2) & 0xc0) == 0x80)
      p += 3;
    else
      p += 1;
  }

  return (int)(p - str);
}

/*
        一覧から項目を選択する (下請け)
*/
static uint8 selectItem(char **sel, char *buf) {
  uint8 ch;
  int top = 0, cur = 0, i, n, y;
  char *item[256], *p;

  for (n = 0, p = buf; n < sizeof(item) / sizeof(item[0]) && *p != 0;
       n++, p += strlen(p) + 1)
    item[n] = p;

  for (;;) {
    ucls();
    for (i = 0, y = 0; i < lcdRows; i++, y++) {
      p = (top + i < n ? item[top + i] : "");
      ulocate(1, y);
      uprintf("%.*s", strsize(p, 12), p);
    }
    for (i = lcdRows, y = 0; i < lcdRows * 2; i++, y++) {
      p = (top + i < n ? item[top + i] : "");
      ulocate(14, y);
      uprintf("%.*s", strsize(p, 10), p);
    }
    ulocate(0, cur);
    uprintf("¥x13");
    updateLCD();

    switch (ch = ggetchr()) {
    case 0x1c: /* → */
      if (top + lcdRows + cur < n)
        top += lcdRows;
      break;
    case 0x1d: /* ← */
      if (top != 0)
        top -= lcdRows;
      break;
    case 0x1e: /* ↑ */
      if (cur != 0)
        cur--;
      else if (top != 0) {
        cur = lcdRows - 1;
        top -= lcdRows;
      }
      break;
    case 0x1f: /* ↓ */
      if (top + cur + 1 >= n)
        ;
      else if (cur != lcdRows - 1)
        cur++;
      else if (top + lcdRows < n) {
        cur = 0;
        top += lcdRows;
      }
      break;
    case 0x05: /* BREAK */
    case 0x06: /* OFF */
    case 0x0c: /* CLS */
    case 0x0a: /* TAB */
      *sel = NULL;
      return ch;
    case 0x0d: /* RETURN */
    case 0x20: /* SPACE */
      *sel = item[top + cur];
      return ch;
    }
  }
}

/*
        一覧にファイル名を追加する (下請け)
*/
static int addItem(char *list, int size, const char *item) {
  int len;
  char *p, *q, p_last, item_last;

  for (p = list; *p != 0; p += strlen(p) + 1) {
    p_last = p[strlen(p) - 1];
    item_last = item[strlen(item) - 1];
    if ((p_last == '/' && item_last == '/') ||
        (p_last != '/' && item_last != '/')) {
      if (strcasecmp(p, item) > 0)
        break;
    } else {
      if (item_last == '/')
        break;
    }
  }
  for (q = p; *q != 0; q += strlen(q) + 1)
    ;

  len = strlen(item);
  if (list + size < q + len + 2)
    return FALSE;

  memmove(p + len + 1, p, (int)(q - p));
  strcpy(p, item);
  return TRUE;
}

/*
        ディレクトリを得る(下請け)
*/
static char *getDir(char *path) {
  char *p;

  for (p = path + strlen(path) - 1; p >= path && *p != '/'; p--)
    *p = 0;

  return path;
}

/*
        行番号つきまたはIntelHEX形式であるかチェックする (下請け)
*/
int isG800File(const char *path) {
  int i, size;
  char buf[160], *p = buf;

  if ((size = readBin(path, buf, sizeof(buf))) < 0)
    return FALSE;

  for (i = 0; i < 4; i++) {
    if ((int)(p - buf) >= size)
      break;
    if (!isdigit((unsigned char)*p) && *p != ':' && *p != 0x1a)
      return FALSE;

    while ((int)(p - buf) < size && *p != '\r' && *p != '\n')
      p++;
    while ((int)(p - buf) < size && (*p == '\r' || *p == '\n'))
      p++;
  }

  return TRUE;
}

/*
        ファイル名の一覧を得る (selectFileの下請け)
*/
#if defined(_WIN32)
static int makeFileList(char *file_list, int size, const char *dir_name) {
  HANDLE h;
  WIN32_FIND_DATAW found;
  int n = 0;
  DWORD cur_abs_dir_len;
  wchar_t w_path[PATH_MAX], cur_abs_dir[PATH_MAX], next_abs_dir[PATH_MAX];
  char dir[PATH_MAX], file[PATH_MAX], path[PATH_MAX], *p;

  memset(file_list, 0, size);

  sprintf(dir, "%s", dir_name);
  for (p = dir; *p != 0; p++)
    if (*p == '/')
      *p = '¥¥';

  sprintf(path, "%s*.*", dir);
  MultiByteToWideChar(CP_UTF8, 0, path, -1, w_path, PATH_MAX);
  if ((h = FindFirstFileW(w_path, &found)) == INVALID_HANDLE_VALUE)
    return -1;

  MultiByteToWideChar(CP_UTF8, 0, dir, -1, w_path, PATH_MAX);
  cur_abs_dir_len =
      GetFullPathNameW(w_path, sizeof(cur_abs_dir), cur_abs_dir, NULL);

  do {
    WideCharToMultiByte(CP_UTF8, 0, found.cFileName, -1, file, sizeof(file),
                        NULL, NULL);

    if (strcmp(file, ".") == 0) /* カレントディレクトリか? */
      continue;
    if (found.dwFileAttributes &
        (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM |
         FILE_ATTRIBUTE_TEMPORARY)) /* 隠しファイル/システムファイル/一時ファイルか?
                                     */
      continue;
    if (found.dwFileAttributes &
        FILE_ATTRIBUTE_DIRECTORY) { /* ディレクトリか? */
      sprintf(path, "%s%s¥¥", dir, file);
      MultiByteToWideChar(CP_UTF8, 0, path, -1, w_path, PATH_MAX);
      if (cur_abs_dir_len ==
          GetFullPathNameW(w_path, sizeof(next_abs_dir), next_abs_dir, NULL))
        if (memcmp(cur_abs_dir, next_abs_dir,
                   cur_abs_dir_len * sizeof(wchar_t)) ==
            0) /* ディレクトリを移動しても変わらない=ルートディレクトリか? */
          continue;
      sprintf(path, "%s/", file);
    } else { /* ファイル */
      if (useFileFilter) {
        strcpy(path, dir_name);
        getDir(path);
        strcat(path, file);
        if (!isG800File(path))
          continue;
      }
      sprintf(path, "%s", file);
    }

    if (!addItem(file_list, size, path))
      break;
    n++;
  } while (FindNextFileW(h, &found));

  FindClose(h);

  return n;
}
#else
static int makeFileList(char *file_list, int size, const char *dir_name) {
  DIR *dir;
  struct stat cur, root;
  struct dirent *p;
  int n = 0;
  char path[PATH_MAX];

  memset(file_list, 0, size);

  if ((dir = opendir(dir_name)) == NULL)
    return -1;

  if (stat("/", &root) != 0)
    root.st_ino = 2;
  if (stat(dir_name, &cur) != 0)
    cur.st_ino = 0;

  for (p = readdir(dir); p != NULL; p = readdir(dir)) {
    if (cur.st_ino == root.st_ino &&
        strcmp(p->d_name, "..") == 0) /* ルートディレクトリの..か? */
      continue;
    if (p->d_name[0] == '.' &&
        strcmp(p->d_name, "..") != 0) /* 隠しファイルか? */
      continue;
    if (p->d_type == DT_DIR) /* ディレクトリか? */
      sprintf(path, "%s/", p->d_name);
    else { /* ファイル */
      if (useFileFilter) {
        strcpy(path, dir_name);
        getDir(path);
        strcat(path, p->d_name);
        if (!isG800File(path))
          continue;
      }
      sprintf(path, "%s", p->d_name);
    }

    if (!addItem(file_list, size, path))
      break;
    n++;
  }

  closedir(dir);
  return n;
}
#endif

/*
        上のディレクトリを得る (selectFileの下請け)
*/
static int upDir(char *path) {
  char *p;

  /* ディレクトリのみにする */
  for (p = path + strlen(path) - 1; p >= path && *p != '/'; p--)
    *p = 0;

  /* ディレクトリがない, または../か? */
  if (p - 2 < path || strcmp(p - 2, "../") == 0) {
    /* ../を付ける */
    if (strlen(path) + strlen("../") >= PATH_MAX)
      return FALSE;
    strcat(path, "../");
    return TRUE;
  }

  /* ディレクトリを1段消す */
  for (p = p - 1; p >= path && *p != '/'; p--)
    *p = 0;
  return TRUE;
}

/*
        パス名を得る (selectFileの下請け)
*/
static int makePath(char *path, char *dir) {
  getDir(path);

  if (strlen(path) + strlen(dir) >= PATH_MAX)
    return FALSE;

  strcat(path, dir);
  return TRUE;
}

/*
        ファイルの一覧から選択する
*/
uint8 selectFile(char *path) {
  char file_list[0x4000], *p = 0, *dir;
  uint8 ch = 0;

  for (;;) {
    if (strcmp(path, "") == 0)
      dir = "./";
    else if (strlen(path) == 2 && path[1] == ':')
      dir = strcat(path, "/");
    else
      dir = path;

    if (makeFileList(file_list, sizeof(file_list), dir) > 0)
      ch = selectItem(&p, file_list);
    else
      return 0x0a;

    switch (ch) {
    case 0x0c: /* CLS */
      strcpy(path, "");
      return ch;
    case 0x06: /* OFF */
    case 0x0a: /* TAB */
      return ch;
    default:
      if (p == NULL)
        return ch;
      if (strcmp(p, "../") == 0)
        upDir(path);
      else if (p[strlen(p) - 1] == '/')
        makePath(path, p);
      else {
        makePath(path, p);
        return ch;
      }
    }
  }
}

/*
        ファイルの一覧から選択する
*/
int inputFile(char *path) {
  uint8 ch;
  char buf[PATH_MAX];

  strcpy(buf, path);

  for (;;) {
    ucls();
    uprintf("FILE:");
    ch = uadds(buf);
    if (ch != 0x0a)
      break;
    ch = selectFile(buf);
    if (ch != 0x0a)
      break;
  }
  ucls();

  if (ch == '\r' || ch == ' ') {
    strcpy(path, buf);
    return TRUE;
  } else
    return FALSE;
}

/*
        数値(16進数2バイト)を得る (下請け)
*/
static int gethex16(uint8 col, uint8 row, const char *prompt) {
  char buf[256], *p;

  ulocate(col, row);
  uprintf("%*s", lcdCols - col, " ");
  ulocate(col, row);
  uprintf("%s", prompt);
  if (ugets(buf) != 0x0d)
    return -1;

  if (strlen(buf) == 0 || strlen(buf) > 4)
    return -1;
  for (p = buf; *p != 0; p++)
    if (!isxdigit(*p))
      return -1;
  return atoix(buf);
}

/*
        ファイルメニューを表示する (menuの下請け)
*/
static int menuFile(void) {
  int size, start, end;
  uint16 address;
  char path[PATH_MAX];
  char info[256] = "";

  for (;;) {
    /* メニューを表示する */
    ucls();
    uprintf("      <<<< FILE >>>\n"
            "\n"
            "  Read  Write\n"
            "\n"
            "\n");
    ulocate(0, lcdRows - 2);
    uprintf("%s", info);

    /* 実行する */
    switch (ggetchr()) {
    case 'R':
    case 'r':
      /* 読み込み */
      strcpy(path, "");
      if (inputFile(path) && strcmp(path, "") != 0) {
        if ((size = loadProg(&address, path)) <= 0)
          sprintf(info, "READ ERROR");
        else
          sprintf(info, "READ OK:%04X-%04X", address, address + size - 1);
      }
      break;
    case 'W':
    case 'w':
      /* 書き込み */
      if ((start = gethex16(0, lcdRows - 2, "START:")) < 0)
        break;
      if ((end = gethex16(11, lcdRows - 2, "END:")) < 0)
        break;
      strcpy(path, "");
      if (!inputFile(path))
        break;
      if (writeHex(path, memory, start, end - start + 1) <= 0) {
        sprintf(info, "WRITE ERROR");
        break;
      }
      sprintf(info, "WRITE OK:%04X-%04X", start, end);
      break;
    case 0x05: /* BREAK */
    case 0x08: /* BS */
    case 0x0c: /* CLS */
      /* 戻る */
      waitRelease();
    case 0x01: /* BASIC */
    case 0x02: /* TEXT */
    case 0x03: /* C */
    case 0x04: /* STAT */
    case 0x06: /* OFF */
    case 0xf0: /* ASMBL */
    case 0xf1: /* BASE-n */
    case 0xf2: /* コントラスト */
      return TRUE;
    }
  }
}

/*
        SIOメニューを実行する (menuの下請け)
*/
static int menuSio(void) {
  char path[PATH_MAX];

  for (;;) {
    /* メニューとSIO状態を表示する */
    ucls();
    uprintf("       <<< SIO >>>\n"
            "\n"
            "  In   Out   Switch\n"
            "\n"
            "\n");
    ulocate(0, lcdRows - 2);
    switch (sioMode) {
    case SIO_MODE_STOP:
      uprintf("MODE:STOP");
      break;
    case SIO_MODE_IN:
      uprintf("MODE:IN[%s]", pathSioIn);
      break;
    case SIO_MODE_OUT:
      uprintf("MODE:OUT[%s]", pathSioOut);
      break;
    }
    updateLCD();

    /* 実行する */
    switch (ggetchr()) {
    case 'S':
    case 's':
      /* SIO切り替え */
      sioMode = (sioMode < 2 ? sioMode + 1 : 0);
      if (sioMode == SIO_MODE_IN && strcmp(pathSioIn, "") == 0)
        sioMode = SIO_MODE_OUT;
      if (sioMode == SIO_MODE_OUT && strcmp(pathSioOut, "") == 0)
        sioMode = SIO_MODE_STOP;
      break;
    case 'I':
    case 'i':
      /* SIO入力 */
      strcpy(path, "");
      if (inputFile(path))
        sioLoad(path);
      break;
    case 'O':
    case 'o':
      /* SIO出力 */
      strcpy(path, "");
      if (inputFile(path))
        sioSave(path);
      break;
    case 0x05: /* BREAK */
    case 0x08: /* BS */
    case 0x0c: /* CLS */
      /* 戻る */
      waitRelease();
    case 0x01: /* BASIC */
    case 0x02: /* TEXT */
    case 0x03: /* C */
    case 0x04: /* STAT */
    case 0x06: /* OFF */
    case 0xf0: /* ASMBL */
    case 0xf1: /* BASE-n */
    case 0xf2: /* コントラスト */
      return TRUE;
    }
  }
}

/*
        メニューを実行する
*/
int menu(void) {
  pushVram();
  ucls();

  for (;;) {
    /* メニューを表示する */
    ucls();
    uprintf("      *** MENU ***\n"
            "\n"
            "  Sio  File  CLS:Return");
    updateLCD();

    /* 実行する */
    switch (ggetchr()) {
    case 'F':
    case 'f':
      /* ファイルメニュー */
      if (menuFile())
        goto last;
      break;
    case 'S':
    case 's':
      /* SIOメニュー */
      if (menuSio())
        goto last;
      break;
    case 0x05: /* BREAK */
    case 0x08: /* BS */
    case 0x0c: /* CLS */
      /* 戻る */
      waitRelease();
    case 0x01: /* BASIC */
    case 0x02: /* TEXT */
    case 0x03: /* C */
    case 0x04: /* STAT */
    case 0x06: /* OFF */
    case 0xf0: /* ASMBL */
    case 0xf1: /* BASE-n */
    case 0xf2: /* コントラスト */
      goto last;
    }
  }

last:;
  popVram();
  return FALSE;
}

/*
        Copyright 2005 ‾ 2016 maruhiro
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
