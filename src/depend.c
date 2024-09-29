/*
        SHARP PC-G800 series emulator
        環境依存部分
*/

#include "SDL.h"
#include "SDL_hex.h"
#include "g800.h"
#include "g800icon.xpm"
#include "keytop.h"
#include "symbol.h"
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#if defined(_WIN32)
#include <windows.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

/* LCD・VRAM */
#define MAX_ZOOM 3 /* ステータス表示部の拡大の最大値 */
#define MAX_SCALE (360 / 8 + 1) /* 階調の最大値 */
#define SCREEN(x, y)                                                           \
  ((uint8_t *)screen->pixels + (y) * screen->pitch +                           \
   screen->format->BytesPerPixel * (x)) /* ピクセルのアドレス */
#define VRAM_OFF(col, row, top)                                                \
  (((row + (top) / 8) % 8) * vramWidth +                                       \
   (col) * cellWidth) /* 仮想VRAMのオフセット */
#define VRAM(col, row, top)                                                    \
  &vram[VRAM_OFF(col, row, top)] /* 仮想VRAMのアドレス */
#define OLD_VRAM(col, row, top)                                                \
  &oldVram[VRAM_OFF(col, row, top)] /* 1フレーム前の仮想VRAMのアドレス */

/* 色・Pixelテーブルのインデックス番号 */
#define COLOR_BODY 0        /* 本体の色 */
#define COLOR_GRAY 1        /* キーの色 灰 */
#define COLOR_GREEN 2       /* キーの色 緑 */
#define COLOR_YELLOW 3      /* キーの色 黄 */
#define COLOR_RED 4         /* キーの色 赤 */
#define COLOR_LIGHTGRAY 5   /* キーの色 明るい灰 */
#define COLOR_LIGHTGREEN 6  /* キーの色 明るい緑 */
#define COLOR_LIGHTYELLOW 7 /* キーの色 明るい黄 */
#define COLOR_LIGHTRED 8    /* キーの色 明るい赤 */
#define COLOR_DARKGRAY 9    /* キーの色 暗い灰 */
#define COLOR_DARKGREEN 10  /* キーの色 暗い緑 */
#define COLOR_DARKYELLOW 11 /* キーの色 暗い黄色 */
#define COLOR_DARKRED 12    /* キーの色 暗い赤 */
#define COLOR_MIDGREEN 13
#define COLOR_MIDYELLOW 14
#define COLOR_MIDRED 15
#define COLOR_LCD_START 16 /* LCDの色の先頭 */

/* 汎用 */
#define MAX(x, y) ((x) > (y) ? (x) : (y)) /* 大きいほうの値 */

/* ステータスのBitmap */
static Bitmap *bmpStatus[] = {bmpStatus1, bmpStatus2, bmpStatus3};

/* キートップのBitmap */
static Bitmap *bmpKeytop[] = {bmpKeytop1, bmpKeytop2, bmpKeytop3};

/* キートップのBitmap (PC-E220) */
static Bitmap *bmpKeytopE220[] = {bmpKeytopE220_1, bmpKeytopE220_2,
                                  bmpKeytopE220_3};

/* キーの背景色 */
static uint8_t keyBackColor[] = {
    COLOR_GRAY,   COLOR_GRAY, /* OFF */
    COLOR_GRAY,               /* Q */
    COLOR_GRAY,               /* W */
    COLOR_GRAY,               /* E */
    COLOR_GRAY,               /* R */
    COLOR_GRAY,               /* T */
    COLOR_GRAY,               /* Y */
    COLOR_GRAY,               /* U */
    COLOR_GRAY,               /* A */
    COLOR_GRAY,               /* S */
    COLOR_GRAY,               /* D */
    COLOR_GRAY,               /* F */
    COLOR_GRAY,               /* G */
    COLOR_GRAY,               /* H */
    COLOR_GRAY,               /* J */
    COLOR_GRAY,               /* K */
    COLOR_GRAY,               /* Z */
    COLOR_GRAY,               /* X */
    COLOR_GRAY,               /* C */
    COLOR_GRAY,               /* V */
    COLOR_GRAY,               /* B */
    COLOR_GRAY,               /* N */
    COLOR_GRAY,               /* M */
    COLOR_GRAY,               /* , */
    COLOR_GREEN,              /* BASIC */
    COLOR_GREEN,              /* TEXT */
    COLOR_GRAY,               /* CAPS */
    COLOR_GRAY,               /* カナ */
    COLOR_GRAY,               /* TAB */
    COLOR_GRAY,               /* SPACE */
    COLOR_GRAY,               /* ↓ */
    COLOR_GRAY,               /* ↑ */
    COLOR_GRAY,               /* ← */
    COLOR_GRAY,               /* → */
    COLOR_GRAY,               /* ANS */
    COLOR_GRAY,               /* 0 */
    COLOR_GRAY,               /* . */
    COLOR_GRAY,               /* = */
    COLOR_GRAY,               /* + */
    COLOR_GRAY,               /* Return */
    COLOR_GRAY,               /* L */
    COLOR_GRAY,               /* ; */
    COLOR_GRAY,               /* CONST */
    COLOR_GRAY,               /* 1 */
    COLOR_GRAY,               /* 2 */
    COLOR_GRAY,               /* 3 */
    COLOR_GRAY,               /* - */
    COLOR_GRAY,               /* M+ */
    COLOR_GRAY,               /* I */
    COLOR_GRAY,               /* O */
    COLOR_GRAY,               /* INS */
    COLOR_GRAY,               /* 4 */
    COLOR_GRAY,               /* 5 */
    COLOR_GRAY,               /* 6 */
    COLOR_GRAY,               /* * */
    COLOR_GRAY,               /* R・CM */
    COLOR_GRAY,               /* P */
    COLOR_GRAY,               /* BS */
    COLOR_GRAY,               /* π */
    COLOR_GRAY,               /* 7 */
    COLOR_GRAY,               /* 8 */
    COLOR_GRAY,               /* 9 */
    COLOR_GRAY,               /* / */
    COLOR_GRAY,               /* ) */
    COLOR_GRAY,               /* nPr */
    COLOR_GRAY,               /* DEG */
    COLOR_GRAY,               /* √ */
    COLOR_GRAY,               /* x^2 */
    COLOR_GRAY,               /* ^ */
    COLOR_GRAY,               /* ( */
    COLOR_GRAY,               /* 1/x */
    COLOR_GRAY,               /* MDF */
    COLOR_YELLOW,             /* 2ndF */
    COLOR_GRAY,               /* sin */
    COLOR_GRAY,               /* cos */
    COLOR_GRAY,               /* ln */
    COLOR_GRAY,               /* log */
    COLOR_GRAY,               /* tan */
    COLOR_GRAY,               /* F←→E */
    COLOR_RED,                /* CLS */
    COLOR_GRAY                /* ON */
};

/* キーの前景色 */
static uint8_t keyForeColor[] = {
    COLOR_LIGHTGRAY,   COLOR_LIGHTGRAY, /* OFF */
    COLOR_LIGHTGRAY,                    /* Q */
    COLOR_LIGHTGRAY,                    /* W */
    COLOR_LIGHTGRAY,                    /* E */
    COLOR_LIGHTGRAY,                    /* R */
    COLOR_LIGHTGRAY,                    /* T */
    COLOR_LIGHTGRAY,                    /* Y */
    COLOR_LIGHTGRAY,                    /* U */
    COLOR_LIGHTGRAY,                    /* A */
    COLOR_LIGHTGRAY,                    /* S */
    COLOR_LIGHTGRAY,                    /* D */
    COLOR_LIGHTGRAY,                    /* F */
    COLOR_LIGHTGRAY,                    /* G */
    COLOR_LIGHTGRAY,                    /* H */
    COLOR_LIGHTGRAY,                    /* J */
    COLOR_LIGHTGRAY,                    /* K */
    COLOR_LIGHTGRAY,                    /* Z */
    COLOR_LIGHTGRAY,                    /* X */
    COLOR_LIGHTGRAY,                    /* C */
    COLOR_LIGHTGRAY,                    /* V */
    COLOR_LIGHTGRAY,                    /* B */
    COLOR_LIGHTGRAY,                    /* N */
    COLOR_LIGHTGRAY,                    /* M */
    COLOR_LIGHTGRAY,                    /* , */
    COLOR_LIGHTGREEN,                   /* BASIC */
    COLOR_LIGHTGREEN,                   /* TEXT */
    COLOR_LIGHTGRAY,                    /* CAPS */
    COLOR_LIGHTGRAY,                    /* カナ */
    COLOR_LIGHTGRAY,                    /* TAB */
    COLOR_LIGHTGRAY,                    /* SPACE */
    COLOR_LIGHTGRAY,                    /* ↓ */
    COLOR_LIGHTGRAY,                    /* ↑ */
    COLOR_LIGHTGRAY,                    /* ← */
    COLOR_LIGHTGRAY,                    /* → */
    COLOR_LIGHTGRAY,                    /* ANS */
    COLOR_LIGHTGRAY,                    /* 0 */
    COLOR_LIGHTGRAY,                    /* . */
    COLOR_LIGHTGRAY,                    /* = */
    COLOR_LIGHTGRAY,                    /* + */
    COLOR_LIGHTGRAY,                    /* Return */
    COLOR_LIGHTGRAY,                    /* L */
    COLOR_LIGHTGRAY,                    /* ; */
    COLOR_LIGHTGRAY,                    /* CONST */
    COLOR_LIGHTGRAY,                    /* 1 */
    COLOR_LIGHTGRAY,                    /* 2 */
    COLOR_LIGHTGRAY,                    /* 3 */
    COLOR_LIGHTGRAY,                    /* - */
    COLOR_LIGHTGRAY,                    /* M+ */
    COLOR_LIGHTGRAY,                    /* I */
    COLOR_LIGHTGRAY,                    /* O */
    COLOR_LIGHTGRAY,                    /* INS */
    COLOR_LIGHTGRAY,                    /* 4 */
    COLOR_LIGHTGRAY,                    /* 5 */
    COLOR_LIGHTGRAY,                    /* 6 */
    COLOR_LIGHTGRAY,                    /* * */
    COLOR_LIGHTGRAY,                    /* R・CM */
    COLOR_LIGHTGRAY,                    /* P */
    COLOR_LIGHTGRAY,                    /* BS */
    COLOR_LIGHTGRAY,                    /* π */
    COLOR_LIGHTGRAY,                    /* 7 */
    COLOR_LIGHTGRAY,                    /* 8 */
    COLOR_LIGHTGRAY,                    /* 9 */
    COLOR_LIGHTGRAY,                    /* / */
    COLOR_LIGHTGRAY,                    /* ) */
    COLOR_LIGHTGRAY,                    /* nPr */
    COLOR_LIGHTGRAY,                    /* DEG */
    COLOR_LIGHTGRAY,                    /* √ */
    COLOR_LIGHTGRAY,                    /* x^2 */
    COLOR_LIGHTGRAY,                    /* ^ */
    COLOR_LIGHTGRAY,                    /* ( */
    COLOR_LIGHTGRAY,                    /* 1/x */
    COLOR_LIGHTGRAY,                    /* MDF */
    COLOR_LIGHTYELLOW,                  /* 2ndF */
    COLOR_LIGHTGRAY,                    /* sin */
    COLOR_LIGHTGRAY,                    /* cos */
    COLOR_LIGHTGRAY,                    /* ln */
    COLOR_LIGHTGRAY,                    /* log */
    COLOR_LIGHTGRAY,                    /* tan */
    COLOR_LIGHTGRAY,                    /* F←→E */
    COLOR_LIGHTRED,                     /* CLS */
    COLOR_LIGHTGRAY                     /* ON */
};

/* キーの前景色(PC-G815) */
const static uint8_t keyForeColorG815[] = {};

/* PCの情報・設定 */
#if SDL_MAJOR_VERSION == 1
const static SDL_VideoInfo *videoInfo; /* ビデオ */
#endif
static SDL_Joystick *joy;   /* ジョイスティック */
static SDL_AudioSpec audio; /* サウンド */

/* LCD */
static SDL_Rect rectLCD;                        /* LCD全体の範囲 */
static SDL_Rect rectLCDmain;                    /* LCDメイン部の範囲 */
static SDL_Rect rectLCDstatus[STATUS_LAST + 1]; /* LCDのステータス部の範囲 */
static int statusRow[STATUS_LAST + 1]; /* ステータスのVRAM上の列 */
static uint8_t statusMask[STATUS_LAST + 1]; /* ステータスのVRAM上のbit位置 */
static int zoomedBpp;                       /* 拡大後の1pixelのbit数 */
static int zoomedPitch; /* 拡大後のLCDメイン部の幅のbyte数 */
static SDL_Color colorTable[256]; /* 色テーブル */
static Uint32 pixelTable[256];    /* pixelテーブル */

/* SurfaceとPixmap(Pixelの集まり) */
#if SDL_MAJOR_VERSION == 2
static SDL_Window *window; /* Window */
#endif
static SDL_Surface *screen;        /* Windowのsurface */
static uint8_t *pixmapBack = NULL; /* LCDの背景のpixmap */
static uint8_t *pixmapDotOff;      /* LCDのドットのpixmap (OFF) */
static uint8_t *pixmapDotOn;       /* LCDのドットのpixmap (ON) */
static uint8_t
    *pixmapStatusOff[STATUS_LAST + 1]; /* LCDのステータスのpixmap (OFF) */
static uint8_t
    *pixmapStatusOn[STATUS_LAST + 1]; /* LCDのステータスのpixmap (ON) */
static uint8_t *pixmapDotTable[MAX_SCALE]; /* LCDのドットのpixmap (各階調) */
static uint8_t
    *pixmapStatusTable[MAX_SCALE][STATUS_LAST + 1]; /* LCDのステータスのpixmap
                                                     (各階調) */
static uint8_t *pixmapButton;                       /* ボタンのpixmap */

/* VRAM */
static uint8_t oldVram[166 * 9]; /* 1フレーム前のVRAM */
static uint8_t oldLcdTop;        /* 1フレーム前のVRAMの表示位置 */

/* キー */
static SDL_Rect rectKey[GKEY_DOUBLE]; /* ソフトウェアキーの範囲 */
static uint8_t autoKey;      /* 自動キー入力中のキーコード */
static int autoKeyCount = 0; /* 自動キー入力カウンタ */

#if SDL_MAJOR_VERSION == 2
/*
        %xx%xx...形式のファイル名をデコードする (下請け)
*/
static char *decodeString(const char *bin) {
  static char str[PATH_MAX];
  const char *r;
  char *w;
  int high, low;

  for (r = bin, w = str; w < str + sizeof(str) - 1 && *r != 0; r++, w++) {
    if (*r == '%') {
      if (*++r == 0)
        break;
      sscanf(r, "%1X", &high);
      if (*++r == 0)
        break;
      sscanf(r, "%1X", &low);
      *w = high << 4 | low;
    } else
      *w = *r;
  }
  *w = 0;

  return str;
}
#endif

/*
        バイナリファイルを読み込む
*/
int readBin(const char *path, void *mem, int mem_size) {
  SDL_RWops *rw;
  int size;

  if (path == NULL || strcmp(path, "") == 0)
    return -1;
  if ((rw = SDL_RWFromFile(path, "rb")) == NULL)
    return -1;

  if (mem_size <= 0)
    size = 0;
  else if (mem != NULL) {
    size = SDL_RWread(rw, mem, 1, (size_t)mem_size);
  } else {
    size = SDL_RWseek(rw, 0, RW_SEEK_END);
    size = (size < mem_size ? size : mem_size);
  }

  SDL_RWclose(rw);

  return size;
}

/*
        バイナリファイルを書き込む
*/
int writeBin(const char *path, const void *mem, int size) {
  SDL_RWops *rw;
  int written_size;

  if (path == NULL || strcmp(path, "") == 0)
    return -1;
  if ((rw = SDL_RWFromFile(path, "wb")) == NULL)
    return -1;

  if (mem != NULL)
    written_size = SDL_RWwrite(rw, mem, 1, size);
  else
    written_size = 0;

  SDL_RWclose(rw);

  return written_size;
}

/*
        IntelHEX形式ファイルを読み込む
*/
int readHex(const char *path, void *mem, int *off, int mem_size, int check) {
  Sint64 size, off64;

  if (path == NULL || strcmp(path, "") == 0)
    return -1;
  if ((size = SDLHex_Load(path, mem, &off64, mem_size, check)) < 0)
    return -1;

  if (off != NULL)
    *off = off64;
  return size;
}

/*
        IntelHEX形式ファイルを読み込む(ファイル内のアドレスを無視する)
*/
int readHexAbs(const char *path, void *mem, int *off, int mem_size, int check) {
  Sint64 size, off64;

  if (path == NULL || strcmp(path, "") == 0)
    return -1;
  if ((size = SDLHex_LoadAbs(path, mem, &off64, mem_size, check)) < 0)
    return -1;

  if (off != NULL)
    *off = off64;
  return size;
}

/*
        IntelHEX形式ファイルを書き込む
*/
int writeHex(const char *path, const void *mem, int off, int size) {
  if (path == NULL || strcmp(path, "") == 0)
    return -1;
  return SDLHex_Save(path, mem, off, size);
}

/*
        IntelHEX形式ファイルを書き込む(ファイル内のアドレスを無視する)
*/
int writeHexAbs(const char *path, const void *mem, int off, int size) {
  if (path == NULL || strcmp(path, "") == 0)
    return -1;
  return SDLHex_SaveAbs(path, mem, off, size);
}

/*
        ファイルをコピーする
*/
int copyFile(const char *src, const char *dst) {
  uint8_t buf[0x8000];
  int size;

  if ((size = readBin(src, buf, sizeof(buf))) < 0)
    return FALSE;
  return writeBin(dst, buf, size) >= 0;
}

/*
        ファイルを削除する
*/
int removeFile(const char *path) {
  SDL_RWops *rw;
#if defined(_WIN32)
  wchar_t w_path[PATH_MAX];
#endif

  if ((rw = SDL_RWFromFile(path, "r")) == NULL)
    return 0;
  SDL_RWclose(rw);

#if defined(_WIN32)
  MultiByteToWideChar(CP_UTF8, 0, path, -1, w_path, sizeof(w_path));
  return DeleteFileW(w_path) ? 0 : -1;
#else
  return unlink(path);
#endif
}

/*
        ファイルを比較する
*/
int cmpFile(const char *path1, const char *path2) {
  uint8_t buf1[0x8000], buf2[0x8000];
  int size;

  if ((size = readBin(path1, buf1, sizeof(buf1))) < 0)
    return FALSE;
  if ((size = readBin(path2, buf2, sizeof(buf2))) < 0)
    return FALSE;
  return memcmp(buf1, buf2, size) == 0;
}

/*
        LCDの濃度の最大値を得る (下請け)
*/
static inline int getScaleMax(void) {
  if (lcdScales == 2)
    return 1;
  else
    return freqUpdateIO / 8;
}

/*
        メインLCDに点を描く (下請け)
*/
static inline void putDot(uint8_t *dst, const void *pix) {
  uint8_t *p = dst;

  for (p = dst; p != dst + zoomedPitch; p += screen->pitch)
    memcpy(p, pix, zoomedBpp);
}

/*
        Pixmapを描く (下請け)
*/
static inline void putPixmap(uint8_t *dst, const uint8_t *pix, int w, int h) {
  uint8_t *p;
  const uint8_t *q;

  for (p = dst, q = pix; p != dst + screen->pitch * h;
       p += screen->pitch, q += w * screen->format->BytesPerPixel)
    memcpy(p, q, w * screen->format->BytesPerPixel);
}

/*
        ステータスを表示する(updateLCD1の下請け)
*/
static inline void updateStatus(SDL_Rect **rect, int status, int chk) {
  int row = statusRow[status], shift = lcdTop % 8;
  uint8_t mask = statusMask[status], pat, *old_pat;
  SDL_Rect *r;

  /* VRAMのアドレスを得る */
  pat = *VRAM(lcdCols, row, lcdTop) >> shift | *VRAM(lcdCols, row + 1, lcdTop)
                                                   << (8 - shift);
  old_pat = OLD_VRAM(lcdCols, row, lcdTop);

  /* ステータスが変化していないなら戻る */
  if (!((pat ^ *old_pat) & mask) && chk)
    return;

  /* ステータスを描く */
  r = &rectLCDstatus[status];
  if (pat & mask) {
    putPixmap(SCREEN(r->x, r->y), pixmapStatusOn[status], r->w, r->h);
    *old_pat |= mask;
  } else {
    putPixmap(SCREEN(r->x, r->y), pixmapStatusOff[status], r->w, r->h);
    *old_pat &= ~mask;
  }

  /* 更新範囲を書き込む */
  *(*rect)++ = *r;
}

/*
        cellを表示する(updateLCD1の下請け)
*/
static inline void updateCell(SDL_Rect **rect, int col, int row, int chk) {
  int begin, d, end;
  uint8_t *p_vram, *p_oldvram, pat, changed, mask;
  uint8_t *p_screen, *p_screen0, *p_screen00;

  /* 仮想VRAMのアドレスを得る */
  p_vram = VRAM(col, row, lcdTop);
  p_oldvram = OLD_VRAM(col, row, 0);
  if (chk) {
    /* 変化なしなら戻る */
    if (memcmp(p_vram, p_oldvram, 6) == 0)
      return;
  } else {
    /* 1フレーム前の仮想VRAMを反転する(再描画のため) */
    mask = 0xff >> (8 - cellHeight);
    *(p_oldvram + 0) = ~*(p_vram + 0) & mask;
    *(p_oldvram + 1) = ~*(p_vram + 1) & mask;
    *(p_oldvram + 2) = ~*(p_vram + 2) & mask;
    *(p_oldvram + 3) = ~*(p_vram + 3) & mask;
    *(p_oldvram + 4) = ~*(p_vram + 4) & mask;
    if (cellWidth == 6)
      *(p_oldvram + 5) = ~*(p_vram + 5) & mask;
  }

  /* Cell内の表示開始終了y座標を求める */
  if ((d = lcdTop % 8) == 0)
    end = begin = 0;
  else if (row == 0) {
    begin = d;
    end = 0;
  } else if (row == lcdRows) {
    begin = 0;
    end = cellHeight - d;
  } else
    end = begin = 0;

  /* 更新範囲とsurfaceのアドレスを求める */
  (*rect)->x = zoom * col * 6 + rectLCDmain.x;
  (*rect)->y = zoom * (row * 8 + begin - d) + rectLCDmain.y;
  (*rect)->w = zoom * cellWidth;
  (*rect)->h = zoom * (cellHeight - end - begin);
  p_screen00 = SCREEN((*rect)->x, (*rect)->y);

  /* Cellを描く */
  for (p_screen0 = p_screen00;
       p_screen0 != p_screen00 + (*rect)->w * screen->format->BytesPerPixel;
       p_screen0 += zoomedBpp) {
    if (*p_vram != *p_oldvram) {
      for (changed =
               ((*p_vram >> begin) ^ (*p_oldvram >> begin)) & (0xff >> end),
          pat = *p_vram >> begin, p_screen = p_screen0;
           changed != 0; changed >>= 1, pat >>= 1, p_screen += zoomedPitch)
        if (changed & 1)
          putDot(p_screen, (pat & 1 ? pixmapDotOn : pixmapDotOff));
      *p_oldvram = *p_vram;
    }
    p_vram++;
    p_oldvram++;
  }

  /* 更新範囲を書き込む */
  if ((*rect)->y == (*rect - 1)->y &&
      (*rect)->x == (*rect - 1)->x + (*rect - 1)->w)
    (*rect - 1)->w += (*rect)->w;
  else
    (*rect)++;
}

/*
        LCDを更新する(残像なし) (updateLCDの下請け)
*/
static int updateLCD1(void) {
  static SDL_Rect rect[255] = {{0, 0, 0, 0}};
  SDL_Rect *p_rect = &rect[1];
  int chk, col, row, status, must_lock;

  /* 表示開始位置が変わったか? */
  if (lcdTop == oldLcdTop)
    chk = TRUE;
  else {
    chk = FALSE;
    oldLcdTop = lcdTop;
  }

  /* Surfaceに書き込む */
  if ((must_lock = SDL_MUSTLOCK(screen)))
    if (SDL_LockSurface(screen) < 0)
      return FALSE;
  for (status = STATUS_FIRST; status != STATUS_LAST + 1; status++)
    updateStatus(&p_rect, status, chk);
  for (row = 0; row != lcdRows + (lcdTop % 8 != 0); row++)
    for (col = 0; col != lcdCols; col++)
      updateCell(&p_rect, col, row, chk);
  if (must_lock)
    SDL_UnlockSurface(screen);

  /* 更新する */
  if (p_rect == &rect[1])
    return FALSE;
#if SDL_MAJOR_VERSION == 2
  SDL_UpdateWindowSurfaceRects(window, &rect[1], (int)(p_rect - rect - 1));
#elif SDL_MAJOR_VERSION == 1
  SDL_UpdateRects(screen, (int)(p_rect - rect - 1), &rect[1]);
#endif

  return TRUE;
}

/*
        LCDを更新する(残像あり) (updateLCDの下請け)
*/
static int updateLCD2(void) {
  static struct {
    uint8_t oldpat[(144 + 1) * 8];
  } page[MAX_SCALE], *p_page = NULL;
  static int lcd_scale[48 * (144 + 1)];
  static SDL_Rect rect[144 * 8] = {{0, 0, 0, 0}};
  SDL_Rect *p_rect = &rect[1];
  int x, x0, y, status, shift, *p_scale;
  uint8_t *p_screen00, *p_screen0, *p_screen, *p;
  uint8_t pat, bit, *p_vram0, *p_vram1, *p_oldpat;

  /* ↓このようにしないと最適化したとき正常に動作しない (gcc4.8.1 win32) */
  if (p_page == NULL)
    p_page = &page[1];
  /* ↑ */

  if (SDL_MUSTLOCK(screen))
    if (SDL_LockSurface(screen) < 0)
      return FALSE;

  /* ステータスを書き込む */
  shift = lcdTop % 8;
  p_scale = lcd_scale;
  for (status = STATUS_FIRST; status != STATUS_LAST + 1; status++) {
    p_vram0 = &vram[((statusRow[status] + lcdTop / 8 + 0) % 8) * vramWidth +
                    vramWidth - 1];
    p_vram1 = &vram[((statusRow[status] + lcdTop / 8 + 1) % 8) * vramWidth +
                    vramWidth - 1];
    pat = *p_vram0 >> shift | *p_vram1 << (8 - shift);
    p_oldpat = &p_page->oldpat[statusRow[status] * vramWidth + vramWidth - 1];
    if (!((pat ^ *p_oldpat) & statusMask[status])) {
      p_scale++;
      continue;
    }

    if (*p_oldpat & statusMask[status])
      (*p_scale)--;
    if (pat & statusMask[status])
      (*p_scale)++;

    *p_rect = rectLCDstatus[status];
    putPixmap(SCREEN(p_rect->x, p_rect->y),
              pixmapStatusTable[*p_scale++][status], p_rect->w, p_rect->h);
    p_rect++;

    if (pat & statusMask[status])
      *p_oldpat |= statusMask[status];
    else
      *p_oldpat &= ~statusMask[status];
  }

  /* メインLCDを書き込む */
  p_vram0 = &vram[((lcdTop / 8) + 0) * vramWidth];
  p_vram1 = p_vram0 + vramWidth;
  p_oldpat = p_page->oldpat;
  for (y = 0, p_screen00 = SCREEN(rectLCDmain.x, rectLCDmain.y); y != lcdHeight;
       y += 8, p_screen00 += zoomedPitch * 8) {
    if (p_vram0 == &vram[vramRows * vramWidth])
      p_vram0 = vram;
    if (p_vram1 == &vram[vramRows * vramWidth])
      p_vram1 = vram;

    for (x0 = 0, p_screen0 = p_screen00; x0 != 144;
         x0 += 6, p_screen0 += zoomedBpp * 6) {
      for (x = x0, p_screen = p_screen0; x != x0 + cellWidth;
           x++, p_screen += zoomedBpp, p_vram0++, p_vram1++, p_oldpat++) {
        pat = *p_vram0 >> shift | *p_vram1 << (8 - shift);
        if (pat == *p_oldpat) {
          p_scale += cellHeight;
          continue;
        }

        for (p = p_screen, bit = 1; p != p_screen + cellHeight * zoomedPitch;
             p += zoomedPitch, bit <<= 1) {
          if (*p_oldpat & bit)
            (*p_scale)--;
          if (pat & bit)
            (*p_scale)++;

          putDot(p, pixmapDotTable[*p_scale]);
          p_scale++;
        }
        *p_oldpat = pat;

        p_rect->x = x * zoom + rectLCDmain.x;
        p_rect->y = y * zoom + rectLCDmain.y;
        p_rect->w = 1 * zoom;
        p_rect->h = cellHeight * zoom;
        if (p_rect->y == (p_rect - 1)->y &&
            p_rect->x == (p_rect - 1)->x + (p_rect - 1)->w)
          (p_rect - 1)->w += p_rect->w;
        else
          p_rect++;
      }
    }

    p_vram0++;
    p_vram1++;
    p_oldpat++;
  }
  p_page = (p_page != &page[getScaleMax() - 1] ? p_page + 1 : page);

  if (SDL_MUSTLOCK(screen))
    SDL_UnlockSurface(screen);

  /* 更新する */
  if (p_rect == &rect[1])
    return FALSE;
#if SDL_MAJOR_VERSION == 2
  SDL_UpdateWindowSurfaceRects(window, &rect[1], (int)(p_rect - &rect[1]));
#elif SDL_MAJOR_VERSION == 1
  SDL_UpdateRects(screen, (int)(p_rect - &rect[1]), &rect[1]);
#endif

  return TRUE;
}

/*
        状態表示を更新する (updateLCDの下請け)
*/
static void updateCaption(void) {
  static char old_caption[512] = "*";
  char caption[512], buf[512];

  switch (sioMode) {
  case SIO_MODE_STOP:
    strcpy(caption, "");
    break;
  case SIO_MODE_IN:
    sprintf(caption, " - IN:%d/%d %s", sioCount / 14, sioBufferSize, pathSioIn);
    break;
  case SIO_MODE_OUT:
    sprintf(caption, " - OUT:%d %s", sioCount / 10, pathSioOut);
    break;
  }

  if (strcmp(caption, old_caption) == 0)
    return;
  strcpy(old_caption, caption);

  sprintf(buf, "%sPOCKET COMPUTER %s%s", (useROM ? "" : "SIM|"),
          machineName[machineSub], caption);
#if SDL_MAJOR_VERSION == 2
  SDL_SetWindowTitle(window, buf);
#elif SDL_MAJOR_VERSION == 1
  SDL_WM_SetCaption(buf, NULL);
#endif
}

/*
        LCDを更新する
*/
int updateLCD(void) {
  updateCaption();
  if (lcdScales == 2)
    return updateLCD1();
  else
    return updateLCD2();
}

/*
        LCDをクリアする (updateLCDContrastの下請け)
*/
static void clearLCD(void) {
  uint8_t *p;

  /* Surfaceをクリアする */
  if (SDL_MUSTLOCK(screen))
    if (SDL_LockSurface(screen) < 0)
      return;
  for (p = screen->pixels + (screen->pitch * rectLCD.y +
                             screen->format->BytesPerPixel * rectLCD.x);
       p != screen->pixels + (screen->pitch * (rectLCD.y + rectLCD.h) +
                              screen->format->BytesPerPixel * rectLCD.x);
       p += screen->pitch)
    memcpy(p, pixmapBack, rectLCD.w * screen->format->BytesPerPixel);
  if (SDL_MUSTLOCK(screen))
    SDL_UnlockSurface(screen);

    /* 更新する */
#if SDL_MAJOR_VERSION == 2
  SDL_UpdateWindowSurface(window);
#elif SDL_MAJOR_VERSION == 1
  SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);
#endif

  oldLcdTop = 0xff;
}

/*
        塗りつぶしたPixmapを作成する (updateLCDContrastの下請け)
*/
static uint8_t *fillPixmap(uint8_t *pixmap, const void *pix, int size) {
  uint8_t *p;

  for (p = pixmap; p < pixmap + size; p += screen->format->BytesPerPixel)
    memcpy(p, pix, screen->format->BytesPerPixel);
  return pixmap;
}

/*
        BitmapをPixmapに変換する (updateLCDContrastの下請け)
*/
static uint8_t *makePixmap(uint8_t *pixmap, SDL_Rect rect, Bitmap bitmap,
                           const void *pix0, const void *pix1) {
  int i, x, y;
  uint8_t mask, *r = bitmap.image;
  uint8_t *p, *q;

  if (rect.w <= 0)
    return NULL;

  fillPixmap(pixmap, pix0, rect.w * rect.h * screen->format->BytesPerPixel);
  for (y = 0, p = pixmap; y != bitmap.height && y != rect.h;
       y++, p += rect.w * screen->format->BytesPerPixel) {
    q = p;
    for (x = 0; x < bitmap.width; x += 8) {
      for (i = x, mask = 0x80; i < x + 8 && i < bitmap.width && i < rect.w;
           i++, mask >>= 1) {
        if (*r & mask)
          memcpy(q, pix1, screen->format->BytesPerPixel);
        q += screen->format->BytesPerPixel;
      }
      r++;
    }
  }

  return pixmap;
}

/*
        LCD電圧を求める (UpdateLCDContrastの下請け)
*/
static int getLcdVoltage(void) {
  int voltage;

  if ((lcdEffectBlack && lcdEffectWhite) || lcdDisabled)
    return 0;
  else if (lcdEffectBlack)
    return 0x20;
  else if (lcdEffectWhite)
    return 0;
  else {
    if ((voltage = lcdContrast + (lcdEffectDark ? 2 : 0)) > 0x20)
      return 0x20;
    else
      return voltage;
  }
}

/*
        LCDの色を求める (UpdateLCDContrastの下請け)
*/
static SDL_Color *getLcdColors(SDL_Color *colors, int voltage) {
  SDL_Color back = {0}, on = {0}, off = {0};
  int c1, c2;

  /* 色を得る */
  back.r = (colorBack >> 16) & 0xff;
  back.g = (colorBack >> 8) & 0xff;
  back.b = (colorBack >> 0) & 0xff;
  if (!lcdEffectReverse) {
    off.r = (colorOff >> 16) & 0xff;
    off.g = (colorOff >> 8) & 0xff;
    off.b = (colorOff >> 0) & 0xff;
    on.r = (colorOn >> 16) & 0xff;
    on.g = (colorOn >> 8) & 0xff;
    on.b = (colorOn >> 0) & 0xff;
  } else {
    off.r = (colorOn >> 16) & 0xff;
    off.g = (colorOn >> 8) & 0xff;
    off.b = (colorOn >> 0) & 0xff;
    on.r = (colorOff >> 16) & 0xff;
    on.g = (colorOff >> 8) & 0xff;
    on.b = (colorOff >> 0) & 0xff;
  }
  if (voltage <= 0x0f) {
    c1 = 0x0f - voltage;
    c2 = voltage;
    colors[0] = back;
    colors[1].r = (back.r * c1 + off.r * c2) / 0x0f;
    colors[1].g = (back.g * c1 + off.g * c2) / 0x0f;
    colors[1].b = (back.b * c1 + off.b * c2) / 0x0f;
    colors[2].r = (back.r * c1 + on.r * c2) / 0x0f;
    colors[2].g = (back.g * c1 + on.g * c2) / 0x0f;
    colors[2].b = (back.b * c1 + on.b * c2) / 0x0f;
  } else {
    c1 = 0x20 - voltage;
    c2 = voltage - 0x0f;
    colors[0].r = (back.r * c1 + off.r * c2) / 0x11;
    colors[0].g = (back.g * c1 + off.g * c2) / 0x11;
    colors[0].b = (back.b * c1 + off.b * c2) / 0x11;
    colors[1].r = (off.r * c1 + on.r * c2) / 0x11;
    colors[1].g = (off.g * c1 + on.g * c2) / 0x11;
    colors[1].b = (off.b * c1 + on.b * c2) / 0x11;
    colors[2] = on;
  }

  return colors;
}

/*
        LCDの色(残像)を求める (UpdateLCDContrastの下請け)
*/
static SDL_Color getLcdColorScale(SDL_Color off, SDL_Color on, int max, int i) {
  SDL_Color result = {0};

  if (lcdScales != 0)
    i = (i * lcdScales / (max + 1)) * max / (lcdScales - 1);
  result.r = (off.r * (max - i) + on.r * i) / max;
  result.g = (off.g * (max - i) + on.g * i) / max;
  result.b = (off.b * (max - i) + on.b * i) / max;
  return result;
}

/*
        コントラストを更新する
*/
void updateLCDContrast(void) {
  const SDL_Color color_black = {0x00, 0x00, 0x00, 0xff};
  const SDL_Color color_gray = {0x50, 0x50, 0x70, 0xff};
  const SDL_Color color_green = {0x20, 0x70, 0x60, 0xff};
  const SDL_Color color_yellow = {0xb0, 0x90, 0x30, 0xff};
  const SDL_Color color_red = {0xb0, 0x50, 0x50, 0xff};
  const SDL_Color color_lightgray = {0xcc, 0xcc, 0xee, 0xff};
  const SDL_Color color_lightgreen = {0xdd, 0xff, 0xcc, 0xff};
  const SDL_Color color_lightyellow = {0xff, 0xee, 0xdd, 0xff};
  const SDL_Color color_lightred = {0xff, 0xcc, 0xcc, 0xff};
  const SDL_Color color_midgreen = {0x88, 0xff, 0x77, 0xff};
  const SDL_Color color_midyellow = {0xff, 0xdd, 0x66, 0xff};
  const SDL_Color color_midred = {0xff, 0x55, 0x55, 0xff};
  const SDL_Color color_white = {0xff, 0xff, 0xff, 0xff};
  SDL_Color base[3];
  int i, cont, z, status, voltage = getLcdVoltage(), max = getScaleMax();

  /* 色を作る */
  colorTable[COLOR_BODY] = color_black;
  colorTable[COLOR_GRAY] = color_gray;
  colorTable[COLOR_GREEN] = color_green;
  colorTable[COLOR_YELLOW] = color_yellow;
  colorTable[COLOR_RED] = color_red;
  colorTable[COLOR_MIDGREEN] = color_midgreen;
  colorTable[COLOR_MIDYELLOW] = color_midyellow;
  colorTable[COLOR_MIDRED] = color_midred;
  if (zoom > 1)
    colorTable[COLOR_LIGHTGRAY] = colorTable[COLOR_LIGHTGREEN] =
        colorTable[COLOR_LIGHTYELLOW] = colorTable[COLOR_LIGHTRED] =
            color_white;
  else {
    colorTable[COLOR_LIGHTGRAY] = color_lightgray;
    colorTable[COLOR_LIGHTGREEN] = color_lightgreen;
    colorTable[COLOR_LIGHTYELLOW] = color_lightyellow;
    colorTable[COLOR_LIGHTRED] = color_lightred;
  }

  /* LCDの色を作る */
  getLcdColors(base, voltage);
  colorTable[COLOR_LCD_START + 0] = base[0];
  for (cont = 0, i = COLOR_LCD_START + 1; cont <= max; cont++, i++)
    colorTable[i] = getLcdColorScale(base[1], base[2], max, cont);

  /* Pixelを作る */
  if (screen->format->BytesPerPixel == 1) {
    for (i = 0; i < 256; i++)
      *(uint8_t *)&pixelTable[i] = i;
#if SDL_MAJOR_VERSION == 2
    SDL_SetPaletteColors(screen->format->palette, colorTable, 0,
                         COLOR_LCD_START + max + 2);
#elif SDL_MAJOR_VERSION == 1
    SDL_SetPalette(screen, SDL_PHYSPAL, colorTable, 0,
                   COLOR_LCD_START + max + 2);
#endif
  } else {
    for (i = 0; i <= COLOR_LCD_START + max + 1; i++)
      pixelTable[i] = SDL_MapRGB(screen->format, colorTable[i].r,
                                 colorTable[i].g, colorTable[i].b);
  }

  /* 画像(コピー元)を作る */
  fillPixmap(pixmapBack, &pixelTable[COLOR_LCD_START + 0], zoomedPitch);
  for (cont = 0, i = COLOR_LCD_START + 1; cont <= max; cont++, i++)
    fillPixmap(pixmapDotTable[cont], &pixelTable[i], zoomedBpp);
  z = (zoom < MAX_ZOOM ? zoom : MAX_ZOOM) - 1;
  for (status = STATUS_FIRST; status <= STATUS_LAST; status++) {
    makePixmap(pixmapStatusOff[status], rectLCDstatus[status],
               bmpStatus[z][status], &pixelTable[COLOR_LCD_START + 0],
               &pixelTable[COLOR_LCD_START + 1]);
    for (cont = 0, i = COLOR_LCD_START + 1; cont <= max; cont++, i++)
      makePixmap(pixmapStatusTable[cont][status], rectLCDstatus[status],
                 bmpStatus[z][status], &pixelTable[COLOR_LCD_START],
                 &pixelTable[i]);
  }

  /* 画面を再表示する */
  clearLCD();
  updateLCD1();
}

/*
        位置を決める (updateLayoutの下請け)
*/
static SDL_Rect setLayout(Rect r) {
  SDL_Rect result;

  result.x = r.x * zoom / PIXEL_WIDTH;
  result.y = r.y * zoom / PIXEL_HEIGHT;
  result.w = r.w * zoom / PIXEL_WIDTH;
  result.h = r.h * zoom / PIXEL_HEIGHT;
  return result;
}

/*
        レイアウトを更新する
*/
void updateLayout(void) {
  const Machineinfo *m;
  SDL_Surface none = {0};
  SDL_Surface *info_image = NULL;
  SDL_Rect rect_function_key;
  int i, gkey, size, status, w, h, z = (zoom < MAX_ZOOM ? zoom : MAX_ZOOM) - 1,
                                   first = (pixmapBack == NULL),
                                   info_bmp_err = FALSE;
  char buf[256];

  /* タイトルバーにマシン名を表示する */
  m = machineInfo[machine];
  sprintf(buf, "POCKET COMPUTER %s", machineName[machineSub]);
#if SDL_MAJOR_VERSION == 2
  SDL_SetWindowTitle(window, buf);
#elif SDL_MAJOR_VERSION == 1
  SDL_WM_SetCaption(buf, NULL);
#endif

  /* ステータスのVRAM上の位置を設定する */
  memcpy(statusRow, m->status_row, sizeof(statusRow));
  memcpy(statusMask, m->status_mask, sizeof(statusMask));

  /* 説明画像を読み込む */
  if (pathInfoImage == NULL)
    info_image = &none;
  else if ((info_image = SDL_LoadBMP(pathInfoImage)) == NULL) {
    info_image = &none;
    info_bmp_err = TRUE;
  }

  /* レイアウトを設定する */
  rectLCD = setLayout(m->pos_lcd);
  rectLCD.y += info_image->h;

  rectLCDmain = setLayout(m->pos_lcd_main);
  rectLCDmain.y += info_image->h;

  for (i = 0; i < sizeof(rectKey) / sizeof(rectKey[0]); i++) {
    rectKey[i] = setLayout(m->pos_key[i]);
    rectKey[i].y += info_image->h;
  }

  for (status = STATUS_FIRST; status <= STATUS_LAST; status++) {
    rectLCDstatus[status] = setLayout(m->pos_lcd_status[status]);
    rectLCDstatus[status].y += info_image->h;
  }

  rect_function_key = setLayout(m->pos_function_key);

  /* ウィンドウを表示する */
  w = MAX(rectLCD.x + rectLCD.w + (useSoftwareKey ? rect_function_key.w : 0),
          info_image->w);
  h = rectLCD.y + rectLCD.h;
#if SDL_MAJOR_VERSION == 2
  SDL_SetWindowSize(window, w, h);
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(window);
  if ((screen = SDL_GetWindowSurface(window)) == NULL) {
    fprintf(stderr, "SDL_GetWindowSurface fail. %s\n", SDL_GetError());
    exit(1);
  }
#elif SDL_MAJOR_VERSION == 1
  if ((screen = SDL_SetVideoMode(
           w, h, videoInfo->vfmt->BitsPerPixel,
           SDL_HWSURFACE | (videoInfo->vfmt->BitsPerPixel <= 8 ? SDL_HWPALETTE
                                                               : 0))) == NULL) {
    fprintf(stderr, "SDL_SetVideoMode fail. %s\n", SDL_GetError());
    exit(1);
  }
#endif

  /* Pixmapを作成する */
  zoomedBpp = screen->format->BytesPerPixel * zoom;
  zoomedPitch = screen->pitch * zoom;
  pixmapBack = (first ? malloc(zoomedPitch) : realloc(pixmapBack, zoomedPitch));
  for (i = 0; i < sizeof(pixmapDotTable) / sizeof(pixmapDotTable[0]); i++)
    pixmapDotTable[i] =
        (first ? malloc(zoomedBpp) : realloc(pixmapDotTable[i], zoomedBpp));
  pixmapDotOff = pixmapDotTable[0];
  pixmapDotOn = pixmapDotTable[getScaleMax()];
  for (status = STATUS_FIRST; status <= STATUS_LAST; status++) {
    size = rectLCDstatus[status].w * rectLCDstatus[status].h *
           screen->format->BytesPerPixel;
    pixmapStatusOff[status] =
        (first ? malloc(size) : realloc(pixmapStatusOff[status], size));
    for (i = 0; i < sizeof(pixmapStatusTable) / sizeof(pixmapStatusTable[0]);
         i++)
      pixmapStatusTable[i][status] =
          (first ? malloc(size) : realloc(pixmapStatusTable[i][status], size));
    pixmapStatusOff[status] = pixmapStatusTable[0][status];
    pixmapStatusOn[status] = pixmapStatusTable[getScaleMax()][status];
  }

  /* コントラストを更新する */
  updateLCDContrast();

  /* 説明画像を表示する */
  if (info_image->pixels != NULL) {
    SDL_BlitSurface(info_image, NULL, screen, NULL);
    SDL_FreeSurface(info_image);
  }

  /* ボタンを表示する */
  if (useSoftwareKey) {
    size = rectKey[GKEY_OFF].w * rectKey[GKEY_OFF].h *
           screen->format->BytesPerPixel;
    pixmapButton = (first ? malloc(size) : realloc(pixmapButton, size));
    for (gkey = GKEY_NONE; gkey <= GKEY_BREAK; gkey++) {
      if (rectKey[gkey].w <= 0)
        continue;

      makePixmap(pixmapButton, rectKey[gkey], bmpKeytop[z][gkey],
                 &pixelTable[keyBackColor[gkey]],
                 &pixelTable[keyForeColor[gkey]]);
      putPixmap(SCREEN(rectKey[gkey].x, rectKey[gkey].y), pixmapButton,
                rectKey[gkey].w, rectKey[gkey].h);
    }
  }

  /* 説明画像が読み込めなかったときエラーを表示する */
  if (info_bmp_err)
    popup("!", "CANNOT OPEN %s", pathInfoImage);
}

/*
        キーを押す(updateKeyの下請け)
*/
static inline uint8_t pressKey(uint8_t k) {
  switch (k) {
  case GKEY_NONE:
    return 0;
  case GKEY_BREAK:
    keyBreak |= 0x80;
    return INTERRUPT_KON;
  default:
    k = (k & 0xff) - 1;
    keyMatrix[k / 8] |= (1 << (k % 8));
    return INTERRUPT_IA;
  case GKEY_SHIFT:
    keyShift |= 0x01;
    return INTERRUPT_IA;
  case GKEY_RESET:
    keyReset = TRUE;
    return 0;
  case GKEY_MENU:
    return 0;
  case GKEY_DEBUG:
    return 0;
  case GKEY_11PIN1:
    pin11In |= 0x01;
    updateSerial();
    return 0;
  case GKEY_11PIN2:
    pin11In |= 0x02;
    updateSerial();
    return 0;
  case GKEY_11PIN3:
    pin11In |= 0x04;
    updateSerial();
    return 0;
  case GKEY_11PIN4:
    pin11In |= 0x08;
    updateSerial();
    return 0;
  case GKEY_11PIN5:
    pin11In |= 0x10;
    updateSerial();
    return 0;
  case GKEY_11PIN6:
    pin11In |= 0x20;
    updateSerial();
    return 0;
  case GKEY_11PIN7:
    pin11In |= 0x40;
    updateSerial();
    return 0;
  case GKEY_11PIN8:
    pin11In |= 0x80;
    updateSerial();
    return 0;
  }
}

/*
        キーを離す(updateKeyの下請け)
*/
static inline void releaseKey(uint8_t k) {
  switch (k) {
  case GKEY_NONE:
    break;
  case GKEY_BREAK:
    keyBreak &= ~0x80;
    break;
  default:
    k = (k & 0xff) - 1;
    keyMatrix[k / 8] &= ~(1 << (k % 8));
    break;
  case GKEY_SHIFT:
    keyShift &= ~0x01;
    break;
  case GKEY_RESET:
    keyReset = FALSE;
    break;
  case GKEY_MENU:
    if (emulatorMode != EMULATOR_MODE_MENU) {
      emulatorMode = EMULATOR_MODE_MENU;
      menu();
      emulatorMode = EMULATOR_MODE_RUN;
    }
    break;
  case GKEY_DEBUG:
    z80.i.trace = !z80.i.trace;
    break;
  case GKEY_11PIN1:
    pin11In &= ~0x01;
    updateSerial();
    break;
  case GKEY_11PIN2:
    pin11In &= ~0x02;
    updateSerial();
    break;
  case GKEY_11PIN3:
    pin11In &= ~0x04;
    updateSerial();
    break;
  case GKEY_11PIN4:
    pin11In &= ~0x08;
    updateSerial();
    break;
  case GKEY_11PIN5:
    pin11In &= ~0x10;
    updateSerial();
    break;
  case GKEY_11PIN6:
    pin11In &= ~0x20;
    updateSerial();
    break;
  case GKEY_11PIN7:
    pin11In &= ~0x40;
    updateSerial();
    break;
  case GKEY_11PIN8:
    pin11In &= ~0x80;
    updateSerial();
    break;
  }
}

/*
        キー状態を更新する
*/
uint8_t updateKey(void) {
  static uint16 pressedKey[KEY_LAST + 1];
  static uint8_t pressedSwKey = GKEY_NONE;
  SDL_Event e;
  const SDL_Rect *rect_key;
#if SDL_MAJOR_VERSION == 2
  SDL_Keymod mod;
#elif SDL_MAJOR_VERSION == 1
  SDLMod mod;
#endif
  int key;
  uint16 gkey;
  uint8_t itype = 0;

  /* イベント処理 */
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_KEYDOWN: /* キーを押した */
                      /* SDLのキーコードを得る */
#if SDL_MAJOR_VERSION == 2
      if (e.key.repeat)
        break;
      key = e.key.keysym.scancode;
#elif SDL_MAJOR_VERSION == 1
      key = e.key.keysym.sym;
#endif
      if (key == 0)
        break;

      /* PC-G800のキーコードを得る */
      mod = SDL_GetModState();
      if ((mod & KMOD_CTRL) && (gkey = keyConvCtrl[key]) != 0)
        gkey = keyConvCtrl[key];
      else if ((mod & KMOD_ALT) && keyConvAlt[key] != 0)
        gkey = keyConvAlt[key];
      else if ((mod & KMOD_SHIFT) && keyConvShift[key] != 0)
        gkey = keyConvShift[key];
      else
        gkey = keyConv[key];
      if (gkey == KEY_NONE)
        break;

      /* 押したSDLのキーに割り付けられたPC-G800のキーを記憶する */
      pressedKey[key] = gkey & ~GMODKEY_MASK;

#if SDL_MAJOR_VERSION == 2
      /* クリップボードからの貼り付けか */
      if (gkey == GKEY_PASTE) {
        if (SDL_HasClipboardText())
          setAutoKeyText(SDL_GetClipboardText(), TRUE);
        break;
        /* クリップボードへのコピーか? */
      } else if (gkey == GKEY_COPY) {
        uint8_t ans[32];
        if (decodeNum(ans, &memory[0x79a0]) >= 0) {
          char utf8[32];
          SDL_SetClipboardText(ankToUtf8(ans, utf8));
        }
      }
#endif

      /* ロックされるキーの場合は自動入力として処理する */
      if (
#if SDL_MAJOR_VERSION == 2
          key == KEY_CAPSLOCK
#elif SDL_MAJOR_VERSION == 1
          key == KEY_CAPSLOCK || key == KEY_NUMLOCK || key == KEY_SCROLLOCK
#endif
      ) {
        setAutoKey(gkey);
        break;
      }

      /* PC-G800のキーを押す */
      itype |= pressKey(gkey & ~GMODKEY_MASK);

      /* シフトキーを押す・離す */
      if ((gkey & GMODKEY_NOSHIFT) && (keyShift & 0x01)) {
        releaseKey(GKEY_SHIFT);
        pressedKey[key] |= GMODKEY_NOSHIFT;
      } else if ((gkey & GMODKEY_SHIFT) && !(keyShift & 0x01)) {
        itype |= pressKey(GKEY_SHIFT);
        pressedKey[key] |= GMODKEY_SHIFT;
      }
      break;
    case SDL_KEYUP: /* キーを離した */
                    /* SDLのキーコードを得る */
#if SDL_MAJOR_VERSION == 2
      if (e.key.repeat)
        break;
      key = e.key.keysym.scancode;
#elif SDL_MAJOR_VERSION == 1
      key = e.key.keysym.sym;
#endif
      if (key == 0)
        break;

      /* PC-G800のキーコードを得る */
      gkey = pressedKey[key];

#if SDL_MAJOR_VERSION == 1
      /* SDL1.2の場合はキーのロックを解除したときも押した処理をする */
      if (key == KEY_CAPSLOCK || key == KEY_NUMLOCK || key == KEY_SCROLLOCK) {
        setAutoKey(gkey);
        break;
      }
#endif

      /* PC-G800のキーを離す */
      releaseKey(gkey & ~GMODKEY_MASK);

      /* シフトキーを押す・離す */
      if (gkey & GMODKEY_NOSHIFT) {
        if (!(keyShift & 0x01) &&
            (pressedKey[KEY_LSHIFT] || pressedKey[KEY_RSHIFT]))
          itype |= pressKey(GKEY_SHIFT);
      } else if (gkey & GMODKEY_SHIFT) {
        if ((keyShift & 0x01) &&
            !(pressedKey[KEY_LSHIFT] || pressedKey[KEY_RSHIFT]))
          releaseKey(GKEY_SHIFT);
      }
      pressedKey[key] = GKEY_NONE;
      break;
    case SDL_JOYAXISMOTION: /* ジョイパッドを動かした */
      if (e.jaxis.axis == 0) {
        if (e.jaxis.value < -32768 / 10)
          itype |= pressKey(joyLeft);
        else
          releaseKey(joyLeft);
        if (e.jaxis.value > 32767 / 10)
          itype |= pressKey(joyRight);
        else
          releaseKey(joyRight);
      } else if (e.jaxis.axis == 1) {
        if (e.jaxis.value < -32768 / 10)
          itype |= pressKey(joyUp);
        else
          releaseKey(joyUp);
        if (e.jaxis.value > 32767 / 10)
          itype |= pressKey(joyDown);
        else
          releaseKey(joyDown);
      }
      break;
    case SDL_JOYBUTTONDOWN: /* ジョイパッドのボタンを押した */
      if (e.jbutton.button >= JOY_BUTTONS ||
          joyButton[e.jbutton.button] == 0xff)
        break;
      itype |= pressKey(joyButton[e.jbutton.button]);
      break;
    case SDL_JOYBUTTONUP: /* ジョイパッドのボタンを離した */
      if (e.jbutton.button >= JOY_BUTTONS ||
          joyButton[e.jbutton.button] == 0xff)
        break;
      releaseKey(joyButton[e.jbutton.button]);
      break;
    case SDL_MOUSEBUTTONDOWN: /* マウスのボタンを押した */
      if (e.button.button == SDL_BUTTON_LEFT) {
        if (!useSoftwareKey)
          break;
        for (rect_key = &rectKey[GKEY_OFF]; rect_key <= &rectKey[GKEY_BREAK];
             rect_key++)
          if (rect_key->x <= e.button.x &&
              e.button.x < rect_key->x + rect_key->w &&
              rect_key->y <= e.button.y &&
              e.button.y < rect_key->y + rect_key->h) {
            pressedSwKey = (uint8_t)(rect_key - rectKey);
            itype |= pressKey(pressedSwKey);
          }
      } else if (e.button.button == SDL_BUTTON_MIDDLE) {
#if SDL_MAJOR_VERSION == 2
        if (SDL_HasClipboardText())
          setAutoKeyText(SDL_GetClipboardText(), TRUE);
#endif
      }
      break;
    case SDL_MOUSEBUTTONUP: /* マウスのボタンを離した */
      if (!useSoftwareKey)
        break;
      if (pressedSwKey == GKEY_NONE)
        break;
      releaseKey(pressedSwKey);
      pressedSwKey = GKEY_NONE;
      break;
#if SDL_MAJOR_VERSION == 2
    case SDL_DROPFILE: /* ファイルをドロップした */
      SDL_RaiseWindow(window);

      if (useROM) {
        if (sioLoad(e.drop.file) <= 0)
          sioLoad(decodeString(e.drop.file));
      } else {
        if (readHex(e.drop.file, memory, NULL, sizeof(memory), FALSE) >= 0)
          storeRAM(pathRAM);
        else if (inportBas(e.drop.file))
          ;
        else
          popup("!", "CANNOT OPEN %s", e.drop.file);
      }
      SDL_free(e.drop.file);
      break;
    case SDL_WINDOWEVENT:
      if (e.window.event == SDL_WINDOWEVENT_EXPOSED)
        SDL_UpdateWindowSurface(window);
      break;
#endif
    case SDL_QUIT: /* 終了した */
      if (!closeAsOff)
        exit(0);
      pressKey(GKEY_OFF);
      break;
    }
  }

  /* 自動キー入力処理 */
  if (autoKeyCount == 0) {
    /* キーを押す */
    if ((autoKey = getAutoKey()) != KEY_NONE) {
      itype |= pressKey(autoKey);
      autoKeyCount++;
    }
  } else if (autoKeyCount == freqUpdateIO / 20) {
    /* キーを離す */
    releaseKey(autoKey);
    autoKeyCount++;
  } else if (autoKeyCount == 2 * freqUpdateIO / 20) {
    /* 次のキーを得る */
    autoKeyCount = 0;
  } else {
    /* 待つ */
    autoKeyCount++;
  }

  return itype;
}

/*
        待つ
*/
int delay(int interval) {
  static Uint32 last = 0, left;
  Uint32 now;

  if (noWait)
    interval = 0;

  now = SDL_GetTicks();
  if (last + interval > now) {
    left = last + interval - now;
    SDL_Delay(left);
    last = now + left;
    return FALSE;
  } else {
    last = now;
    return TRUE;
  }
}

/*
        シリアルポートを更新する(テスト)
*/
void updateSerial(void) {
  char buf[256];

  if (!serialTest)
    return;

  switch (pin11If) {
  case PIN11IF_3IO:
    sprintf(buf, "3in 3out [%s %s %s %s %s %s]",
            (io3Out & 0x01 ? "BUSY" : "----"),
            (io3Out & 0x02 ? "Dout" : "----"), (pin11In & 0x04 ? "Xin" : "---"),
            (io3Out & 0x80 ? "Xout" : "----"), (pin11In & 0x10 ? "Din" : "---"),
            (pin11In & 0x20 ? "ACK" : "---"));
    break;
  case PIN11IF_8PIO:
    sprintf(buf, "PIO [%s %s %s %s %s %s %s %s]",
            (pio8Io & 0x01 ? (pin11In & 0x01 ? "I" : "v")
                           : (pio8Out & 0x01 ? "O" : "^")),
            (pio8Io & 0x02 ? (pin11In & 0x02 ? "I" : "v")
                           : (pio8Out & 0x02 ? "O" : "^")),
            (pio8Io & 0x04 ? (pin11In & 0x04 ? "I" : "v")
                           : (pio8Out & 0x04 ? "O" : "^")),
            (pio8Io & 0x08 ? (pin11In & 0x08 ? "I" : "v")
                           : (pio8Out & 0x08 ? "O" : "^")),
            (pio8Io & 0x10 ? (pin11In & 0x10 ? "I" : "v")
                           : (pio8Out & 0x10 ? "O" : "^")),
            (pio8Io & 0x20 ? (pin11In & 0x20 ? "I" : "v")
                           : (pio8Out & 0x20 ? "O" : "^")),
            (pio8Io & 0x40 ? (pin11In & 0x40 ? "I" : "v")
                           : (pio8Out & 0x40 ? "O" : "^")),
            (pio8Io & 0x80 ? (pin11In & 0x80 ? "I" : "v")
                           : (pio8Out & 0x80 ? "O" : "^")));
    break;
  case PIN11IF_UART:
    strcpy(buf, "UART []");
    break;
  default:
    strcpy(buf, "Unknown");
    break;
  }
#if SDL_MAJOR_VERSION == 2
  SDL_SetWindowTitle(window, buf);
#elif SDL_MAJOR_VERSION == 1
  SDL_WM_SetCaption(buf, NULL);
#endif
}

/*
        音を出力する (コールバック関数)
*/
static SDLCALL void playSound(void *unused, uint8_t *stream, int len) {
#if SDL_MAJOR_VERSION == 2
  memset(stream, audio.silence, len);
#endif
  SDL_MixAudio(stream, soundReadBuffer, len, SDL_MIX_MAXVOLUME);
  soundPlayed = TRUE;
}

/*
        環境依存部分を終了する (コールバック関数)
*/
void quitDepend(void) {
  if (buzzer != BUZZER_NONE)
    SDL_CloseAudio();
  SDL_Quit();
}

/*
        環境依存部分を初期化する
*/
int initDepend(void) {
  SDL_Surface *icon;
  uint8_t mask[32 * 32 / 8];
  int i;

  if (z80.i.trace)
    SDL_SetMainReady();
  else {
#if defined(Z80_PROF)
    SDL_SetMainReady();
#endif
  }

  /* SDLを初期化する */
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER |
               (useJoy ? SDL_INIT_JOYSTICK : 0) |
               (buzzer != BUZZER_NONE ? SDL_INIT_AUDIO : 0))) {
    fprintf(stderr, "SDL_Init fail. %s\n", SDL_GetError());
    exit(1);
  }

#if SDL_MAJOR_VERSION == 2
  /* Windowを初期化する */
  if ((window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED, 0, 0,
                                 SDL_WINDOW_HIDDEN)) == NULL) {
    fprintf(stderr, "SDL_CreateWindow fail. %s\n", SDL_GetError());
    exit(1);
  }
#elif SDL_MAJOR_VERSION == 1
  /* ビデオ情報を得る */
  if ((videoInfo = SDL_GetVideoInfo()) == NULL) {
    fprintf(stderr, "SDL_GetVideoInfo fail. %s\n", SDL_GetError());
    exit(1);
  }
#endif

  /* アイコンを設定する */
  if ((icon = SDL_CreateRGBSurfaceFromXpm(g800icon_xpm, mask)) != NULL) {
#if SDL_MAJOR_VERSION == 2
    SDL_SetWindowIcon(window, icon);
#elif SDL_MAJOR_VERSION == 1
    SDL_WM_SetIcon(icon, mask);
#endif
  }

#if SDL_MAJOR_VERSION == 1
  /* キーリピートを設定する */
  SDL_EnableKeyRepeat(0, SDL_DEFAULT_REPEAT_INTERVAL);
#endif

  /* ジョイスティックを初期化 */
  if (useJoy)
    if ((joy = SDL_JoystickOpen(0)) == NULL)
      ;

  /* 音声を初期化する */
  if (buzzer != BUZZER_NONE) {
    audio.freq = FREQ_SOUND;
    audio.format = AUDIO_S8;
    audio.channels = 1;
    audio.samples = soundBufferSize;
    audio.callback = playSound;
    if (SDL_OpenAudio(&audio, NULL) < 0) {
      fprintf(stderr, "SDL_OpenAudio fail. %s\n", SDL_GetError());
      exit(1);
    }
    SDL_PauseAudio(0);
  }

  /* PC-E220ならばキートップを変える */
  if (machineSub == MACHINE_SUB_PCE220) {
    for (i = 0; i < sizeof(bmpKeytop) / sizeof(bmpKeytop[0]); i++) {
      bmpKeytop[i][GKEY_HAT] = bmpKeytop[i][GKEY_SQU];
      bmpKeytop[i][GKEY_SQU] = bmpKeytop[i][GKEY_SQR];
      bmpKeytop[i][GKEY_SQR] = bmpKeytopE220[i][5];
      bmpKeytop[i][GKEY_PI] = bmpKeytopE220[i][4];
      bmpKeytop[i][GKEY_NPR] = bmpKeytopE220[i][3];
      bmpKeytop[i][GKEY_CLS] = bmpKeytopE220[i][2];
      bmpKeytop[i][GKEY_FE] = bmpKeytopE220[i][1];
      bmpKeytop[i][GKEY_TEXT] = bmpKeytop[i][GKEY_BASIC];
      bmpKeytop[i][GKEY_BASIC] = bmpKeytopE220[i][0];
    }
  }

  /* PC-E200・PC-E220・PC-G815ならばキーの色を変える */
  if (machine == MACHINE_E200) {
    keyForeColor[GKEY_2NDF] = COLOR_BODY;
  } else if (machine == MACHINE_G815) {
    keyBackColor[GKEY_BASIC] = COLOR_GRAY;
    keyForeColor[GKEY_BASIC] = COLOR_MIDGREEN;
    keyBackColor[GKEY_TEXT] = COLOR_GRAY;
    keyForeColor[GKEY_TEXT] = COLOR_MIDGREEN;
    keyBackColor[GKEY_2NDF] = COLOR_GRAY;
    keyForeColor[GKEY_2NDF] = COLOR_MIDYELLOW;
    keyBackColor[GKEY_CLS] = COLOR_GRAY;
    keyForeColor[GKEY_CLS] = COLOR_MIDRED;
  }

  /* レイアウトを初期化する */
  updateLayout();

  return 0;
}

/*
        Copyright 2005 ~ 2022 maruhiro
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
